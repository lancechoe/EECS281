// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "TableEntry.h"
#include <getopt.h>
#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>
#include "sillyql.h"

using namespace std;

int main(int argc, char** argv) {
    ios_base::sync_with_stdio(false); // you should already have this
    cin >> std::boolalpha;  // add these two lines
    cout << std::boolalpha; // add these two lines

    Database data;
    data.getOpt(argc, argv);
    string input = "";
    bool askPrinted = false;

    while(true){
        cout << "% ";
        cin >> input;
        askPrinted = false;

        try{
            if (input[0] == 'C'){
                data.createData();
            }
            else if(input[0] == 'R'){
                data.removeData();
            }
            else if(input[0] == 'I'){
                data.insert();
            }
            else if(input[0] == 'D'){
                data.deleteRow();
            }
            else if(input[0] == 'G'){
                data.generateIndex();
            }
            else if(input[0] == 'J'){
                data.join();
            }
            else if(input[0] == 'P'){
                data.print();
            }
            else if(input[0] == 'Q'){
                throw 0;
            }
            else if(input[0] == '#'){
                throw string{ "comment" };
            }
            else{
                throw string{ "4" };
            }
        }
        catch (string e){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

			if (e[0] == '1'){
				cout << "Error during " << input << ": Cannot create already existing table " <<
				e.substr(1, e.size() - 1) << "\n";
            }
			else if (e[0] == '2'){
				cout << "Error during " << input << ": " << e.substr(1, e.size() - 1) << 
				" does not name a table in the database\n";
            }
			else if (e[0] == '3') {
				size_t space = e.find(" ");
				if (!askPrinted) {
					cout << "Error during " << input << ": " << e.substr(1, space) <<
						"does not name a column in " << e.substr(space + 1, e.size() - space-1) << "\n";
					askPrinted = true;
				}
			}
			else if (e[0] == '4'){
				cout << "Error: unrecognized command\n";
            }
        }
        catch(int){
            cout << "Thanks for being silly!\n";
            return 0;
        }
    }
}  // main()