#ifndef HEADER_FILE
#define HEADER_FILE
#define MAX_DOORS 1000

typedef struct Room_ Room;                      // Representation of a Room with adjacency list(int doors[MAX_DOORS])
struct Room_{                       
    int ID;
    int doors[MAX_DOORS];
    int noOfDoors;
    Room* next;
};
typedef struct Map_ Map;                        // Linked list of rooms(our graph)
struct Map_{                        
    Room* first;
    int noOfRooms;
};
Map loadFromFile(char* file);                   // Returns a map loaded from a file.
Map loadFromDir(char* path_d, char* path_f);    /* generates a map that mirrors a directory structure of path_d and its subdirectories.
                                                Every directory is a room and the rooms are connected with their parent directories and subdirectories.
                                                The created map is written to path-f. */
Map loadRandom(int n);              // Returns a random map of n rooms and writes the result in path.
int addRoom(Map* m, int ID);                    // Adds a room to the Map. Returns 0 on succesful addition
int addDoor(Map* m, int ID1, int ID2);          // Adds a door to the Room. Returns 0 on succesful addition
void printRoom(Room r);                         // Prints info about all the Rooms in the Map
void printMap(Map m);                           // Prints info about the Room
void deleteMap(Map m);                          // Frees all allocated memory

#endif
