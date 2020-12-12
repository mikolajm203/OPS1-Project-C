#include "Game.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))

int addRoom(Map* m, int ID){
    m->rooms[m->noOfRooms].ID = ID;
    m->rooms[m->noOfRooms].noOfCurItems = 0;
    m->rooms[m->noOfRooms].noOfDestinedItems = 0;
    m->noOfRooms++;
    return 0;
}
int addDoor(Map* m, int ID1, int ID2){
    if(m->noOfRooms < 2) ERR("addDoor: Wrong number of rooms");
    int isFound = 0; // variable to track if ID1 or ID2 were found
    Room* first;
    Room* second;
    for(int i = 0; i < m->noOfRooms; i++){
        if(m->rooms[i].ID == ID1){
            first = &m->rooms[i];
            isFound++;
        }
        if(m->rooms[i].ID == ID2){
            second = &m->rooms[i];
            isFound++;
        }
    }

    if(isFound == 2){
        for(int i = 0; i < first->noOfDoors; i++){
            if(first->doors[i] == second->ID)
                return 0;
        }
        first->doors[first->noOfDoors] = second->ID;
        first->noOfDoors++;
        second->doors[second->noOfDoors] = first->ID;
        second->noOfDoors++;
    }
    return 0;
}
void printRoom(Room r){
    printf("Room ID: %d\n Doors to:", r.ID);
    for(int i = 0 ; i < r.noOfDoors; i++){
        printf(" %d", r.doors[i]);
    }
    printf("\n");
}
void printMap(Map m){
    printf("------MAP------\n");
    if(m.noOfRooms == 0){
        printf("empty map\n");
        return;
    }
    for(int i = 0; i < m.noOfRooms; i++){
        printRoom(m.rooms[i]);
    }
}
Map* loadRandom(int n){
    Map* ret = (Map*)malloc(sizeof(Map));
    if (ret == NULL)    return NULL;
    for(int i = 0; i < n; i++){

    }
    return ret;
}
int loadFromFile(char* path, Map* map){
    map->noOfRooms = 0;
    int f;
    if((f = open(path, O_RDONLY)) < 0)    ERR("open");
    int buf;
    int noOfRooms;
    if(read(f, &noOfRooms, sizeof(int)) == -1) ERR("read");
    for(int i = 0; i < noOfRooms; i++){
        if(read(f, &buf, sizeof(int)) == -1) ERR("read");
        if(addRoom(map, buf) < 0) ERR("read");
    }
    int connectedRooms[2];
    while(read(f, &connectedRooms[0], sizeof(int)) != 0){
        if(read(f, &connectedRooms[1], sizeof(int)) == -1) ERR("read");
        if(addDoor(map, connectedRooms[0], connectedRooms[1]) < 0);
    }
    close(f);
    return 0;
}
int saveMap(Map m, char* path){
    int f;
    if((f = open(path, O_WRONLY|O_CREAT, 0777)) < 0) return -1;
    if(write(f, &m.noOfRooms, sizeof(int)) == -1) return -1;
    for(int i = 0; i < m.noOfRooms; i++){
        if(write(f, &m.rooms[i].ID, sizeof(int)) == -1) ERR("write");
    }
    for(int i = 0; i < m.noOfRooms; i++){
        for(int j = 0 ; j < m.rooms[i].noOfDoors; j++){
            if(write(f, &m.rooms[i].ID, sizeof(int)) == -1) ERR("write");
            if(write(f, &m.rooms[i].doors[j], sizeof(int)) == -1) ERR("write");
        }
    }
    close(f);
    return 0;
}
Item* initItems(Map* map){
    int noOfItems = (map->noOfRooms) * 3 / 2;
    Item* items = (Item*) malloc(sizeof(Item) * noOfItems);
    for(int i = 0; i < noOfItems; i++){
        items[i].ID = i;
    }
    for(int i = 0; i < noOfItems; i++){
        do{
            items[i].currentRoom = map->rooms[rand() % map->noOfRooms].ID;
        }while(!putItem(map, items[i].currentRoom, items[i].ID));
        do{
            items[i].destinedRoom = map->rooms[rand() % map->noOfRooms].ID;
        }while(!setDestined(map, items[i].destinedRoom, items[i].ID));
    }
    return items;
}
int setDestined(Map* map, int roomID,int itemID){
    for(int i = 0; i < map->noOfRooms; i++){
        if(map->rooms[i].ID == roomID){
            if(map->rooms[i].noOfDestinedItems < 2){
                map->rooms[i].noOfDestinedItems++;
                return 1;
            }
            else
                return 0;
        }
    }
    return 0;
}
int putItem(Map* map, int roomID, int itemID){
    for(int i = 0; i < map->noOfRooms; i++){
        if(map->rooms[i].ID == roomID){
            if(map->rooms[i].noOfCurItems < 2){
                map->rooms[i].curItems[map->rooms[i].noOfCurItems] = itemID;
                map->rooms[i].noOfCurItems++;
                return 1;
            }
            else
                return 0;
        }
    }
    printf("GOWNO\n");
    return 1;
}
void initPlayer(Player* player, Map* map){
    player->noOfItems = 0;
    int startID = map->rooms[rand() % map->noOfRooms].ID;
    for(int i = 0 ; i < map->noOfRooms; i++){
        if(startID == map->rooms[i].ID){
            player->currentRoom = &map->rooms[i];
        }
    }
}
void initGame(Game* g){
    srand(time(NULL));
    g->items = initItems(&g->map);
    g->noOfItems = (g->map.noOfRooms * 3) / 2;
    initPlayer(&g->player, &g->map);
}
void deleteMap(Map* map){
    for(int i = 0; i < map->noOfRooms; i++){
        map->rooms[i].noOfDoors = 0;
    }
    map->noOfRooms = 0;
}
