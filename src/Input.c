#include "Input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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
        printf("2. exit\n");
        break;
    case IN_GAME:
        printf("\nAvailable commands: \n");
        printf("1. quit\n");
        printf("2. move-to \"room\"\n");
        printf("3. pick-up \"item\"\n");
        printf("4. drop \"item\"\n\n");
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
    default:
        break;
    }
}
int proccessInput(char* command, Game* game){
    switch (game->state)
    {
    case MAIN_MENU:
        proccessMenu(command, game);
        break;

    case IN_GAME:
        proccessGame(command, game);
        break;

    default:
        return -1;
        break;
    }
    return 0;
}

int proccessMenu(char* command, Game* game){
    char* path = (char*)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
    if(strcmp(command, "exit") == 0){
        printf("closing the game..\n");
        free(command);
        exit(EXIT_SUCCESS);
    }
    else if(strcmp(command, "read-map") == 0){
        scanf("%s", path);
        if( access( path, F_OK ) == -1)
        {
            printf("no such file in path: %s\n", path);
            return 0;
        }
        loadFromFile(path, &game->map);
        initGame(game);
        game->state = IN_GAME;
    }
    else{
        printf("Wrong command!.\n");
    }
    free(path);
    return 0;
}
int proccessGame(char* command, Game* game){
    if(strcmp(command, "quit") == 0){
        printf("returning to main menu..\n");
        game->state = MAIN_MENU;
    }
    else if(strcmp(command, "move-to") == 0){
        int roomID;
        scanf("%d", &roomID);
        moveTo(&game->map, &game->player, roomID);
    }
    else if(strcmp(command, "pick-up") == 0){
        int pickID;
        scanf("%d", &pickID);
        pickUp(&game->player, game->items, game->noOfItems, pickID);
    }
    else if(strcmp(command, "drop") == 0){
        int dropID;
        scanf("%d" ,&dropID);
        drop(&game->player, game->items, game->noOfItems, dropID);
    }
    else{
        printf("Wrong command!.\n");
    }
    return 0;
}
