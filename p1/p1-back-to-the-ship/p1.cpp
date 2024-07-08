// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include "p1.h"

int main(int argc, char** argv){
    std::ios_base::sync_with_stdio(false);
    
    Map map;
    Space space;
    deque<Tile*> search;
    deque<Tile*> path;

    get_options(argc, argv, map);
    writeMap(space, map);
    searchNext(space, map, search);
    trace(map, path);

    if(map.outputMode == 'M'){
        printM(map, space);
    }
    else{
        printL(path);
    }
}

void get_options(int argc, char** argv, Map &map){
    int option_index = 0, option = 0;

    struct option longOpts[] = {
        {"stack", no_argument, nullptr, 's'},
        {"queue", no_argument, nullptr, 'q'},
        {"output", required_argument, nullptr, 'o'},
        {"help", no_argument, nullptr, 'h'},
        { nullptr, 0, nullptr, '\0' }
    };

    map.outputMode = 'M'; // default
    map.routeMode = '\0';

    while((option = getopt_long(argc, argv, "sqo:h", longOpts, &option_index)) != -1){
        switch(option){
            case 's':
                if(map.routeMode != 0){
                    cerr << "Choose one routing scheme\n";
                    exit(1);
                }
                else{
                    map.routeMode = 's';
                    break;
                }

            case 'q':
                if(map.routeMode != 0){
                    cerr << "Choose one routing scheme\n";
                    exit(1);
                }
                else{
                    map.routeMode = 'q';
                    break;
                }

            case 'o':
                if(optarg[0] == 'M'){
                    map.outputMode = 'M';
                }
                else{
                    map.outputMode = 'L';
                }
                break;

            case 'h':
                cout << "Help instructions\n";
                exit(0);
            
            default:
                break;
        }
    }

    if(map.routeMode == 0){
        cerr << "Routing scheme is not specified\n";
        exit(1);
    }
}

void setTile(Space &space, Map &map){
    
    space = vector<vector<vector<Tile>>>
    (map.levelSize, vector<vector<Tile>>
    (map.spaceSize, vector<Tile>
    (map.spaceSize, Tile())));

    for(uint32_t i = 0; i < map.levelSize; i++){
        for(uint32_t j = 0; j < map.spaceSize; j++){
            for(uint32_t k = 0; k < map.spaceSize; k++){
                space[i][j][k].level = i;
                space[i][j][k].row = j;
                space[i][j][k].col = k;
            }
        }
    }
}

void writeMap(Space &space, Map &map){

    cin >> map.inputMode >> map.levelSize >> map.spaceSize;
    cin.ignore(1);
    setTile(space, map);

    string s;
    uint32_t countLine = 0;

    while(getline(cin, s)){
        if(s[0] == '/'){
            continue;
        }
        else if(s[0] == ' ' || s[0] == '\0'){
            break;
        }

        switch(map.inputMode){
            case 'M':
                for(uint32_t i = 0; i < map.spaceSize; i++){
                    uint32_t level = countLine / map.spaceSize;
                    uint32_t row = countLine % map.spaceSize;
                    uint32_t col = i;
                    char type = s[i];
                    checkType(type);

                    space[level][row][col].level = level;
                    space[level][row][col].row = row;
                    space[level][row][col].col = col;
                    space[level][row][col].type = type;

                    if(type == 'S'){
                        map.startTile = &space[level][row][col];
                        map.startTile->discovered=true;
                    }
                    else if(type == 'H'){
                        map.endTile = &space[level][row][col];
                    }
                }
                countLine++;
                break;
            
            case 'L':
                uint32_t level;
                uint32_t row;
                uint32_t col;
                char type;
                char junk;
                istringstream is (s);
                
                while(is >> junk >> level >> junk >> row >> junk >> col >> junk >> type){
                    checkNums(level, row, col, map);
                    checkType(type);
                    // space[level][row][col].level = level;
                    // space[level][row][col].row = row;
                    // space[level][row][col].col = col;
                    space[level][row][col].type = type;

                    if(type == 'S'){
                    map.startTile = &space[level][row][col];
                    map.startTile->discovered = true;
                    }
                    else if(type == 'H'){
                    map.endTile = &space[level][row][col];
                    }
                }
                break;
        }
    }
}

bool checkNext(Tile *nextTile){
    if(nextTile->type != '#' && !nextTile->discovered){
        return true;
    }
    else{
        return false;
    }
}

void addTileToSearch(Tile *nextTile, Tile *prevTile, deque<Tile*> &search, Map &map){
    if(map.routeMode == 's'){
        search.push_front(nextTile);
    }
    else{
        search.push_back(nextTile);
    }
    nextTile->discovered = true;
    nextTile->prev = prevTile;
}


