//Luca Conti
//11856871
//Lab 7

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <cmath>

using namespace std;


struct memoryTable {//Structure contains lists of all values needed to be printed in final memory table
    list <int> mainMemAdd;
    list <int> mmBlkNum;
    list <int> cmSetNum;
    list <string> cmBlkNum;
    list <bool> hitMiss;
};
/*Name: isNum
Parameters: string input - represents input given by user after prompted
Return value: returns bool if string is integer
Purpose: checks if given string can be converted to an integer number
*/
bool isNum(string input){
    for(int i = 0; i < input.size(); i++){
        if(!isdigit(input[i])){
            cout << "Input must be an integer\n";
            return false;
        }
    }
    return true;
}

/*Name: validInput
Parameters: string input - represents input given by user after prompted
Return value: returns bool if string is integer and power of two
Purpose: ensures user inputs are valid inputs for the purpose of the program
*/
bool validInput(string input){
    if(!isNum(input)){
        return false;
    }
    if(floor(log2(stoi(input))) == log2(stoi(input))){
        return true;
    }
    cout << "Input must be a power of 2\n";
    return false;
}
/*Name: getInput
Parameters: string prompt - represents desired prompt to ask user for input
Return Value - integer value of user input
Purpose: prompts user for input and makes sure input is integer and power of 2 before returning it as an int
*/
int getInput(string prompt){
    string input;
    cout << prompt;
    cin >> input;
    while(!validInput(input)){
        cout << prompt;
        cin >> input;
    }
    return stoi(input);
}

/*Name: getTag
Parameters: memAdd - the memory address to find the tag of
            mmSize - size of mm to know max value of tag
            tagSize - size that the returned tag needs to be
Return Value: string representing the binary tag of the given mmAddress
Purpose: converts a mmAddress to a tag of given size with a given max value
*/
string getTag(int memAdd, int mmSize, int tagSize){
    string tag;
    int num = memAdd;
    for (int i = 1; i <= tagSize; i++){
        if(num > mmSize/exp2(i)) {
            tag.push_back('1');
            num -= mmSize/exp2(i);
        }
        else {
            tag.push_back('0');
        }
    }
    return tag;
}

/*Name: initCache
Parameters: 2-d string array representing the cache memeory
            int degree - represents the degree of associativity for the cache
            int tagSize - size of tag to know how many X's to put in each tag spot
            int blocks - total number of blocks in the cache
Return Val: nothing it just changes the given cache array in the function
Purpose: initalize all values in cache array including valid and dirty bit, tag and data
*/
void initCache(string cache[][6], int degree, int tagSize, int blocks){
    string baseTag;
    for (int i = 0; i < tagSize; i++){
        baseTag.push_back('X');
    }
    for (int i = 0; i < blocks; i++){
        cache[i][0] = to_string(i/degree);
        cache[i][1] = to_string(i);
        cache[i][2] = "0";
        cache[i][3] = "0";
        cache[i][4] = baseTag;
        cache[i][5] = "?";
    }
    return;
}
/*Name: Age LRU
Parameters: int LRU[]: array representing the "age" of each block in cm
            int recentBlock: int representing the index of the block most recently used
            int numCMBlocks: total number of blocks in cache, also size of LRU array
Return val: nothing, just updates LRU[] in function
Purpose: add 1 value of "age" to every block in the LRU array except recent block which gets set to 0
*/
void ageLRU(int LRU[], int recentBlock, int numCMBlocks){
    for(int i = 0; i < numCMBlocks; i++){
        if(i == recentBlock){
            LRU[i] = 0;
        }
        else{
            LRU[i] += 1;
        }
    }
}
/*name: setCacheMemory
Params: int cmBlock-block in cache to be updates
        int mmBlock - value of mmBlock to be stored
        string tag - tag value to be stored
        string readWrite - tells if value is to be read or written to cm
        string cache[][] - array representing the cache memory
Return value: nothing, updates cache array with new val
Purpose: stores new mmBlock into the cache memory
*/
void setCacheMemory(int cmBlock, int mmBlock, string tag, string readWrite,string cache[][6]){
    cache[cmBlock][2] = "1";
    if(readWrite == "W") {
        cache[cmBlock][3] = "1";
    }
    else {
        cache[cmBlock][3] = "0";
    }
    cache[cmBlock][4] = tag;
    cache[cmBlock][5] = to_string(mmBlock);
}

