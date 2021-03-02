//------------------------------------------------------------------------
//I declare that this piece of work which is the basis for recognition of 
//achieving learning outcomes in the OPS1 course was completed on my own.
//Mikolaj Stanczyk 305798
//------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "Game.h"
#include "Input.h"
#define MAX_BACKUP_PATH 200
#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))

typedef struct SwapArg_{    //Struct passed as argument to swapThread
    Game* g;
    pthread_t tid;
    sigset_t *pMask;
    unsigned int seed;
}SwapArg;
typedef struct SaveArg_{    //Struct passed as argument to saveThread
    Game* g;
    pthread_t tid;
    sigset_t *pMask;
    char* path;
}SaveArg;
void* swapThread(void* arg);
char* getBackup(int argc, char** argv);
void* saveThread(void* arg);
int main(int argc, char** argv){
    char* backupPath = getBackup(argc, argv);
    srand(time(NULL));
    Game game;
    game.state = MAIN_MENU;
    pthread_mutex_t mxGame = PTHREAD_MUTEX_INITIALIZER;
    game.mxGame = &mxGame;
    game.map.noOfRooms = 0;
    //---------------setting up autosave thread--------------------
    sigset_t oldSaveMask, newSaveMask;
    sigemptyset(&newSaveMask);
    sigaddset(&newSaveMask, SIGUSR1);
    sigaddset(&newSaveMask, SIGUSR2);
    if (pthread_sigmask(SIG_BLOCK, &newSaveMask, &oldSaveMask)) ERR("SIG_BLOCK error");
    SaveArg sarg;
    sarg.pMask = &newSaveMask;
    sarg.g = &game;
    sarg.path = backupPath;
    pthread_attr_t threadAttr;
    if(pthread_attr_init(&threadAttr)) ERR("Couldn't create pthread_attr_t");
    if(pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED)) ERR("Couldn't setdetachsatate on pthread_attr_t");
    if(pthread_create(&sarg.tid, &threadAttr, saveThread, &sarg) != 0) ERR("pthread_create"); 
    //-------------end of setting up autosave thread---------------
    //---------------setting up swapping items thread--------------------
    sigset_t oldSwapMask, newSwapMask;
    sigemptyset(&newSwapMask);
    sigaddset(&newSwapMask, SIGUSR1);
    sigaddset(&newSwapMask, SIGUSR2);
    if (pthread_sigmask(SIG_BLOCK, &newSwapMask, &oldSwapMask)) ERR("SIG_BLOCK error");
    SwapArg arg;
    arg.g = &game;
    arg.pMask = &newSwapMask;
    arg.seed = rand();
    if(pthread_create(&arg.tid, &threadAttr, swapThread, &arg) != 0) ERR("pthread_create");
    //------------end of setting up swapping items thread--------------------
    //---------------main game loop-----------------------
    while(1){
        printInfo(&game);
        char* command = readInput();
        proccessInput(command, &game, arg.tid, sarg.tid);
        if(game.state == IN_GAME && checkWin(game.items, game.noOfItems))  game.state = POST_GAME;
        free(command);
    }
    //--------------end of main game loop ----------------------
    //------------- freeing space and destroying mutexes --------
    pthread_mutex_destroy(&mxGame);
    free(game.items);
    //--------end of freeing space and destroying mutexes --------
    return EXIT_SUCCESS;
}
void* swapThread(void* arg){
    SwapArg* a = arg;
    int sigNo;
    for(;;){
        if(sigwait(a->pMask, &sigNo) && a->g->state == IN_GAME) ERR("sigwait failed.");
        if(sigNo == SIGUSR1){  
            int item1ID;
            int item2ID;
            pthread_mutex_lock(a->g->mxGame);
            item1ID = a->g->items[rand_r(&a->seed) % a->g->noOfItems].ID;
            do{
                item2ID = a->g->items[rand_r(&a->seed) % a->g->noOfItems].ID;
            }while(item1ID == item2ID);
            swapItems(a->g, item1ID, item2ID);
            pthread_mutex_unlock(a->g->mxGame);
        }      
    }
    return NULL;
}
char* getBackup(int argc, char** argv){
    char c;
    char* backupPath;
    if((backupPath = getenv("HOME")) == NULL) ERR("No $HOME");
    strcat(backupPath, "/.game_autosave");
    while((c = getopt(argc, argv, "d:")) != -1){
        if(c == 'd'){
            backupPath = optarg;
            return backupPath;
        }
        else ERR("wrong flags");
    }
    if(getenv("GAME_AUTOSAVE") != NULL){
        backupPath = getenv("GAME_AUTOSAVE");
    }
    return backupPath;
}
void* saveThread(void* arg){
    SaveArg* a = (SaveArg*) arg;
    while(a->g->state != POST_GAME){
        if(sleep(60) == 0){
            if(a->g->state == IN_GAME){
                pthread_mutex_lock(a->g->mxGame);               
                saveGame(*a->g, a->path);
                printf("\nAutomatically saved game in %s\n", a->path);
                pthread_mutex_unlock(a->g->mxGame);
            }
        }
        else{
            printf("Recieved SIGUSR1\n");
            continue;
        }
    }
    return NULL;
}
