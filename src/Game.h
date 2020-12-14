#ifndef HEADER_FILE
#define HEADER_FILE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <pthread.h>
#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))
#define MAX_DOORS 50
#define MAX_ITEMS 2
#define MAX_ROOMS 50
#define IN_BACKPACK 1000

typedef enum {
    MAIN_MENU,
    IN_GAME,
    POST_GAME
}GAME_STATE;

typedef struct Item_ Item;
struct Item_{                                   //Item struct to store all neccessary data
    int ID;                 //item ID
    int currentRoom;        //room ID where that item lies
    int destinedRoom;         //room ID where that item belongs
};

typedef struct Room_ Room;                      // Representation of a Room with adjacency list(int doors[MAX_DOORS])
struct Room_{                       
    int ID;                 //ID of the room
    int curItems[MAX_ITEMS];//array of items' ID in the room
    int noOfCurItems;       //number of items in the room
    int noOfDestinedItems;  //number of items that come from this room
    int doors[MAX_DOORS];   //array of rooms' IDs connected to this room
    int noOfDoors;          //Number of dors in this room
};

typedef struct Player_ Player;                  //Player struct to store all neccessary data
struct Player_{
    int backpack[MAX_ITEMS];//array of items' IDs in player's backpack
    int noOfItems;          //number of items in the backpack
    Room* currentRoom;        //room ID where player is right now
};

typedef struct Map_ Map;                        // Linked list of rooms(our graph)
struct Map_{                        
    Room rooms[MAX_ROOMS];
    int noOfRooms;
};

typedef struct Game_ Game;
struct Game_{
    Map map;
    GAME_STATE state;
    Player player;
    Item* items;
    int noOfItems;
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

void loadFromFile(char* path, Map* map);                                    // loads map from file and starts the game
void generateMap(int n, char* path);                                        // generates random map and saves it in path
Map* loadFromDir(char* path_d, char* path_f);                               /* generates a map that mirrors a directory structure of path_d and its subdirectories.
                                                                            Every directory is a room and the rooms are connected with their parent directories and
                                                                            subdirectories. The created map is written to path-f. */
void saveMap(Map m, char* path);                                            // Saves map to the path.
void saveGame(Game game, char* path);                                       // 
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
int checkWin(Item* items, int noOfItems);                                   //checks if player has put items in correct rooms
void cleanup(Game* game);
#endif