void searchNext(Space &space, Map &map, deque<Tile*> &search){
    search.push_front(map.startTile);
    Tile *curTile;
    Tile *nextTile;

    while(!search.empty()){
        curTile = search.front();
        search.pop_front();

        // North
        if(static_cast<int>(curTile->row)-1 >= 0){
            nextTile = &space[curTile->level][curTile->row-1][curTile->col];
            
            if(checkNext(nextTile)){

                addTileToSearch(nextTile, curTile, search, map);
            }

            if(nextTile->type == 'H'){
                return;
            }
        }

        // East
        if(curTile->col+1 < map.spaceSize){
            nextTile = &space[curTile->level][curTile->row][curTile->col+1];
            
            if(checkNext(nextTile)){
                addTileToSearch(nextTile, curTile, search, map);
            }

            if(nextTile->type == 'H'){
                return;
            }
        }

        // South
        if(curTile->row+1 < map.spaceSize){
            nextTile = &space[curTile->level][curTile->row+1][curTile->col];
            
            if(checkNext(nextTile)){

                addTileToSearch(nextTile, curTile, search, map);
            }

            if(nextTile->type == 'H'){
                return;
            }
        }

        // West
        if(static_cast<int>(curTile->col)-1 >= 0){
            nextTile = &space[curTile->level][curTile->row][curTile->col-1];
            
            if(checkNext(nextTile)){

                addTileToSearch(nextTile, curTile, search, map);
            }

            if(nextTile->type == 'H'){
                return;
            }
        }      

        //Elevator
        if(curTile->type == 'E'){
            for(uint32_t i = 0; i < map.levelSize; i++){
                if(space[i][curTile->row][curTile->col].type == 'E' && i != curTile->level){
                    
                    nextTile = &space[i][curTile->row][curTile->col];
                    
                    if(checkNext(nextTile)){
                        addTileToSearch(nextTile, curTile, search, map);
                    }
                }
            }
        }
    }
}

bool isPath(Tile *curTile, Tile *nextTile){
    if(nextTile->type != '#' && nextTile->discovered == false){
        nextTile -> discovered = true;
        nextTile -> prev = curTile;
        return true;
    }
    else{
        return false;
    }
}

void trace(Map &map, deque<Tile*> &path){
      if(map.endTile->prev == nullptr){
          return;
     }

    Tile *curTile = map.endTile;
    Tile *prevTile;

    while(curTile != map.startTile){
        prevTile = curTile->prev;

        if(curTile->row - prevTile->row == static_cast<uint32_t>(-1)){
            prevTile->type = 'n';
        }
        else if(curTile->row - prevTile->row == static_cast<uint32_t>(1)){
            prevTile->type = 's';
        }
        else if(curTile->col - prevTile->col == static_cast<uint32_t>(-1)){
            prevTile->type = 'w';
        }
        else if(curTile->col - prevTile->col == static_cast<uint32_t>(1)){
            prevTile->type = 'e';
        }
        else{
            prevTile->elevator = static_cast<int>(curTile->level); 
        }

        if(map.outputMode == 'L'){
            path.push_front(prevTile);
        }

        curTile = prevTile;
    }
}

void printL(deque<Tile*> &path){
    ostringstream os;
    os << "//path taken\n";

    while(!path.empty()){
        Tile *trace = path.front();
        path.pop_front();
        if(trace->type == 'E' && trace->elevator != -1){
             os << "(" << trace->level << "," << trace->row << "," <<
             trace->col << "," << trace->elevator << ")\n";
         }
        else{
            os << "(" << trace->level << "," << trace->row << "," <<
            trace->col << "," << trace->type << ")\n";
        } 
    }

    cout << os.str();
}

void printM(Map &map, Space &space){
    ostringstream os;
    os << "Start in level " << map.startTile->level 
    << ", row " << map.startTile->row << ", column " 
    << map.startTile->col << "\n";

    for(uint32_t i = 0; i < map.levelSize; i++){
        os << "//level " << i << '\n';
        for(uint32_t j = 0; j < map.spaceSize; j++){
            for(uint32_t k = 0; k < map.spaceSize; k++){
                 if(space[i][j][k].type == 'E' && space[i][j][k].elevator != -1){
                     os << space[i][j][k].elevator;
                 }
                else{
                    os << space[i][j][k].type;
                }
            }
            os << '\n';
        }
    }
    cout << os.str();
}

void checkType(char type){
    if(!(type == '.' || type == '#' || type == 'H' ||
    type == 'S' || type == 'E')){
        cout << "Wrong type of character\n";
        exit(1);
    }
}

void checkNums(uint32_t level, uint32_t row, uint32_t col, Map &map){
    if(level >= map.levelSize || row >= map.spaceSize 
    || col >= map.spaceSize){
        cout << "Wrong number in cordinates\n";
        exit(1);
    }
}
