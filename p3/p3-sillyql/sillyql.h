// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "TableEntry.h"
#include <getopt.h>
#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>
#include <utility>
#include <algorithm>

using namespace std;

class Less{
private:
    size_t colNum;
    TableEntry value;

public:
    Less(size_t inColNum, TableEntry inValue) :
        colNum{inColNum}, value{inValue} {}

    bool operator()(vector<TableEntry>& row){
        return row[colNum] < value;
    }
    
    bool operator()(const TableEntry& inTable){
        return inTable < value;
    }

    bool operator()(pair<const TableEntry, int>& inPair){
        return inPair.first < value;
    }
};

class Greater{
private:
    size_t colNum;
    TableEntry value;

public:
    Greater(size_t inColNum, TableEntry inValue) :
        colNum{inColNum}, value{inValue} {}

    bool operator()(vector<TableEntry>& row){
        return row[colNum] > value;
    }
    
    bool operator()(const TableEntry& inTable){
        return inTable > value;
    }

    bool operator()(pair<const TableEntry, int>& inPair){
        return inPair.first > value;
    }
};

class Equal{
private:
    size_t colNum;
    TableEntry value;

public:
    Equal(size_t inColNum, TableEntry inValue) :
        colNum{inColNum}, value{inValue} {}

    bool operator()(vector<TableEntry>& row){
        return row[colNum] == value;
    }
    
    bool operator()(const TableEntry& inTable){
        return inTable == value;
    }

    bool operator()(pair<const TableEntry, int>& inPair){
        return inPair.first == value;
    }
};

TableEntry convert(EntryType& type, const string& value){
    if(type == EntryType::String){
        return TableEntry(value);
    }
    else if(type == EntryType::Int){
        int temp;
        temp = stoi(value);
        return TableEntry(temp);
    }
    else if(type == EntryType::Bool){
        if(value == "true"){
            return TableEntry(true);
        }
        else{
            return TableEntry(false);
        }
    }
    else{
        double temp;
        temp = stod(value);
        return TableEntry(temp);
    }
}

template<class ForwardIterator, class Compare>
int printCount(ForwardIterator first, ForwardIterator last, Compare pred, vector<pair<string, int>>& printCol, bool q){
    int printed = 0;
    while(first != last){
        if(pred(*first)){
            if(!q){
                for(auto it : printCol){
                    cout << first->at(size_t(it.second)) << " ";
                }
                cout << "\n";
            }
            printed++;
        }
        first++;
    }
    return printed;
}

class Table{

private:
    friend class Database;

    vector<vector<TableEntry>> table;
    unordered_map<string, size_t> colNames;
    vector<EntryType> colTypes;
    map<TableEntry, vector<size_t>> bst;
    unordered_map<TableEntry, vector<size_t>> hash;
    string name;
    string iName;
    size_t iNum;
    string iType;

public:
    Table() : name{ "" } {};

    Table(string t_name, int colNum) : name{ t_name }, iName{ "" }, iNum{ 0 }, 
    iType{ "" }{
        colTypes.resize(size_t(colNum));
        colNames.reserve(size_t(colNum) * 2);
    };

    size_t colCheck(const string& col){
        auto it = colNames.find(col);

        if(it != colNames.end()){
            return it->second;
        }
        else{
            throw string{ "3" + col + " " + name };
        }
    }
    
    template <class ForwardIterator>
    int printRange(ForwardIterator first, ForwardIterator last, vector<pair<string, int>>& printCol, bool q){
        int printed = 0;

        while(first != last){
            if(!q){
                for(auto i : first->second){
                    for(auto it : printCol){
                        cout << table[i][size_t(it.second)] << " ";
                    }
                    cout << "\n";
                    printed++;
                }
            }
            else{
                printed += int(first->second.size());
            }
            first++;
        }
        return printed;
    }

