#ifndef HEADER_FILE_2
#define HEADER_FILE_2
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define MAX_COMMAND_LENGTH 1000
#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))
#include "Game.h"


char* readInput();                                                 //Reads input from user
void printInfo(Game* game);                                        //Prints info to the user about the game
void proccessInput(char* command, Game* game);                     //Proccesses input from user (main program's logic)
void proccessMenu(char* command, Game* game);                      //Proccesses input while in MAIN_MENU state
void proccessGame(char* command, Game* game);                      //Proccesses input while in IN_GAME state
void proccessPost(char* command, Game* game);                      //Proccesses inpit while in POST_GAME state
#endif
