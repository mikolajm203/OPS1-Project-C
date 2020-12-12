#ifndef HEADER_FILE_2
#define HEADER_FILE_2
#define MAX_COMMAND_LENGTH 1000
#include "map.h"

typedef enum {
    MAIN_MENU,
    IN_GAME
}GAME_STATE;
int init();                                                       //Initializes program
char* readInput();                                                //Reads input from user
int proccessInput(GAME_STATE* state, char* command, Map* map);    //Proccesses input from user (main program's logic)
int proccessMenu(char* command, Map* map, GAME_STATE* state);     //Proccesses input while in MAIN_MENU state
int proccessGame(char* command);                                  //Proccesses input while in IN_GAME state
#endif