    void printTable(bool q){
        string colNum;
        int numCol;
        cin >> colNum;
        numCol = stoi(colNum);

        vector<pair<string, int>> colName;
        colName.reserve(size_t(numCol));
        string temp;
        for(int i = 0; i < numCol; i++){
            cin >> temp;
            colName.push_back(pair<string, int>{temp, colCheck(temp)});
        }

        string condition;
        cin >> condition;

        if(condition == "ALL"){
            if(!q){
                for(auto i : colName){
                    cout << i.first << " ";
                }
                cout << "\n";
                for(size_t row = 0; row < table.size(); row++){
                    for(auto i : colName){
                        cout << table[row][size_t(i.second)] << " ";
                    }
                    cout << "\n";
                }
            }
            cout << "Printed " << table.size() << " matching rows from " << name << "\n";
        }
        else if(condition == "WHERE"){
            string nameCol;
            string value;
            string op;
            size_t cols;
            cin >> nameCol;
            cin >> op;
            cin >> value;

            cols = colCheck(nameCol);

            if(!q){
                for(auto i : colName){
                    cout << i.first << " ";
                }
                cout << "\n";
            }
            
            int printed = 0;
            
            if(op == "<"){
                if(nameCol == iName && iType == "bst"){
                    TableEntry val = convert(colTypes[cols], value);
                    auto endIt = bst.lower_bound(val);
                    printed = printRange(bst.begin(), endIt, colName, q);
                }
                else{
                    printed = printCount(table.begin(), table.end(), Less(cols, convert(colTypes[cols], value)), colName, q);
                }
            }
            else if(op == "="){
                if(nameCol == iName && iType == "bst"){
                    TableEntry val = convert(colTypes[cols], value);
                    auto rowIt = bst.find(val);
                    if(rowIt != bst.end()){
                        for(auto k : rowIt->second){
                            if(!q){
                                for(auto colIt : colName){
                                    cout << table[k][size_t(colIt.second)] << " ";
                                }
                                cout << "\n";
                            }
                            printed++;
                        }
                    }
                }
                else if(nameCol == iName && iType == "hash"){
                    TableEntry val = convert(colTypes[cols], value);
                    auto rowIt = hash.find(val);
                    if(rowIt != hash.end()){
                        for(auto k : rowIt->second){
                            if(!q){
                                for(auto colIt : colName){
                                    cout << table[k][size_t(colIt.second)] << " ";
                                }
                                cout << "\n";
                            }
                            printed++;
                        }
                    }
                }
                else{
                    printed = printCount(table.begin(), table.end(), Equal(cols, convert(colTypes[cols], value)), colName, q);
                }
            }
            else{
                if(nameCol == iName && iType == "bst"){
                    TableEntry val = convert(colTypes[cols], value);
                    auto bIt = bst.upper_bound(val);
                    printed = printRange(bIt, bst.end(), colName, q);
                }
                else{
                    printed = printCount(table.begin(), table.end(), Greater(cols, convert(colTypes[cols], value)), colName, q);
                }
            }
            cout << "Printed " << printed << " matching rows from " << name << "\n";
        }
    }

    void insertTable(){
        string rowNum;
        int numRow;
        cin >> rowNum;
        numRow = stoi(rowNum);
        cin >> rowNum;
        size_t curRowNum = table.size();

        table.resize(curRowNum + size_t(numRow));
        for(auto i : table){
            i.reserve(colNames.size());
        }

        if(iName.empty()){
            for(size_t i = curRowNum; i < size_t(numRow) + curRowNum; i++){
                for(size_t j = 0; j < colNames.size(); j++){
                    string temp;
                    cin >> temp;
                    table[i].emplace_back(convert(colTypes[j], temp));
                }
            }
        }
        else{
            for(size_t i = curRowNum; i < size_t(numRow) + curRowNum; i++){
                for(size_t j = 0; j < colNames.size(); j++){
                    string temp;
                    cin >> temp;
                    table[i].emplace_back(convert(colTypes[j], temp));
                }
                if(iType == "bst"){
                    bst[table[i][iNum]].push_back(i);
                }
                else{
                    hash[table[i][iNum]].push_back(i);
                }
            }
        }

        cout << "Added " << numRow << " rows to " << name << " from position " << curRowNum << " to "
        << curRowNum + size_t(numRow) - 1 << "\n";
    }

};

class Database{
    bool quiet = false;

public:
    unordered_map<string, Table*> myDatabase;

    ~Database(){
        for(auto i : myDatabase){
            delete i.second;
        }
    }

    Table* tableCheck(const string& table){
        auto tableCheck = myDatabase.find(table);
        if(tableCheck == myDatabase.end()){
            throw "2" + table;
        }

        return tableCheck->second;
    }

    size_t colCheck(const string& col, Table* tablePtr){
        if(tablePtr->colNames.find(col) != tablePtr->colNames.end()){
            return tablePtr->colNames.find(col)->second;
        }
        else{
            throw string{ "3" + col + " " + tablePtr->name };
        }
    }