int main(){
    string cont = "y"; //String value representing if user wants to continue
    while(cont == "y"){
        int mmSize, cmSize, blockSize, degAssoc, numCMBlocks, numCMSets; //integer values for user inputs and values to be calculated
        string replacementPolicy, degAssocString, fileName;//string values for user inputs
        ifstream inputFS;//Filestream variable for when the file is opened
        mmSize = getInput("Enter the size of main memory in bytes: ");//Runs getInput with this prompt and saves user input into variable
        cmSize = getInput("Enter the size of the cache in bytes: ");
        blockSize = getInput("Enter the cache block/line size: ");
        cout << "Enter the degree of set-associativity (input n for an n-way set-associative mapping): ";
        cin >> degAssocString;
        while(degAssocString != "n" && !isNum(degAssocString)){//Waits until user input is integer or "n"
            cout << "Enter the degree of set-associativity (input n for an n-way set-associative mapping): ";
            cin >> degAssocString;
        }
        cout << "Enter the replacement policy (L = LRU, F = FIFO): ";
        cin >> replacementPolicy;
        while(replacementPolicy != "L" && replacementPolicy != "F"){//Waits until user input is "L" or "F"
            cout << "Enter the replacement policy (L = LRU, F = FIFO): ";
            cin >> replacementPolicy;
        }
        cout << "Enter the name of the input file containing the list of memory references generated by the CPU: ";
        cin >> fileName;
        inputFS.open(fileName);
        while (!inputFS.is_open()) {//Waits until user file is valid
            cout << "Could not open file " << fileName << endl;
            cout << "Enter the name of the input file containing the list of memory references generated by the CPU: ";
            cin >> fileName;
            inputFS.open(fileName);
        }
        numCMBlocks = cmSize/blockSize;//Finds how many blocks in cache
        int addressSize, offsetSize, indexSize, tagSize, totalCacheSize;
        addressSize = log2(mmSize);//Finds address size
        offsetSize = log2(blockSize);//Finds offset size
        if (degAssocString == "n"){//Converts the degree of associativity to an integer to be used
            numCMSets = 1;
            degAssoc = numCMBlocks;//If user chose "n", the degAssoc is the same as the number of blocks
        }
        else {
            degAssoc = stoi(degAssocString);
            numCMSets = numCMBlocks/degAssoc;
        }
        indexSize = log2(numCMSets);//Finds the length of index
        tagSize = addressSize - offsetSize - indexSize;//Finds length of tag
        totalCacheSize = cmSize + numCMBlocks*(2 + tagSize)/8;//Calculates total size needed for cache
        cout << "Simulator Output:\n\n";
        cout << "Total address lines required = " << addressSize << endl;
        cout << "Number of bits for offset = " << offsetSize << endl;
        cout << "Number of bits for index = " << indexSize << endl;
        cout << "Number of bits for tag = " << tagSize << endl;
        cout << "Total cache size required = " << totalCacheSize << endl << endl;
        int numMemRefs;//Number of memory references in file
        //string trash;//Trach variable for <cr> line in file
        inputFS >> numMemRefs;//Gets the number of memory references
        //inputFS >> trash;
        string readWrite[numMemRefs];//array to store either "R" or "W" from file
        int memRef[numMemRefs];//Array to story the mm address to be referenced.
        string line; //String to represent each 
        for(int i = 0; i < numMemRefs; i++) {//Runs through file to grab each memory reference and read/write
            inputFS >> line;
            readWrite[i] = line;
            inputFS >> line;
            memRef[i] = stoi(line);
        }
        memoryTable tableOutput;//Creates a table using the memory table struct
        string cacheMemory[numCMBlocks][6];//Creates a 2-d string array to represent the cache memory
        initCache(cacheMemory, degAssoc, tagSize, numCMBlocks);//Calls the initCache function
        int bestHitSize = ceil((double)mmSize/blockSize);//Finds the max size of the best hit array
        int bestHitArray[bestHitSize] = {};//Initializes best hit array as all 0
        int LRU[numCMBlocks] = {};//Initializies least recently used array as all zeros
        int FIFO[numCMSets] = {}; //Initializes FIFO array as zeros
        for(int i = 0; i < numMemRefs; i++){
            bool hit = false;//bool to represent if the action was a hit
            bool stored = false;//bool to represent if the block got stored before replacement
            tableOutput.mainMemAdd.push_back(memRef[i]);//Adds the memory address to the table
            int memBlock = memRef[i]/blockSize;
            tableOutput.mmBlkNum.push_back(memBlock);//Adds the memory block to the table
            bestHitArray[memBlock] += 1;
            int cacheSet = memBlock % numCMSets;
            tableOutput.cmSetNum.push_back(cacheSet); //Adds the cache memory set to the table
            int cacheBlockStart = cacheSet * degAssoc; //Finds first block in set
            int cacheBlockEnd = cacheSet * degAssoc + (degAssoc - 1);//Finds last block in set
            string cacheBlockRange;
            if(cacheBlockStart != cacheBlockEnd){//If the set has a range of blocks it stores it as x to y
                cacheBlockRange.append(to_string(cacheBlockStart));
                cacheBlockRange.append(" to ");
                cacheBlockRange.append(to_string(cacheBlockEnd));
            }
            else{//If the set contains one block it stores it as x
                cacheBlockRange = to_string(cacheBlockStart);
            }
            tableOutput.cmBlkNum.push_back(cacheBlockRange);//Adds block range to table
            string tag = getTag(memRef[i], mmSize, tagSize);//Gets tag for address
            for(int block = 0; block < degAssoc; block++){//Loops through each block in the chosen set
                if (cacheMemory[cacheSet*degAssoc + block][2] == "0"){//Checks if the block is valid, if not we can store there
                    setCacheMemory(cacheSet*degAssoc + block, memBlock, tag, readWrite[i], cacheMemory);//Stores mmblock in cache
                    ageLRU(LRU, cacheSet*degAssoc + block, numCMBlocks);//Ages each block except the one stored by 1
                    tableOutput.hitMiss.push_back(false);//Declares it as a miss
                    stored = true;//Changes bool to show the block was stored
                    break;
                }
                //Checks if the tags match and that the stored blocks are the same
                else if(cacheMemory[cacheSet*degAssoc + block][4] == tag && stoi(cacheMemory[cacheSet*degAssoc + block][5]) == memBlock){
                    ageLRU(LRU, cacheSet*degAssoc + block, numCMBlocks);//Ages everything but the block accsessed 
                    tableOutput.hitMiss.push_back(true);//Declares a hit
                    stored = true;
                    hit = true;//Changes hit bool to true
                    break;
                }
            }
            if(!stored) {//Checks if we need to use a replacement policy
                if(replacementPolicy == "F"){//Checks if we need to use FIFO
                    setCacheMemory(cacheSet*degAssoc + FIFO[cacheSet], memBlock, tag, readWrite[i], cacheMemory);
                    stored = true;//Memory block was stored
                    tableOutput.hitMiss.push_back(false);//Records it was a miss
                    FIFO[cacheSet] += 1;//Increases the FIFO value at the set so we know the index of the next value that was First in
                    if(FIFO[cacheSet] >= degAssoc) {//If the value is equal to the degree of associativity it needs to be reset
                        FIFO[cacheSet] = 0;//Resets FIFO value at the specific set
                    }
                }
                else{//Otherwise we use LRU
                    int max = cacheSet*degAssoc;//Sets max to first cache block
                    for(int j = 1; j < degAssoc; j++){//Loops to find the oldest cache in the set
                        if(LRU[cacheSet*degAssoc + j] > LRU[max]){
                            max = cacheSet*degAssoc + j;
                        }
                    }
                    setCacheMemory(max, memBlock, tag, readWrite[i], cacheMemory);//Puts new value into the oldest block
                    ageLRU(LRU, max, numCMBlocks);//Ages all blocks except for the one just used
                    tableOutput.hitMiss.push_back(false);//Declares a miss
                }
            }
        }
        int totalHits = 0;//int to find actual hits
        int bestHit = 0;//int to find best possible hits
        for(int i = 0; i < bestHitSize; i++){//Loops through bestHitArray counting up each duplicate stored
            if(bestHitArray[i] > 0){
                bestHit += bestHitArray[i] - 1;
            }
        }
        cout << "main memory address\tmm blk #\tcm set #\t cm blk #\thit/miss\n";//Prints table header
        for(int i =  0; i < numMemRefs; i++){//Loops through table printing and formatting each value
            printf("%19i\t", tableOutput.mainMemAdd.front());
            tableOutput.mainMemAdd.pop_front();
            printf("%8i\t", tableOutput.mmBlkNum.front());
            tableOutput.mmBlkNum.pop_front();
            printf("%8i\t", tableOutput.cmSetNum.front());
            tableOutput.cmSetNum.pop_front();
            printf("%9s\t", tableOutput.cmBlkNum.front().c_str());
            tableOutput.cmBlkNum.pop_front();
            if(tableOutput.hitMiss.front()){//Prints hit if hit bool was true otherwise prints false
                printf("  hit");
                totalHits ++;
            }
            else {
                printf("  miss");
            }
            tableOutput.hitMiss.pop_front();
            cout << endl;
        }
        double bestHitRate = (double)bestHit/numMemRefs * 100;//Calculates best hit rate
        double actualHitRate = (double)totalHits/numMemRefs * 100;//Calculates actual hit rate
        cout << "\nBest hit rate = " << bestHit << "/" << numMemRefs << " = ";//Displays the hit rates
        printf("%.2lf", bestHitRate); //Formats it to only have 2 decimals
        cout << "%" << endl;
        cout << "Actual hit rate = " << totalHits << "/" << numMemRefs << " = ";
        printf("%.2lf", actualHitRate); 
        cout << "%" << endl << endl;
        cout << "Cache blk #\tdirty bit\tvalid bit\t tag\t      Data" << endl;//Prints header for cache
        for(int i = 0; i < numCMBlocks; i++){//Prints and formats each value in cache
            printf("%11s\t", cacheMemory[i][1].c_str());
            printf("%9s\t", cacheMemory[i][2].c_str());
            printf("%9s", cacheMemory[i][3].c_str());
            printf("%11s", cacheMemory[i][4].c_str());
            if(cacheMemory[i][5] != "?"){//Adds mm blk # before each data value that isnt "?"
                cacheMemory[i][5].insert(0, "mm blk #");
            }
            printf("%14s\t", cacheMemory[i][5].c_str());
            cout << endl;
        }
        cout << "Continue? (y = yes, n = no):";//Prompts user to start over
        cin >> cont;
        while(cont != "y" && cont != "n"){//Ensures input is "y" or "n"
            cout << "Invalid Input.\nContinue? (y = yes, n = no):";
            cin >> cont;
        }
    }
}
