#include "controls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int init(){
    printf("INIT\n");
    return 0;
}

char* readInput(){
    char* ret = (char*)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
    scanf("%s", ret);
    return ret;
}

int proccessInput(GAME_STATE* state, char* command, Map* map){
    switch (*state)
    {
    case MAIN_MENU:
        proccessMenu(command, map, state);
        break;

    case IN_GAME:
        break;

    default:
        return -1;
        break;
    }
    return 0;
}

int proccessMenu(char* command, Map* map, GAME_STATE* state){
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
        if(loadFromFile(path, map) == -1)   return -1;
     //   *state = IN_GAME;
    }
    else if(strcmp(command, "print") == 0){
        printMap(*map);
    }
    else{
        printf("Wrong command!.\n");
        command = readInput();
    }
    free(path);
    return 0;
}
