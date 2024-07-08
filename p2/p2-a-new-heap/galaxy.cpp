// Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
#include <vector>
#include <iostream>
#include <string>
#include <getopt.h>
#include <algorithm>
#include <queue>
#include "P2random.h"
using namespace std;

// output switch
bool outputV = false;
bool outputM = false;
bool outputG = false;
bool outputW = false;

// for default output
int num_battles = 0; 
// total number of battles that have happened over the course of the day.

// for M output
uint32_t cur_timeStamp = 0;
// bool is_time_battled = false; // in this timeStamp, battle occurred?
vector<vector<int>> numTroopsLost;

// for G output
struct General{
    int num_Jtroops = 0;
    int num_Stroops = 0;
    int num_trooplost = 0;
};
vector<General> generals;

struct Battalion{
    uint32_t timeStamp; // the time that this deployment order is issued
    uint32_t generalId; // the general who is issuing the deployment order
    uint32_t planetID; // the planet which the troops are being deployed to
    // string name; // Jedi or Sith
    int force = 0; // the average force-sensitivity of troops being deployed
    int numTroops; // the number of troops being deployed
    int count; // duplicates

    void get_options(int argc, char** argv){
    int option_index = 0, option = 0;

        struct option longOpts[] = {
            {"verbose", no_argument, nullptr, 'v'},
            {"median", no_argument, nullptr, 'm'},
            {"general-eval", no_argument, nullptr, 'g'},
            {"watcher", no_argument, nullptr, 'w'},
            { nullptr, 0, nullptr, '\0' }
        };

        while((option = getopt_long
        (argc, argv, "vmgw", longOpts, &option_index)) != -1){
            switch(option){
            case 'v':
                    outputV = true;
                    break;
                
            case 'm': 
                    outputM = true;
                    break;

            case 'g':
                    outputG = true;
                    break;

            case 'w':
                    outputW = true;
                    break;

            default:
                cerr << "Unknown command line option" << endl;
                exit(1);
            }
        }
    }
};

// for W output
struct Movie{
vector<Battalion> attack;
vector<Battalion> ambush;
Battalion bestJattack;
Battalion bestSattack;
Battalion tempJ;
Battalion bestJambush;
Battalion bestSambush;
Battalion tempS;
int attfd = -1;
int ambfd = -1;
};
vector<Movie> movies;

struct jediComp{ // Minheap
    bool operator()(const Battalion &a, const Battalion &b){
        return ((a.force > b.force) || 
        ((a.force) == (b.force) && (a.count) > (b.count)));
    }
};

struct sithComp{ // Maxheap
    bool operator()(const Battalion &a, const Battalion &b){
        return ((a.force < b.force) || 
        ((a.force) == (b.force) && (a.count) > (b.count)));
    }
};

struct Planet{
    priority_queue <Battalion, vector<Battalion>, jediComp> jedi;
    priority_queue <Battalion, vector<Battalion>, sithComp> sith;
};

int findMedian(vector<int> &nums) {
  sort(nums.begin(), nums.end());
  int median;
  if (nums.size() % 2 == 0) {
    median = (nums[nums.size()/2-1] + nums[nums.size()/2]) / 2;
  } else {
    median = nums[nums.size()/2];
  }

  return median; 
}

void battle(Battalion &sith, Battalion &jedi){
    int battled_troops;

    if(sith.numTroops > jedi.numTroops){
        sith.numTroops -= jedi.numTroops;
        battled_troops = jedi.numTroops;
        jedi.numTroops = 0;
    }
    else if(sith.numTroops < jedi.numTroops){
        jedi.numTroops -= sith.numTroops;
        battled_troops = sith.numTroops;
        sith.numTroops = 0;
    }
    else{
        battled_troops = jedi.numTroops;
        sith.numTroops = 0;
        jedi.numTroops = 0;
    }

    // for output V mode
    if(outputV){
        cout << "General " << sith.generalId << "'s battalion attacked General "
        << jedi.generalId << "'s battalion on planet " << sith.planetID << ". "
        << battled_troops*2 << " troops were lost.\n";
    }
    
    // for output M mode
    numTroopsLost[sith.planetID].push_back(battled_troops*2);

    // for output G mode
    generals[sith.generalId].num_trooplost += battled_troops;
    generals[jedi.generalId].num_trooplost += battled_troops;

    num_battles++;
}

