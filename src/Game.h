#ifndef HEADER_FILE
#define HEADER_FILE
#define MAX_DOORS 1000
#define MAX_ITEMS 2
#define MAX_ROOMS 50

typedef enum {
    MAIN_MENU,
    IN_GAME
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

void initGame(Game* game); 
int loadFromFile(char* path, Map* map);         // Returns a map loaded from a file. Returns NULL on failure.
Map* loadFromDir(char* path_d, char* path_f);   /* generates a map that mirrors a directory structure of path_d and its subdirectories.
                                                Every directory is a room and the rooms are connected with their parent directories and subdirectories.
                                                The created map is written to path-f. */
Map* loadRandom(int n);                         // Returns a random map of n rooms and writes the result in path.
int saveMap(Map m, char* path);                 // Saves map to the path. Returns 0 on succes and -1 otherwise
int addRoom(Map* m, int ID);                    // Adds a room to the Map. Returns 0 on succesful addition
int addDoor(Map* m, int ID1, int ID2);          // Adds a door to the Room. Returns 0 on succesful addition. Does nothing if door already exists
void printRoom(Room r);                         // Prints info about all the Rooms in the Map
void printMap(Map m);                           // Prints info about the Room
void deleteMap(Map* m);                         // Frees all allocated memory
int putItem(Map* m, int roomID, int itemID);
int setDestined(Map* map, int roomID,int itemID);
void initPlayer(Player* player, Map* map);
Item* initItems(Map* m);                       // Populates map with items
void moveTo(Map* map, Player* player,int room);
void pickUp(Player* player, Item* items, int noOfItems, int itemToPickUp);
void drop(Player* player, Item* items, int noOfItems, int itemToDrop);
#endif