    void getOpt(int argc, char** argv){
        int option_index = 0, option = 0;
        opterr = false;

        struct option longOpts[] = {
            {"quiet", no_argument, nullptr, 'q'},
            {"help", no_argument, nullptr, 'h'},
            { nullptr, 0, nullptr, '\0' }
        };
    
        while((option = getopt_long
        (argc, argv, "qh", longOpts, &option_index)) != -1){
            switch(option){
            case 'q':
                    quiet = true;
                    break;
                
            case 'h': 
                    cout << "Helpful message\n";
                    exit(1);
            }
        }
    }

    // Database CREATE
    void createData(){

        string name;
        int numCol;
        string colNum;
        cin >> name;
        cin >> colNum;
        numCol = stoi(colNum);

        if(myDatabase.find(name) != myDatabase.end()){
            throw string{ "1" + name };
        }

        Table* table = new Table(name, numCol);

        string type;
        for(size_t i = 0; i < table->colTypes.size(); i++){
            cin >> type;

            if(type == "string"){
                table->colTypes[i] = EntryType::String;
            }
            else if(type == "double"){
                table->colTypes[i] = EntryType::Double;
            }
            else if(type == "bool"){
                table->colTypes[i] = EntryType::Bool;
            }
            else if(type == "int"){
                table->colTypes[i] = EntryType::Int;
            }
            else{
                assert(false);
            }
        }

        size_t numColumn = size_t(numCol);

        vector<string> printNames(numColumn);
        for(size_t i = 0; i < size_t(numCol); i++){
            cin >> name;
            table->colNames[name] = i;
            printNames[i] = name;
        }

        myDatabase[table->name] = table;
        
        cout << "New table " << table->name << " with column(s) ";
        for(auto name : printNames){
            cout << name << " ";
        }
        cout << "created\n";

    }

    // Database REMOVE
    void removeData() {
        string name;
        cin >> name;
        auto it = myDatabase.find(name);
        if(it != myDatabase.end()){
            delete it->second;
            myDatabase.erase(it);
            cout << "Table " << name << " deleted\n";
            return;
        }
        throw string{ "2" + name };
    }

