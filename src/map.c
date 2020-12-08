#include "map.h"
#include <stdio.h>
#include <stdlib.h>

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
            tmp->doors[tmp->noOfDoors] = ID2;
            tmp->noOfDoors++;
            isFound++;
        }
        if(tmp->ID == ID2){
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
    Room* tmp = m.first;
    printf("------MAP------\n");
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
Map loadRandom(int n){
    Map ret = {NULL, 0};
    for(int i = 0; i < n; i++){

    }
    return ret;
}