void deployBattalion(Battalion &battalion){

    string s;
    int countLine = 0;
    
    string inputMode;
    uint32_t num_gen = 0;
    uint32_t num_planets = 0;
    uint32_t num_deploys = 0;
    uint32_t rate = 0;
    uint32_t seed = 0;

    // COMMENT (ignore)
    getline(cin, s);
    // MODE
    cin >> s >> inputMode;
    // numG
    cin >> s >> num_gen;
    // numP
    cin >> s >> num_planets;

    vector<Planet> planets(num_planets); // output V
    numTroopsLost.resize(num_planets); // output M
    generals.resize(num_gen); // output G
    movies.resize(num_planets); // output W

    stringstream ss;
    // inputMode is the "PR" or "DL" from line 2
    if (inputMode == "PR") {  // inputMode is read from line 2 of the header
        cin >> s >> seed;
        cin >> s >> num_deploys;
        cin >> s >> rate;

        P2random::PR_init(ss, seed, num_gen, num_planets, num_deploys, rate);
    }
    istream &inputStream = inputMode == "PR" ? ss : cin;

    char c; // trash
    cout << "Deploying troops...\n";
    string name; // Jedi or Sith

    while (inputStream >> battalion.timeStamp >> name >> c 
    >> battalion.generalId >> c >> battalion.planetID >> c >> battalion.force
    >> c >> battalion.numTroops) {

        battalion.count = countLine; // order for duplicate

        //error checking    
        // Unknown command line option
        // Invalid decreasing timestamp
        // Invalid general ID
        // Invalid planet ID
        // Invalid force sensntivity level
        // Invalid number of troops
        if(battalion.timeStamp < cur_timeStamp){
            cerr << "Invalid decreasing timestamp" << endl;
            exit(1);
        }        
        if(battalion.generalId >= num_gen){
            cerr << "Invalid general ID" << endl;
            exit(1);
        }
        if(battalion.planetID >= num_planets){
            cerr << "Invalid planet ID" << endl;
            exit(1);
        }
        if(battalion.force <= 0){
            cerr << "Invalid force sensntivity level" << endl;
            exit(1);
        }
        if(battalion.numTroops <= 0){
            cerr << "Invalid number of troops" << endl;
            exit(1);
        }
        
        // for output W, attack
        if(outputW){
        Battalion forMovie = battalion;
        uint32_t pID = forMovie.planetID;
        movies[pID].attack.push_back(forMovie);
        
        if(name == "JEDI"){
            if(movies[pID].bestJattack.force == 0){ // bestJattack not initialized yet
                movies[pID].bestJattack = forMovie;
            }
            else if(movies[pID].bestJattack.force != 0 && movies[pID].bestSattack.force == 0){ 
                // bestJ initialized, bestS not initialized
                if(movies[pID].bestJattack.force > forMovie.force){ // smaller force ?
                    movies[pID].bestJattack = forMovie; // now new smaller is the best
                }
            }
            else if(movies[pID].attfd != -1){ // if there exist attfd,
                if(movies[pID].tempJ.force == 0
                && movies[pID].bestJattack.force > forMovie.force)
                {
                    movies[pID].tempJ = forMovie;
                }
                else{
                    if(movies[pID].tempJ.force > forMovie.force){
                        movies[pID].tempJ = forMovie;
                    }
                }
            }
        }
        else if(name == "SITH"){
            if(movies[pID].bestJattack.force != 0 && movies[pID].bestSattack.force == 0
            && movies[pID].bestJattack.force <= forMovie.force){
            // if bestJ initialized and sith's force is higer than bestJedi,
                movies[pID].bestSattack = forMovie;
                movies[pID].attfd = forMovie.force - movies[pID].bestJattack.force;
            }
            else if(movies[pID].attfd != -1 && movies[pID].tempJ.force == 0
            && movies[pID].bestJattack.force <= forMovie.force){
                if(forMovie.force - movies[pID].bestJattack.force > movies[pID].attfd){
                    movies[pID].bestSattack = forMovie;
                    movies[pID].attfd = forMovie.force - movies[pID].bestJattack.force;
                }
                else if(forMovie.force - movies[pID].bestJattack.force == movies[pID].attfd){
                    if(forMovie.timeStamp < movies[pID].bestSattack.timeStamp){
                        movies[pID].bestSattack = forMovie;
                    }
                }
            }
            else if(movies[pID].attfd != -1 && movies[pID].tempJ.force != 0
            && movies[pID].tempJ.force <= forMovie.force){
                // attfd already exists & tempJ exists 
                if(forMovie.force - movies[pID].tempJ.force > movies[pID].attfd){
                    movies[pID].bestSattack = forMovie;
                    movies[pID].bestJattack = movies[pID].tempJ;
                    movies[pID].attfd = forMovie.force - movies[pID].tempJ.force;
                }
                else if(forMovie.force - movies[pID].tempJ.force == movies[pID].attfd){
                    if(forMovie.timeStamp < movies[pID].bestSattack.timeStamp){
                        movies[pID].bestSattack = forMovie;
                        movies[pID].bestJattack = movies[pID].tempJ;
                    }
                }
            }
        }
        }

        // for output W, ambush
        if(outputW){
        Battalion forMovie = battalion;
        uint32_t pID = forMovie.planetID;
        movies[pID].ambush.push_back(forMovie);
        
        if(name == "SITH"){
            if(movies[pID].bestSambush.force == 0){
                movies[pID].bestSambush = forMovie;
            }
            else if(movies[pID].bestSambush.force != 0 && movies[pID].bestJambush.force == 0){ 
                if(movies[pID].bestSambush.force < forMovie.force){ 
                    movies[pID].bestSambush = forMovie;
                }
            }
            else if(movies[pID].ambfd != -1){
                if(movies[pID].tempS.force == 0 
                && movies[pID].bestSambush.force < forMovie.force)
                {
                    movies[pID].tempS = forMovie;
                }
                else if(movies[pID].tempS.force != 0)
                {
                    if(movies[pID].tempS.force < forMovie.force){
                        movies[pID].tempS = forMovie;
                    }
                }
            }
        }
        else if(name == "JEDI"){
            if(movies[pID].bestSambush.force != 0 && movies[pID].bestJambush.force == 0
            && movies[pID].bestSambush.force >= forMovie.force){
            // if bestJ initialized and sith's force is higer than bestJedi,
                movies[pID].bestJambush = forMovie;
                movies[pID].ambfd = movies[pID].bestSambush.force - forMovie.force;
            }
            else if(movies[pID].ambfd != -1 && movies[pID].tempS.force == 0
            && movies[pID].bestSambush.force >= forMovie.force){
            // if ambfd already exist and there is no tempS, and smaller jedi came, update ambfd
                if(movies[pID].bestSambush.force - forMovie.force > movies[pID].ambfd){
                    movies[pID].bestJambush = forMovie;
                    movies[pID].ambfd = movies[pID].bestSambush.force - forMovie.force;
                }
                else if(movies[pID].bestSambush.force - forMovie.force == movies[pID].ambfd){
                    if(forMovie.timeStamp < movies[pID].bestSattack.timeStamp){
                        movies[pID].bestJambush = forMovie;
                    }
                }
            }
            else if(movies[pID].ambfd != -1 && movies[pID].tempS.force != 0
            && movies[pID].tempS.force >= forMovie.force){
                if(movies[pID].tempS.force - forMovie.force > movies[pID].ambfd){
                    movies[pID].bestJambush = forMovie;
                    movies[pID].bestSambush = movies[pID].tempS;
                    movies[pID].ambfd = movies[pID].tempS.force - forMovie.force;
                }
                else if(movies[pID].tempS.force - forMovie.force == movies[pID].ambfd){
                    if(forMovie.timeStamp < movies[pID].bestSattack.timeStamp){
                        movies[pID].bestJambush = forMovie;
                        movies[pID].bestSambush = movies[pID].tempS;
                    }
                }
            }
        }
        }

        // checking if timeStamp increased ( for output M )
        if(cur_timeStamp != battalion.timeStamp){
            if(outputM){
                for(uint32_t i = 0; i < num_planets; i++){
                    if(numTroopsLost[i].size() != 0){
                        int median = findMedian(numTroopsLost[i]);
                        cout << "Median troops lost on planet " << i << " at time "
                        << cur_timeStamp << " is " << median << ".\n";
                    }
                }    
            }
            cur_timeStamp = battalion.timeStamp;
            // is_time_battled = false;
        }

        //deploy
        if(name == "JEDI"){
            // storing General's Jedi troops ( for output G )
            for(uint32_t i = 0; i < num_gen; i++){
                if(battalion.generalId == i){
                    generals[i].num_Jtroops += battalion.numTroops;
                }
            }
            // default output
            if(planets[battalion.planetID].sith.empty()){
                planets[battalion.planetID].jedi.push(battalion);
            }
            else{
                Battalion sith_top = planets[battalion.planetID].sith.top();
                while(battalion.numTroops != 0 
                && sith_top.force >= battalion.force
                && sith_top.numTroops != 0){ // until its troop is 0
                    
                    battle(sith_top, battalion);
                    // is_time_battled = true;

                    planets[battalion.planetID].sith.pop();
                    planets[battalion.planetID].sith.push(sith_top);
                    
                    if(battalion.numTroops != 0 || sith_top.numTroops == 0){
                        planets[battalion.planetID].sith.pop();
                        sith_top = planets[battalion.planetID].sith.top();
                    }

                }
                if(battalion.numTroops != 0){ // after all the possible battle, push
                    planets[battalion.planetID].jedi.push(battalion);
                }
            }
        }
        else if(name == "SITH"){
            // storing General's Sith troops ( for output G )
            for(uint32_t i = 0; i < num_gen; i++){
                if(battalion.generalId == i){
                    generals[i].num_Stroops += battalion.numTroops;
                }
            }
            // default output
            if(planets[battalion.planetID].jedi.empty()){
                planets[battalion.planetID].sith.push(battalion);
            }
            else{
                Battalion jedi_top = planets[battalion.planetID].jedi.top();
                while(battalion.numTroops != 0 
                && jedi_top.force <= battalion.force
                && jedi_top.numTroops != 0){ // until its troop is 0
                    
                    battle(battalion, jedi_top);
                    // is_time_battled = true;
                    
                    planets[battalion.planetID].jedi.pop();
                    planets[battalion.planetID].jedi.push(jedi_top);

                    if(battalion.numTroops != 0 || jedi_top.numTroops == 0){
                        planets[battalion.planetID].jedi.pop();
                        jedi_top = planets[battalion.planetID].jedi.top();
                    }
                }
                if(battalion.numTroops != 0){ // after all the possible battle, push
                    planets[battalion.planetID].sith.push(battalion);
                }
            }
        }

        countLine++;
    }  

    if(outputM){
        for(uint32_t i = 0; i < num_planets; i++){

            if(numTroopsLost[i].size() != 0){
                        int median = findMedian(numTroopsLost[i]);
                        cout << "Median troops lost on planet " << i << " at time "
                        << cur_timeStamp << " is " << median << ".\n";
                    }
        }    
    }  
    cout << "---End of Day---\n";
    cout << "Battles: " << num_battles << endl;

    if(outputG){
        cout << "---General Evaluation---\n";
        for(uint32_t i = 0; i < num_gen; i++){
            cout << "General " << i << " deployed " <<
            generals[i].num_Jtroops << " Jedi troops and " <<
            generals[i].num_Stroops << " Sith troops, and " <<
            generals[i].num_Jtroops+generals[i].num_Stroops-generals[i].num_trooplost <<
            "/" << generals[i].num_Jtroops+generals[i].num_Stroops <<
            " troops survived.\n";
        }
    }

    if(outputW){
        cout << "---Movie Watcher---\n";
        
        for(uint32_t i = 0; i < num_planets; i++){
            if(movies[i].ambfd != -1){
            cout << "A movie watcher would enjoy an ambush on planet " << i << 
            " with Sith at time " << movies[i].bestSambush.timeStamp << 
            " and Jedi at time " << movies[i].bestJambush.timeStamp <<
            " with a force difference of " << movies[i].ambfd << ".\n";      
            }
            else{
            cout << "A movie watcher would not see an interesting ambush on planet "
            << i << ".\n";
            }
            if(movies[i].attfd != -1){
            cout << "A movie watcher would enjoy an attack on planet " << i << 
            " with Jedi at time " << movies[i].bestJattack.timeStamp << 
            " and Sith at time " << movies[i].bestSattack.timeStamp <<
            " with a force difference of " << movies[i].attfd << ".\n";   
            }
            else{
            cout << "A movie watcher would not see an interesting attack on planet "
            << i << ".\n";
            }    
        }
    }

}

int main(int argc, char** argv){
    
    Battalion battalion;
    Planet planet;    
    battalion.get_options(argc, argv);
    deployBattalion(battalion);
    
    return 0;
}