#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))
enum GAME_STATE{
    MAIN_MENU,
    IN_GAME
};
int main(int argc, char** argv){
    Map m = {NULL, 0};
    for(int i = 0; i < 5; i++){
        addRoom(&m, i);
    }
    addDoor(&m, 0, 1);
    addDoor(&m, 0, 4);
    printMap(m);
    deleteMap(m);
    return EXIT_SUCCESS;
}
