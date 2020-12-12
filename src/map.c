#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int addRoom(Map* m, int ID){
    Room* toAdd =(Room*)malloc(sizeof(Room));
    if(toAdd == NULL) return -1;
    toAdd->ID = ID;
    toAdd->next = NULL;
    toAdd->noOfDoors = 0;
    if(m->noOfRooms == 0){
        m->first = toAdd;
        m->noOfRooms++;
        return 0;
    }
    Room* cur = m->first;
    Room* next = cur->next;
    while(next != NULL){
        cur = cur->next;
        next = next->next;
    }
    m->noOfRooms++;
    cur->next = toAdd;
    next = NULL;
    return 0;
}
int addDoor(Map* m, int ID1, int ID2){
    if(m->noOfRooms == 0) return -1;
    int isFound = 0; // variable to track if ID1 or ID2 were found
    Room* tmp = m->first;
    while(tmp != NULL){
        if(tmp->ID == ID1){
            for(int i = 0; i < tmp->noOfDoors; i++){
                if(tmp->doors[i] == ID2)
                    return 0;
            }
            tmp->doors[tmp->noOfDoors] = ID2;
            tmp->noOfDoors++;
            isFound++;
        }
        if(tmp->ID == ID2){
            for(int i = 0; i < tmp->noOfDoors; i++){
                if(tmp->doors[i] == ID1)
                    return 0;
            }
            tmp->doors[tmp->noOfDoors] = ID1;
            tmp->noOfDoors++;
            isFound++;
        }
        tmp = tmp->next;
    }
    if(isFound != 2) return -1;
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
    if(m.first == NULL){
        printf("empty map\n");
        return;
    }
    Room* tmp = m.first;
    while(tmp != NULL){
        printRoom(*tmp);
        tmp = tmp->next;
    }
}
void deleteMap(Map m){
    if(m.noOfRooms == 1){
        free(m.first);
        return;
    }
    Room* cur = m.first;
    Room* next = cur->next;
    while(next != NULL){
        free(cur);
        cur = next;
        next = cur->next;
    }
    free(cur);
    return;
}
Map* loadRandom(int n){
    Map* ret = (Map*)malloc(sizeof(Map));
    if (ret == NULL)    return NULL;
    for(int i = 0; i < n; i++){

    }
    return ret;
}
int loadFromFile(char* path, Map* map){
    map->first = NULL;
    map->noOfRooms = 0;
    int f;
    if((f = open(path, O_RDONLY)) < 0)    return -1;
    int buf;
    int noOfRooms;
    if(read(f, &noOfRooms, sizeof(int)) == -1) return -1;
    for(int i = 0; i < noOfRooms; i++){
        if(read(f, &buf, sizeof(int)) == -1) return -1;
        if(addRoom(map, buf) < 0) return -1;
    }
    int connectedRooms[2];
    while(read(f, &connectedRooms[0], sizeof(int)) != 0){
        if(read(f, &connectedRooms[1], sizeof(int)) == -1) return -1;
        if(addDoor(map, connectedRooms[0], connectedRooms[1]) < 0) return -1;
    }
    close(f);
    return 0;
}
int saveMap(Map m, char* path){
    int f;
    if((f = open(path, O_WRONLY|O_CREAT, 0777)) < 0) return -1;
    if(write(f, &m.noOfRooms, sizeof(int)) == -1) return -1;
    Room* tmp = m.first;
    while(tmp != NULL){
        if(write(f, &tmp->ID, sizeof(int)) == -1) return -1;
        tmp = tmp->next;
    }
    tmp = m.first;
    while(tmp != NULL){
        for(int i = 0; i < tmp->noOfDoors; i++){
            if(write(f, &tmp->ID, sizeof(int)) == -1) return -1;
            if(write(f, &tmp->doors[i], sizeof(int)) == -1) return -1;
        }
        tmp = tmp->next;
    }
    close(f);
    return 0;
}
