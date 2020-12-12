#ifndef HEADER_FILE_2
#define HEADER_FILE_2
#define MAX_COMMAND_LENGTH 1000
#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))
#include "Game.h"


char* readInput();                                                //Reads input from user
void printInfo(Game* game);
int proccessInput(char* command, Game* game);                     //Proccesses input from user (main program's logic)
int proccessMenu(char* command, Game* game);                      //Proccesses input while in MAIN_MENU state
int proccessGame(char* command, Game* game);                      //Proccesses input while in IN_GAME state
#endif
