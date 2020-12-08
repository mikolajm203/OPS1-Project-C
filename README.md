# OPS1-Project-C
Project for Operating Systems 1 subject

The aim of the project is to write a game where a player has to organize items located in rooms.

The game takes place on the map which is a graph of rooms (rooms are vertices of the graphs and player can move between the rooms only when there is an edge between them). In every room there are at most two items. The player starts game in a random room and does not carry any items initially. The player can:

    -move to any adjacent room
    -pick up an item located in the current room as long as the number of carried items will not exceed 2.
    -put down a carried item as long as number of items in the room will not exceed 2.

Each item has its unique number and a room that is assigned to (that is not the room it is located in, at least not initially). The goal is to move every item to the room it is assigned to in as few actions as possible. 
