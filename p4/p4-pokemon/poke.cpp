// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
#include <iostream>
#include <getopt.h>
#include <iomanip>
#include <vector>
#include <limits>
#include <cmath>

using namespace std;

const double MAX = numeric_limits<double>::infinity();

struct Area{
    int x;
    int y;
    Area(int x, int y) : x(x), y(y) {}
    int areaType; // sea(Q3) = 3, land(Q1,2,4) = 2, coastline(axes) = 1
};

// Global
size_t numArea;
vector<Area> dataArea; // MST
// MST
vector<bool> k_v;
vector<double> d_v;
vector<int> p_v;
// FASTTSP
vector<size_t> path;
double lengthTSP = 0;
// OPTTSP
vector<vector<double>> edges;
vector<size_t> pathTSP;
vector<size_t> bestPath;
vector<bool> visited;
double currDist = 0;
double bestDist = 0;

int get_options(int argc, char **argv){
    int option_index = 0, option = 0;
    int numMode = 0;
    opterr = false;

    struct option longOpts[] = {
        {"mode", required_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, '\0'}
    };

    while ((option = getopt_long(argc, argv, "m:h", longOpts, &option_index)) != -1){
        switch (option){
        case 'm':
            if(optarg[0] == 'M'){ // MST
                numMode = 1;
            }
            else if(optarg[0] == 'F'){ // FASTTSP
                numMode = 2;
            }
            else if(optarg[0] == 'O'){ // OPTTSP
                numMode = 3;
            }
            else{
                cerr << "Error : Choose MST / FASTTSP / OPTTSP\n";
                exit(1);
            }
            break;

        case 'h':
            cout << "Your program, poke, should take the following case-sensitive command line options:\n";
            cout << "-m, –mode {MST|FASTTSP|OPTTSP}\n";
            cout << "MST: Find the minimum spanning tree (MST) of the map\n";
            cout << "FASTTSP: Find a fast, but not necessarily optimal, solution to the TSP\n";
            cout << "OPTTSP: Find the optimal solution to the TSP\n";
            exit(0);

        default:
            cerr << "Unknown command line option" << endl;
            exit(1);
        }
    }
    return numMode;
}

// Read the input and save the data of the Areas
void readMap(int selectMode){
    cin >> numArea;
    dataArea.reserve(numArea);
    bool isSea = false;
    bool isCoast = false;
    bool isLand = false;
    
    if(selectMode == 1){
        k_v.resize(numArea, false);
        d_v.resize(numArea, MAX);
        p_v.resize(numArea);
    }

    for(size_t i = 0; i < numArea; i++){
        int x;
        int y;
        cin >> x >> y;

        dataArea.emplace_back(x, y);

        if(selectMode == 1){
            if(dataArea[i].x < 0 && dataArea[i].y < 0){
                dataArea[i].areaType = 3;
                isSea = true;
            }
            else if((dataArea[i].x == 0 && dataArea[i].y <= 0) || (dataArea[i].y == 0 && dataArea[i].x <= 0)){
                dataArea[i].areaType = 1;
                isCoast = true;
            }
            else{
                dataArea[i].areaType = 2;
                isLand = true;
            }
        }
    }

    if(isSea && isLand && !isCoast){
        cerr << "Cannot construct MST\n";
        exit(1);
    }
}

// Calculate the distance (without sqrt)
double squareDist(const Area& a, const Area& b, int mode) {
    if(mode == 1){
    if((a.areaType == 3 && b.areaType == 2) || (a.areaType == 2 && b.areaType == 3)){
        return MAX;
    }
	else{
        return 
        ((double)(a.x - b.x) * (double)(a.x - b.x)) + 
        ((double)(a.y - b.y) * (double)(a.y - b.y));
    }
    }
    else{
        return 
        ((double)(a.x - b.x) * (double)(a.x - b.x)) + 
        ((double)(a.y - b.y) * (double)(a.y - b.y));
    }
}

// MST
void findMST(){
    
    d_v[0] = 0;
    k_v[0] = true;
    p_v[0] = -1;
    double min = MAX;
    size_t minI;
    size_t curr = 0;
    double totalDist = 0;

    for(size_t j = 0; j < numArea; j++){
        for(size_t i = 0; i < numArea; i++){
            if(!k_v[i]){
                if(squareDist(dataArea[curr], dataArea[i], 1) < d_v[i]){
                    d_v[i] = squareDist(dataArea[curr], dataArea[i], 1);
                    p_v[i] = (int)curr;
                }

                if(d_v[i] < min){
                    min = d_v[i];
                    minI = i;
                }
            }
        }

        k_v[minI] = true;
        totalDist += sqrt(d_v[minI]);        
        curr = minI;
        min = MAX;
        minI = 0;
    }

    // Print
    cout << totalDist << "\n";
    for(size_t i = 0; i < numArea; i++){
        if((int)i < p_v[i] && p_v[i] >= 0){
            cout << i << " " << p_v[i] << "\n";
        }
        else if((int)i > p_v[i] && p_v[i] >= 0){
            cout << p_v[i] << " " << i << "\n";
        }
    }
}

