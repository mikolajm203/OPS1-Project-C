#include "Input.h"

char* readInput(){
    char* ret = (char*)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
    scanf("%s", ret);
    return ret;
}
void printInfo(Game* game){
    switch (game->state)
    {
    case MAIN_MENU:
        printf("\nAvailable commands:\n");
        printf("1. read-map \"path\"\n");
        printf("2. generate-random-map \"Number of rooms\" \"path\"\n");
        printf("3. load-game \"path\"\n");
        printf("4. map-from-dir-tree \"path-d\" \"path-f\"\n");
        printf("5. exit\n");
        break;
    case IN_GAME:
        printf("\nAvailable commands: \n");
        printf("1. move-to \"room\"\n");
        printf("2. pick-up \"item\"\n");
        printf("3. drop \"item\"\n");
        printf("4. find-path \"room\" \"number of threads\"\n");
        printf("5. save \"path\"\n");
        printf("6. swap\n");
        printf("7. quit\n\n");
        printf("You are in room: %d\n", game->player.currentRoom->ID);
        printf("Doors to: ");
        for(int i = 0; i < game->player.currentRoom->noOfDoors; i++){
            printf("%d ", game->player.currentRoom->doors[i]);
        }
        printf("\n\nItems in the room:\n");
        for(int i = 0; i < game->noOfItems; i++){
            if(game->items[i].currentRoom == game->player.currentRoom->ID)
                printf("%d( destined room: %d)\n", game->items[i].ID, game->items[i].destinedRoom);
        }
        printf("\nYour backpack: [");
        if(game->player.noOfItems == 0)
            printf("EMPTY");
        for(int i = 0; i < game->player.noOfItems; i++)
            printf(" %d", game->player.backpack[i]);
        printf("]\n");
        break;
    case POST_GAME:
        printf("\n---------------------\n");
        printf("------YOU WIN!!------\n");
        printf("---------------------\n");
        printf("Type \" exit\" to close the game\n");
        break;
    default:
        break;
    }
}
void proccessInput(char* command, Game* game, pthread_t swaptid, pthread_t savetid){
    switch (game->state)
    {
    case MAIN_MENU:
        proccessMenu(command, game);
        break;
    case IN_GAME:
        proccessGame(command, game, swaptid, savetid);
        break;
    case POST_GAME:
        proccessPost(command, game);
        break;
    default:
        ERR("strange state");
        break;
    }
}

void proccessMenu(char* command, Game* game){
    char* path = (char*)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
    if(strcmp(command, "exit") == 0){
        pthread_mutex_lock(game->mxGame);
        printf("closing the game..\n");
        free(command);
        pthread_mutex_unlock(game->mxGame);
        exit(EXIT_SUCCESS);
    }
    else if(strcmp(command, "read-map") == 0){
        scanf("%s", path);
        loadFromFile(path, &game->map);
        initGame(game);
    }
    else if(strcmp(command, "generate-random-map") == 0){
        int n;
        scanf("%d", &n);
        scanf("%s", path);
        printf("generating map %d %s\n", n, path);
        generateMap(n, path);
    }
    else if(strcmp(command, "load-game") == 0){
        scanf("%s", path);
        loadGame(game, path);
        game->state = IN_GAME;
    }
    else if(strcmp(command, "map-from-dir-tree") == 0){
        char* path_f = (char*)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
        scanf("%s", path);
        scanf("%s", path_f);
        loadFromDir(&game->map, path, path_f);
        printMap(game->map);
        free(path_f);
    }
    else{
        printf("Wrong command!.\n");
    }
    free(path);
}
void proccessGame(char* command, Game* game, pthread_t swaptid, pthread_t savetid){
    if(strcmp(command, "quit") == 0){
        pthread_mutex_lock(game->mxGame);
        printf("returning to main menu..\n");
        game->state = MAIN_MENU;
        pthread_mutex_unlock(game->mxGame);
    }
    else if(strcmp(command, "move-to") == 0){
        int roomID;
        scanf("%d", &roomID);
        pthread_mutex_lock(game->mxGame);
        moveTo(&game->map, &game->player, roomID);
        pthread_mutex_unlock(game->mxGame);
    }
    else if(strcmp(command, "pick-up") == 0){
        int pickID;
        scanf("%d", &pickID);
        pthread_mutex_lock(game->mxGame);
        pickUp(&game->player, game->items, game->noOfItems, pickID);
        pthread_mutex_unlock(game->mxGame);
    }
    else if(strcmp(command, "drop") == 0){
        int dropID;
        scanf("%d" ,&dropID);
        pthread_mutex_lock(game->mxGame);
        drop(&game->player, game->items, game->noOfItems, dropID);
        pthread_mutex_unlock(game->mxGame);
    }
    else if(strcmp(command, "find-path") == 0){
        int room, k;
        scanf("%d", &room);
        scanf("%d", &k);
        pthread_mutex_lock(game->mxGame);
        printPath(*game, room, k);
        pthread_mutex_unlock(game->mxGame);
    }
    else if(strcmp(command, "save") == 0){
        char path[MAX_COMMAND_LENGTH];
        scanf("%s", path);
        pthread_kill(savetid, SIGUSR1);
        pthread_mutex_lock(game->mxGame);
        saveGame(*game, path);
        pthread_mutex_unlock(game->mxGame);
    }
    else if(strcmp(command, "swap") == 0){
        if(pthread_kill(swaptid, SIGUSR1) != 0) ERR("pthread_kill");
    }
    else{
        printf("Wrong command!\n");
    }
}
void proccessPost(char* command, Game* game){
    if(strcmp(command, "return") == 0){
        game->state = MAIN_MENU;
        return;
    }
    else if(strcmp(command, "exit") == 0){
        printf("closing the game...\n");
        exit(EXIT_SUCCESS);
    }
    else{
        printf("Wrong command!\n");
    }
}
