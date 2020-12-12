#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "map.h"
#include "controls.h"
#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))

int main(int argc, char** argv){
    Map map = {NULL, 0};
    GAME_STATE state = MAIN_MENU;
    while(1){
        char* command = readInput();
        proccessInput(&state, command, &map);
    }
    printMap(map);
    return EXIT_SUCCESS;
}
