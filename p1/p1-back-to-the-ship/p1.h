// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include <iostream>
#include <string>
#include <getopt.h>
#include <vector>
#include <deque>
#include <sstream>
using namespace std;

struct Tile{
    uint32_t level;
    uint32_t row;
    uint32_t col;
    char type = '.';
    bool discovered = false;
    Tile *prev;
    char dir;
    int elevator = -1;
};

struct Map{
    char inputMode;
    char routeMode;
    char outputMode;
    uint32_t levelSize;
    uint32_t spaceSize;
    Tile *startTile;
    Tile *endTile;
};

typedef vector<vector<vector<Tile>>> Space;
void get_options(int argc, char** argv, Map &map);
void setTile(Space &space, Map &map); 
void writeMap(Space &space, Map &map); 
void searchNext(Space &space, Map &map, deque<Tile*> &search);
void checkType(char type);
void checkNums(uint32_t level, uint32_t row, uint32_t col, Map &map);
void trace(Map &map, deque<Tile*> &path);
void printL(deque<Tile*> &path);
void printM(Map &map, Space &space);
bool checkNext(Tile *nextTile);
void addTileToSearch(Tile *nextTile, Tile *prevTile, deque<Tile*> &search, Map &map);