    void insert(){
        string tableName;
        string temp;
        int numRow;
        cin >> tableName;
        cin >> tableName;
        Table* tablePtr = tableCheck(tableName);

        auto check = myDatabase.find(tableName);
        if(check == myDatabase.end()){
            cin >> temp;
            numRow = stoi(temp);
            for(int i = 0; i < numRow; i++){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            throw "2" + tableName;
        }

        tablePtr = check->second;
        tablePtr->insertTable();
    }

    void update(const string& colName, const string& type, const size_t& numCol, Table* tablePtr){
        if(!tablePtr -> hash.empty()){
            tablePtr -> hash.clear();
        }
        else if(!tablePtr -> bst.empty()){
            tablePtr -> bst.clear();
        }

        if(type == "bst"){
            for(size_t i = 0; i < tablePtr->table.size(); i++){
                tablePtr -> bst[tablePtr -> table[i][numCol]].push_back(i);
            }
        }
        else if(type == "hash"){
            tablePtr -> hash.reserve(tablePtr -> table.size());
            for(size_t i = 0; i < tablePtr->table.size(); i++){
                tablePtr -> hash[tablePtr -> table[i][numCol]].push_back(i);
            }
        }

        tablePtr->iName = colName;
        tablePtr->iType = type;
        tablePtr->iNum = numCol;
    }

    void deleteRow(){
        string tableName = "";
        string colName = "";
        string op = "";
        string value = "";
        size_t numCol = 0;
        size_t numRow = 0;

        cin >> tableName;
        cin >> tableName;
        cin >> colName;
        cin >> colName;
        cin >> op;
        cin >> value;

        Table* tablePtr = tableCheck(tableName);
        numRow = tablePtr->table.size();
        numCol = colCheck(colName, tablePtr);
        auto end = tablePtr->table.end();

        if(op == "<"){
            end = remove_if(tablePtr->table.begin(), tablePtr->table.end(), 
            Less(numCol, convert(tablePtr->colTypes[numCol], value)));
        }
        else if(op == "="){
            end = remove_if(tablePtr->table.begin(), tablePtr->table.end(), 
            Equal(numCol, convert(tablePtr->colTypes[numCol], value)));
        }
        else if(op == ">"){
            end = remove_if(tablePtr->table.begin(), tablePtr->table.end(), 
            Greater(numCol, convert(tablePtr->colTypes[numCol], value)));
        }
        else{
            assert(false);
        }

        tablePtr->table.erase(end, tablePtr->table.end());

        update(tablePtr->iName, tablePtr->iType, tablePtr->iNum, tablePtr);

        cout << "Deleted " << numRow - tablePtr->table.size() << " rows from " << tableName << "\n";
    }

    void generateIndex(){
        string tableName = "";
        string colName = "";
        string indexType = "";
        string value = "";
        size_t numCol = 0;
        string ignore = "";

        cin >> ignore;
        cin >> tableName;
        cin >> indexType;
        cin >> ignore;
        cin >> ignore;
        cin >> colName;

        Table* tablePtr = tableCheck(tableName);

        numCol = colCheck(colName, tablePtr);

        update(colName, indexType, numCol, tablePtr);

        cout << "Created " << indexType << " index for table " <<
        tableName << " on column " << colName << "\n";
        
    }

    void join(){
        string table1 = "";
        string table2 = "";
        string col1 = "";
        string col2 = "";
        size_t numCol1 = 0;
        size_t numCol2 = 0;
        int numCol = 0;
        string ignore = "";

        cin >> table1;

        Table* tablePtr1 = tableCheck(table1);

        cin >> ignore;
        cin >> table2;

        Table* tablePtr2 = tableCheck(table2);

        cin >> ignore;
        cin >> col1;

        numCol1 = colCheck(col1, tablePtr1);

        cin >> ignore;
        cin >> col2;

        numCol2 = colCheck(col2, tablePtr2);

        cin >> ignore;
        cin >> ignore;
        cin >> ignore;

        numCol = stoi(ignore);

        vector<pair<int, string>> colNames;
        string temp = "";
        colNames.reserve(size_t(numCol));

        for(int i = 0; i < numCol; i++){
            int numTable = 0;
            cin >> temp;
            cin >> ignore;
            numTable = stoi(ignore);

            if(numTable == 1){
                colCheck(temp, tablePtr1);
                colNames.push_back(pair<int, string>{1, temp});
            }
            else{
                colCheck(temp, tablePtr2);
                colNames.push_back(pair<int, string>{2, temp});
            }
        }
        if(!quiet){
            for(auto i : colNames){
                cout << i.second << " ";
            }
            cout << "\n";
        }

        int numPrint = 0;

        if(tablePtr2->iNum == numCol2 && tablePtr2->iName == col2 && tablePtr2 -> iType == "bst"){
            for(auto i : tablePtr1 -> table){
                auto it = tablePtr2 -> bst.find(i[numCol1]);
                if(it != tablePtr2->bst.end()){
                    for(auto j : it->second){
                        if(!quiet){
                            for(auto k : colNames){
                                if(k.first == 1){
                                    cout << i[tablePtr1->colNames[k.second]] << " ";
                                }
                                else{
                                    cout << tablePtr2->table[j][tablePtr2->colNames[k.second]] << " ";
                                }
                            }
                            cout << "\n";
                        }
                        numPrint++;
                    }
                }
            }
        }
        else if(tablePtr2 -> iNum == numCol2 && tablePtr2 -> iName == col2 && tablePtr2 -> iType == "hash"){
            for(auto i : tablePtr1 -> table){
                auto it = tablePtr2 -> hash.find(i[numCol1]);
                if(it != tablePtr2->hash.end()){
                    for(auto j : it->second){
                        if(!quiet){
                            for(auto k : colNames){
                                if(k.first == 1){
                                    cout << i[tablePtr1->colNames[k.second]] << " ";
                                }
                                else{
                                    cout << tablePtr2->table[j][tablePtr2->colNames[k.second]] << " ";
                                }
                            }
                            cout << "\n";
                        }
                        numPrint++;
                    }
                }
            }
        }
        else{
            unordered_map<TableEntry, vector<size_t>> hash(tablePtr2 -> table.size()*2);
            for(size_t i = 0; i < tablePtr2->table.size(); i++){
                hash[tablePtr2->table[i][numCol2]].push_back(i);
            }
            for(auto i : tablePtr1 -> table){
                auto it = hash.find(i[numCol1]);
                if(it != hash.end()){
                    for(auto j : it -> second){
                        if(!quiet){
                            for(auto k : colNames){
                                if(k.first == 1){
                                    cout << i[tablePtr1->colNames[k.second]] << " ";
                                }
                                else{
                                    cout << tablePtr2->table[j][tablePtr2->colNames[k.second]] << " ";
                                }
                            }
                            cout << "\n";
                        }
                        numPrint++;
                    }
                }
            }
        }

        cout << "Printed " << numPrint << " rows from joining " << table1 << " to " << table2 << "\n";
    }

    void print(){
        string tableName;
        string ignore;

        cin >> ignore;
        cin >> tableName;

        Table* tablePtr = tableCheck(tableName);
        tablePtr->printTable(quiet);
    }

};