// FASTTSP
void findFASTTSP(int selectMode){
    path.reserve(numArea);
    
    // Making small TSP to start
    lengthTSP += sqrt(squareDist(dataArea[0], dataArea[2], 2));
    for(size_t i = 0; i < 3; i++){
        path.push_back(i);
        if(i != 0){
            lengthTSP += sqrt(squareDist(dataArea[i], dataArea[i-1], 2));
        }
    }

    for(size_t i = 3; i < numArea; i++){
        double min = MAX;
        size_t minI = 0;

        for(size_t j = 0; j < path.size(); j++){
            size_t nextj = j + 1;
            if(j == path.size() - 1){
                nextj = 0;
            }

            double minLength = lengthTSP;
            minLength += sqrt(squareDist(dataArea[path[j]], dataArea[i], 2)) + sqrt(squareDist(dataArea[path[nextj]], dataArea[i], 2));
            minLength -= sqrt(squareDist(dataArea[path[j]], dataArea[path[nextj]], 2));

            if(minLength < min){
                min = minLength;
                minI = j;
            }
        }

        lengthTSP += sqrt(squareDist(dataArea[path[minI]], dataArea[i], 2)) + 
        sqrt(squareDist(dataArea[path[(minI + 1) % path.size()]], dataArea[i], 2));
        lengthTSP -= sqrt(squareDist(dataArea[path[minI]], dataArea[path[(minI + 1) % path.size()]], 2));

        path.insert(path.begin() + (int)minI + 1, i);
    }

    if(selectMode == 2){
    // Print
    cout << lengthTSP << "\n";
    for(size_t i = 0; i < path.size(); i++){
        cout << path[i] << " ";
    }
    }
}

// OPTTSP
double lowerBound(vector<size_t> &path1, size_t permLength){
    double totalD = 0;
    vector<bool> isVisited(numArea, false);
    vector<double> thisDist(numArea, MAX);

    double min = MAX;
    size_t startingArea = (size_t)-1;
    for(size_t i = 0; i < numArea; i++){
        if(!visited[i]){
            if(edges[path1[permLength - 1]][i] < min){
                min = edges[path1[permLength - 1]][i];
                startingArea = i;
            }
        }
    }
    totalD += min;

    thisDist[startingArea] = 0;

    for(size_t i = 0; i < numArea; i++){
        if(!visited[i]){
            size_t idx = 0;
            min = MAX;
            for(size_t j = 0; j < numArea; j++){
                if(!visited[j]){
                    if(!isVisited[j] && thisDist[j] < min){
                        min = thisDist[j];
                        idx = j;
                    }
                }
            }

            isVisited[idx] = true;

            for(size_t j = 0; j < numArea; j++){
                if(!visited[j]){
                    if(!isVisited[j]){
                        double temp = edges[idx][j];
                        if(temp < thisDist[j]){
                            thisDist[j] = temp;
                        }
                    }
                }
            }
        }
    }

    for(size_t i = 0; i < numArea; i++){
        if(!visited[i]){
            totalD += thisDist[i];
        }
    }

    min = MAX;
    for(size_t i = 0; i < numArea; i++){
        if(!visited[i]){
            if(i != startingArea && edges[0][i] < min){
                min = edges[0][i];
            }
        }
    }
    totalD += min;

    return totalD;
}

bool promising(size_t permLength){
    return ((currDist + lowerBound(pathTSP, permLength)) < bestDist);
}

void genPerms(vector<size_t> &path2, size_t permLength) {
    if (permLength == path2.size()) {
        currDist += edges[0][path2.back()];
        if(currDist < bestDist){
            bestDist = currDist;
            bestPath = path2;
        }
        currDist -= edges[0][path2.back()];
        return;
    }  // if ..complete path

    if(path.size() - permLength >= 5){
        if (!promising(permLength)) {
            return;
        }  // if ..not promising
    }

    for (size_t i = permLength; i < path2.size(); ++i) {
        swap(path2[permLength], path2[i]);
        visited[path2[permLength]] = true;
        currDist += edges[path2[permLength -1]][path2[permLength]];
        genPerms(path2, permLength + 1);
        currDist -= edges[path2[permLength -1]][path2[permLength]];
        visited[path2[permLength]] = false;
        swap(path2[permLength], path2[i]);
    }  // for ..unpermuted elements
}  // genPerms()


void findOPTTSP(){
    findFASTTSP(3);

    pathTSP.reserve(numArea);
    bestPath.reserve(numArea);
    visited.resize(numArea, false);
    pathTSP = path;
    bestPath = path;
    visited[0] = true;
    bestDist = lengthTSP;
    edges.resize(numArea);
    for(size_t i = 0; i < numArea; i++){
        edges[i].resize(numArea);
    }

    for(size_t i = 0; i < numArea; i++){
        for(size_t j = 1; j < numArea; j++){
            if(i == j){
                edges[j][i] = 0;
            }
            else{
                edges[i][j] = sqrt(squareDist(dataArea[i], dataArea[j], 3));
                edges[j][i] = edges[i][j];
            }
        }
    }

    genPerms(pathTSP, 1);

    cout << bestDist << "\n";
    for(size_t i = 0; i < path.size(); i++){
        cout << bestPath[i] << " ";
    }
}

int main(int argc, char** argv){
    cout << std::setprecision(2); //Always show 2 decimal places
    cout << std::fixed; //Disable scientific notation for large numbers

    int selectMode = 0;
    selectMode = get_options(argc, argv);

    if(selectMode == 1){
        readMap(selectMode);
        findMST();
    }
    else if(selectMode == 2){
        readMap(selectMode);
        findFASTTSP(selectMode);
    }
    else if(selectMode == 3){
        readMap(selectMode);
        findOPTTSP();
    }

    return 0;
}