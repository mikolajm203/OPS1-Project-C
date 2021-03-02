#ifndef HEADER_FILE
#define HEADER_FILE
#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))
#define MAX_DOORS 50
#define MAX_ITEMS 2
#define MAX_ROOMS 50
#define IN_BACKPACK 1000
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>


typedef enum {
    MAIN_MENU,
    IN_GAME,
    POST_GAME
}GAME_STATE;

typedef struct Item_ Item;
struct Item_{                                   //Item struct to store all neccessary data
    int ID;                 
    int currentRoom;        
    int destinedRoom;      
};

typedef struct Room_ Room;                      // Representation of a Room with adjacency list(int doors[MAX_DOORS])
struct Room_{                       
    int ID;                 
    int curItems[MAX_ITEMS];
    int noOfCurItems;       
    int noOfDestinedItems;  
    int doors[MAX_DOORS];   
    int noOfDoors;          
};

typedef struct Player_ Player;                  //Player struct to store all neccessary data
struct Player_{
    int backpack[MAX_ITEMS];
    int noOfItems;          
    Room* currentRoom;      
};

typedef struct Map_ Map;                        // Graph representation of a map
struct Map_{
    int noOfRooms;                        
    Room rooms[MAX_ROOMS];
};

typedef struct Game_ Game;                      
struct Game_{
    Map map;
    GAME_STATE state;
    Player player;
    int noOfItems;
    Item* items;
    pthread_mutex_t* mxGame;
};
typedef struct SeekArg_{                        // struct passed to function seekPath
    pthread_t tid;
    int* pathLength;
    pthread_mutex_t* mxLength;
    int* path;
    pthread_mutex_t* mxPath;
    unsigned int seed;
    int destID;
    Player player;
    Map* map;
}SeekArg;
ssize_t bulk_write(int fd, void *buf, size_t count);
ssize_t bulk_read(int fd, void *buf, size_t count);
void loadFromFile(char* path, Map* map);                                    // loads map from file and starts the game
void generateMap(int n, char* path);                                        // generates random map and saves it in path
void loadFromDir(Map* m, char* path_d, char* path_f);                       // saves ma generated from path_d to path_f
void DirToMap(Map* m, char* path_d, int prevID);                            /* generates a map that mirrors a directory structure of path_d and its subdirectories.
                                                                            Every directory is a room and the rooms are connected with their parent directories and
                                                                            subdirectories.*/
void saveMap(Map* m, char* path);                                           // Saves map to the path.
void saveGame(Game game, char* path);                                       // Saves game to the path
void loadGame(Game* game, char* path);                                      // Loads game from the path
void addRoom(Map* m, int ID);                                               // Adds a room to the Map.
void addDoor(Map* m, int ID1, int ID2);                                     // Adds a door to the Room. Does nothing if door already exists.
void printRoom(Room r);                                                     // Prints info about the Room
void printMap(Map m);                                                       // Prints info about all the Rooms in the Map
Item* initItems(Map* m);                                                    // Populates map with items
int putItem(Map* m, int roomID, int itemID);                                // Returns true if item can be placed in roomID.
int setDestined(Map* map, int roomID,int itemID);                           // Returns true it item's destination can be roomID
void initPlayer(Player* player, Map* map);                                  // Initializes Player struct. Puts him in random room
void initGame(Game* game);                                                  // Initializes Gamestruct
void moveTo(Map* map, Player* player,int room);                             // Moves player to room
void pickUp(Player* player, Item* items, int noOfItems, int itemToPickUp);  // Picks up itemToPickUp if possible
void drop(Player* player, Item* items, int noOfItems, int itemToDrop);      //Drops itemToDrop if possible
void printPath(Game game, int room, int k);                                 //Prints moderately short path to room using k threads
void* seekPath(void* args);                                                 //Function for threads in printPath
void swapItems(Game* g, int ID1, int ID2);                                  //Swaps location of items ID1 and ID2
int checkWin(Item* items, int noOfItems);                                   //checks if player has put items in correct rooms
#endif
