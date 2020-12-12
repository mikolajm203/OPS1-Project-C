#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Game.h"
#include "Input.h"
#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))


int main(int argc, char** argv){
    Game game;
    game.state = MAIN_MENU;
    while(1){
        printInfo(&game);
        char* command = readInput();
        proccessInput(command, &game);
    }
    return EXIT_SUCCESS;
}
