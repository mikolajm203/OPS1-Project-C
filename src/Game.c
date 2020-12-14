#include "Game.h"

void loadFromFile(char* path, Map* map){
    map->noOfRooms = 0;
    for(int i = 0; i < MAX_ROOMS; i++){
        map->rooms[i].noOfDoors = 0;
    }
    int f;
    if((f = open(path, O_RDONLY)) < 0)    ERR("open");
    int buf;
    int noOfRooms;
    if(read(f, &noOfRooms, sizeof(int)) == -1) ERR("read");
    for(int i = 0; i < noOfRooms; i++){
        if(read(f, &buf, sizeof(int)) == -1) ERR("read");
        addRoom(map, buf);
        printf("added room: %d\n", buf);
    }
    int connectedRooms[2];
    while(read(f, &connectedRooms[0], sizeof(int)) != 0){
        if(read(f, &connectedRooms[1], sizeof(int)) == -1) ERR("read");
        addDoor(map, connectedRooms[0], connectedRooms[1]);
        printf("added door: %d %d\n", connectedRooms[0], connectedRooms[1]);
    }
    close(f);
}
void generateMap(int n, char* path){
    struct stat s;
    if(n > MAX_ROOMS){
        printf("Too much rooms!\n");
        return;
    }
    if( stat(path,&s) == 0 )
    {
        if( s.st_mode & S_IFDIR ){
            printf("You haven't specified the name of the file!\n");
            return;
        }
    }
    Map map;
    map.noOfRooms = 0;
    int isVisited[n];
    int noOfVisited = 0;
    for(int i = 0; i < n; i++){
        isVisited[i] = 0;
        addRoom(&map, map.noOfRooms);
    }
    int currentID = 0;
    isVisited[0] = 1;
    noOfVisited++;
    while(noOfVisited != n){
        int nextID;
        nextID = rand() % n;
        if(!isVisited[nextID]){
            isVisited[nextID] = 1;
            noOfVisited++;
        }
        addDoor(&map, currentID, nextID);
        currentID = nextID;
    }
    printMap(map);
    saveMap(map, path);
}
void saveMap(Map m, char* path){
    int f;
    if((f = open(path, O_WRONLY|O_CREAT, 0777)) < 0) ERR("open in saveMap");
    if(write(f, &m.noOfRooms, sizeof(int)) == -1) ERR("write in saveMap");
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
}
void saveGame(Game game, char* path){
    int f;
    if((f = open(path, O_WRONLY|O_CREAT, 0777)) < 0) ERR("open in saveGame");
    
}
void addRoom(Map* m, int ID){
    m->rooms[m->noOfRooms].ID = ID;
    m->rooms[m->noOfRooms].noOfCurItems = 0;
    m->rooms[m->noOfRooms].noOfDestinedItems = 0;
    m->noOfRooms++;
}
void addDoor(Map* m, int ID1, int ID2){
    if(ID1 == ID2)  return;
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
    if(first->noOfDoors > MAX_DOORS || second->noOfDoors > MAX_DOORS)   ERR("Too many doors");
    if(isFound == 2){
        for(int i = 0; i < first->noOfDoors; i++){
            if(first->doors[i] == second->ID)
                return;
        }
        first->doors[first->noOfDoors] = second->ID;
        first->noOfDoors++;
        second->doors[second->noOfDoors] = first->ID;
        second->noOfDoors++;
    }
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
    ERR("putItem");
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
    ERR("setDestined");
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
    g->items = initItems(&g->map);
    g->noOfItems = (g->map.noOfRooms * 3) / 2;
    initPlayer(&g->player, &g->map);
}
void moveTo(Map* map, Player* player, int room){
    if(player->currentRoom->ID == room){
        printf("You are already in this room!\n");
        return;
    }
    for(int i = 0; i < player->currentRoom->noOfDoors; i++){
        if(player->currentRoom->doors[i] == room){
            for(int j = 0; j < map->noOfRooms; j++){
                if(map->rooms[j].ID == room){
                    player->currentRoom = &map->rooms[j];
                    return;
                }
            }
        }
    }
    printf("No doors to room %d", room);
}
void pickUp(Player* player, Item* items, int noOfItems, int itemToPickUp){
    if(player->noOfItems >= 2){
        printf("Backapck is full!!\n");
        return;
    }
    for(int i = 0; i < noOfItems; i++){
        if(items[i].ID == itemToPickUp && items[i].currentRoom == player->currentRoom->ID){
            player->backpack[player->noOfItems] = itemToPickUp;
            player->noOfItems++;
            player->currentRoom->noOfCurItems--;
            items[i].currentRoom = IN_BACKPACK;
            if(player->currentRoom->curItems[0] == itemToPickUp)
                player->currentRoom->curItems[0] = player->currentRoom->curItems[1];
            return;
        }
    }
    printf("no such item in this room!!\n");
}
void drop(Player* player, Item* items, int noOfItems, int itemToDrop){
    if(player->noOfItems <= 0){
        printf("Your backpack is empty!!!\n");
        return;
    }
    if(player->currentRoom->noOfCurItems >= 2){
        printf("There is no place in this room!!!\n");
        return;
    }
    for(int i = 0 ; i < noOfItems; i++){
        if(items[i].ID == itemToDrop){
            for(int j = 0; j < player->noOfItems; j++){
                if(player->backpack[j] == itemToDrop){
                    player->currentRoom->curItems[player->currentRoom->noOfCurItems] = itemToDrop;
                    player->currentRoom->noOfCurItems++;
                    items[i].currentRoom = player->currentRoom->ID;
                    player->noOfItems--;
                    if(player->backpack[0] == itemToDrop){
                        player->backpack[0] = player->backpack[1];
                    }
                    return;
                }
            }
        }
    }
    printf("No such item in your backpack!!\n");
}
void printPath(Game game, int room, int k){
    int pathLength = MAX_ROOMS;
    int path[MAX_ROOMS];
    SeekArg* a = (SeekArg*)malloc(sizeof(SeekArg)*k);
    pthread_mutex_t mxLength = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mxPath = PTHREAD_MUTEX_INITIALIZER;
    for(int i = 0; i < k; i++){
        a[i].pathLength = &pathLength;
        a[i].mxLength = &mxLength;
        a[i].path = &path[0];
        a[i].mxPath = &mxPath;
        a[i].seed = rand();
        a[i].destID = room;
        a[i].player.currentRoom = game.player.currentRoom;
        a[i].map = &game.map;
    }
    for(int i = 0; i < k; i++){
        if(pthread_create(&a[i].tid, NULL, seekPath, &a[i]) != 0) ERR("pthread_create");
    }
    for (int i = 0; i < k; i++) {
        if(pthread_join(a[i].tid, NULL) != 0) ERR("thread_join");
    }
    printf("Shortest path found: %d", path[0]);
    for(int i = 1; i < pathLength; i++){
        printf(" -> %d", path[i]);
    }
    printf("\n");
    free(a);
}
void* seekPath(void* args){
    SeekArg* a = args;
    int myPathLength = 0;
    int myPath[MAX_ROOMS];
    int current = a->player.currentRoom->ID;
    myPath[0] = a->player.currentRoom->ID;
    myPathLength++;
    while(current != a->destID && myPathLength < MAX_ROOMS){
        current = a->player.currentRoom->doors[rand_r(&a->seed) % a->player.currentRoom->noOfDoors];;
        moveTo(a->map, &a->player, current);
        myPath[myPathLength] = current;
        myPathLength++;
    }
    pthread_mutex_lock(a->mxLength);
    if(myPathLength < *a->pathLength){
        *a->pathLength = myPathLength;
        pthread_mutex_lock(a->mxPath);
        for(int i = 0; i < myPathLength; i++){
            a->path[i] = myPath[i];
        }
        pthread_mutex_unlock(a->mxPath);
    }
    pthread_mutex_unlock(a->mxLength);
    return NULL;
}
int checkWin(Item* items, int noOfItems){
    for(int i = 0; i < noOfItems; i++){
        if(items[i].currentRoom != items[i].destinedRoom)
            return 0;
    }
    return 1;
}
void cleanup(Game* game){
    free(game->items);
}
