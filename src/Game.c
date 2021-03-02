#include "Game.h"
ssize_t bulk_read(int fd, void *buf, size_t count){
        ssize_t c;
        ssize_t len=0;
        do{
                c=TEMP_FAILURE_RETRY(read(fd,buf,count));
                if(c<0) return c;
                if(c==0) return len; //EOF
                buf+=c;
                len+=c;
                count-=c;
        }while(count>0);
        return len ;
}
ssize_t bulk_write(int fd, void *buf, size_t count){
        ssize_t c;
        ssize_t len=0;
        do{
                c=TEMP_FAILURE_RETRY(write(fd,buf,count));
                if(c<0) return c;
                buf+=c;
                len+=c;
                count-=c;
        }while(count>0);
        return len ;
}
void loadFromFile(char* path, Map* map){
    map->noOfRooms = 0;
    for(int i = 0; i < MAX_ROOMS; i++){
        map->rooms[i].noOfDoors = 0;
    }
    int f;
    if(TEMP_FAILURE_RETRY((f = open(path, O_RDONLY))) < 0)    ERR("open");
    if(bulk_read(f, &map->noOfRooms, sizeof(int)));
    for(int i = 0; i < MAX_ROOMS; i++){
        if(bulk_read(f, &map->rooms[i].ID, sizeof(int)) < 0) ERR("read");
        for(int j = 0; j < MAX_ITEMS; j++){
            if(bulk_read(f, &map->rooms[i].curItems[j], sizeof(int)) < 0) ERR("read");
        }
        if(bulk_read(f, &map->rooms[i].noOfCurItems, sizeof(int)) < 0) ERR("read");
        if(bulk_read(f, &map->rooms[i].noOfDestinedItems, sizeof(int)) < 0) ERR("read");
        for(int j = 0; j < MAX_DOORS; j++){
            if(bulk_read(f, &map->rooms[i].doors[j], sizeof(int)) < 0) ERR("read");
        }
        if(bulk_read(f, &map->rooms[i].noOfDoors, sizeof(int)) < 0) ERR("read");
    }
    if(TEMP_FAILURE_RETRY(close(f) == -1)) ERR("close");
}
void generateMap(int n, char* path){
    struct stat s;
    if(n > MAX_ROOMS){
        printf("Too much rooms!\n");
        return;
    }
    stat(path,&s);
    if(S_ISDIR(s.st_mode)) {
        printf("You haven't specified the name of the file!\n");
        return;
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
    for(int i = 0; i < map.noOfRooms; i++){
        map.rooms[i].noOfCurItems = 0;
        map.rooms[i].noOfDestinedItems = 0;
    }
    saveMap(&map, path);
}
void loadFromDir(Map* m, char* path_d, char* path_f){
    DirToMap(m, path_d, 0);
    saveMap(m, path_f);
}
void DirToMap(Map* m, char* path_d, int prevID){
    DIR* dir;
    struct dirent* direntry;
    if((dir = opendir(path_d)) == NULL){
        return;
    }
    if(m->noOfRooms == 0)
        addRoom(m, m->noOfRooms);
    char* newPath = path_d;
    while((direntry = readdir(dir)) != NULL){
        if(strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0){
            int curID = m->noOfRooms;
            addRoom(m, m->noOfRooms);
            strcat(newPath, "/");
            strcat(newPath, direntry->d_name);
            addDoor(m, prevID, curID);
            DirToMap(m, newPath, curID);
            char* tmp;
            tmp = strstr(newPath, direntry->d_name);
            tmp--;
            *tmp = '\0';
        }
    }
}
void saveMap(Map* m, char* path){
    int f;
    if(TEMP_FAILURE_RETRY(f = open(path, O_WRONLY|O_CREAT, 0666)) < 0) ERR("open in saveMap");
    if(bulk_write(f, m, sizeof(Map)) < 0) ERR("write");
    if(TEMP_FAILURE_RETRY(close(f)) < 0) ERR("close");
}
void saveGame(Game game, char* path){
    int f;
    if(TEMP_FAILURE_RETRY((f = open(path, O_WRONLY|O_CREAT, 0666))) < 0) ERR("open in saveGame");
    // ----------- MAP --------------
    if(bulk_write(f, &game.map, sizeof(Map)) < 0) ERR("write in saveMap");
    //---------- Player --------------
    if(bulk_write(f, &game.player.currentRoom->ID, sizeof(int)) < 0) ERR("write");
    if(bulk_write(f, &game.player.noOfItems, sizeof(int)) < 0) ERR("write");
    for(int i = 0; i < game.player.noOfItems; i++)
        if(bulk_write(f, &game.player.backpack[i], sizeof(int)) < 0) ERR("write");
    //------------ Items ------------
    if(bulk_write(f, &game.noOfItems, sizeof(int)) < 0) ERR("write");
    for(int i = 0; i < game.noOfItems; i++){
        if(bulk_write(f, &game.items[i].ID, sizeof(int)) < 0) ERR("write");
        if(bulk_write(f, &game.items[i].currentRoom, sizeof(int)) < 0) ERR("write");
        if(bulk_write(f, &game.items[i].destinedRoom, sizeof(int)) < 0) ERR("write");
    }
    if(TEMP_FAILURE_RETRY(close(f)) < 0) ERR("close");
}
void loadGame(Game* game, char* path){
    game->map.noOfRooms = 0;
    game->noOfItems = 0;
    int f;
    if(TEMP_FAILURE_RETRY(f = open(path, O_RDONLY)) < 0)    ERR("open");
    // ---------------- MAP -----------------
    if(bulk_read(f, &game->map.noOfRooms, sizeof(int)));
    for(int i = 0; i < MAX_ROOMS; i++){
        if(bulk_read(f, &game->map.rooms[i].ID, sizeof(int)) < 0) ERR("read");
        for(int j = 0; j < MAX_ITEMS; j++){
            if(bulk_read(f, &game->map.rooms[i].curItems[j], sizeof(int)) < 0) ERR("read");
        }
        if(bulk_read(f, &game->map.rooms[i].noOfCurItems, sizeof(int)) < 0) ERR("read");
        if(bulk_read(f, &game->map.rooms[i].noOfDestinedItems, sizeof(int)) < 0) ERR("read");
        for(int j = 0; j < MAX_DOORS; j++){
            if(bulk_read(f, &game->map.rooms[i].doors[j], sizeof(int)) < 0) ERR("read");
        }
        if(bulk_read(f, &game->map.rooms[i].noOfDoors, sizeof(int)) < 0) ERR("read");
    }
    int buf;
    //------------- Player ------------------
    int playerRoomID;
    if(bulk_read(f, &playerRoomID, sizeof(int))< 0) ERR("read");
    for(int i = 0; i < game->map.noOfRooms; i++){
        if(game->map.rooms[i].ID == playerRoomID){
            game->player.currentRoom = &game->map.rooms[i];
            break;
        }
    }
    if(bulk_read(f, &buf, sizeof(int)) < 0) ERR("read");
    game->player.noOfItems = buf;
    for(int i = 0; i < game->player.noOfItems; i++){
        if(bulk_read(f, &buf, sizeof(int)) < 0) ERR("read");
        game->player.backpack[i] =buf;
    }
    //------------- Items ----------------
    if(bulk_read(f, &buf, sizeof(int)) < 0) ERR("read");
    game->noOfItems = buf;
    game->items = (Item*) malloc(sizeof(Item) * game->noOfItems);
    for(int i = 0; i < game->noOfItems; i++){
        if(bulk_read(f, &buf, sizeof(int)) < 0) ERR("read");
        game->items[i].ID = buf;
        if(bulk_read(f, &buf, sizeof(int)) < 0) ERR("read");
        game->items[i].currentRoom = buf;
        if(bulk_read(f, &buf, sizeof(int)) < 0) ERR("read");
        game->items[i].destinedRoom = buf;
    }
    if(TEMP_FAILURE_RETRY(close(f)) < 0) ERR("close");
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
    g->state = IN_GAME;
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
void swapItems(Game* g, int ID1, int ID2){
    int index1, index2;
    for(int i = 0; i < g->noOfItems; i++){
        if(g->items[i].ID == ID1)
            index1 = i;
        if(g->items[i].ID == ID2)
            index2 = i;
    }
    int tmp = g->items[index1].currentRoom;
    g->items[index1].currentRoom = g->items[index2].currentRoom;
    g->items[index2].currentRoom = tmp;
    int roomIndex1, itemIndex1;
    int roomIndex2, itemIndex2;
    for(int i = 0; i <g->map.noOfRooms; i++){
        for(int j = 0; j < g->map.rooms[i].noOfCurItems; j++){
            if(g->map.rooms[i].curItems[j] == ID1){
                roomIndex1 = i;
                itemIndex1 = j;
            }
            else if(g->map.rooms[i].curItems[j] == ID2){
                roomIndex2 = i;
                itemIndex2 = j;
            }
        }
    }
    tmp = g->map.rooms[roomIndex1].curItems[itemIndex1];
    g->map.rooms[roomIndex1].curItems[itemIndex1] = g->map.rooms[roomIndex2].curItems[itemIndex2];
    g->map.rooms[roomIndex2].curItems[itemIndex2] = tmp;
    printf("Swapping item %d with %d\n", ID1, ID2);
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
