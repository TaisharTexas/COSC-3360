#include <stdio.h> 
#include <unistd.h> 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
using namespace std;

class Process {
public:
    vector<string> myActions;

    // Constructor with parameters
    Process(int deadline, int computationTime, int numResources, vector<string>& myActions, vector<map<string, bool> > &resourceDict, sem_t *semaphore, int id) {
        this->deadline = deadline;
        this->computationTime = computationTime;
        this->numResources = numResources;
        this->myActions = myActions;
        this->resourceDict = resourceDict;
        this->id = id;
        this->semaphore = semaphore;
        masterString = "\n";
    }


    void executeAction(string& inputAction){
        // int i = 0;
        // while(i < myActions.size()){

            if(inputAction.substr(0,7) == "release"){
                //release resources action 
                // (communicate which resources are released through pipe to the parent function so it can update what resources are currently available)
                masterString += "releaseResources Action\n";
            }
            else if(inputAction.substr(0,7) == "request"){
                //request action (requires a semaphore and pipe communication)
                //when request is sent, process is held up with a semaphore and does not proceed until semaphore is released by parent
                masterString += "requestResources Action\n";
                deadline -= 1;
                cout << "p" << id << " should stop here forever" << endl;
                int value;
                sem_getvalue(semaphore, &value);
                cout << "Semaphore value: " << value << endl;
                sem_wait(semaphore);
                cout << "p" << id << " kept going" << endl;
                masterString += "requestResources Action Released";
                printf("request sema in p%d released\n", id);

            }
            else if(inputAction.substr(0,13) == "use_resources"){
                //use resources action
                masterString += "useResources Action\n";
            }
            else if(inputAction.substr(0,5)=="print"){
                //print resources used action
                masterString += "printResourcesUsed Action\n";
            }
            else if(inputAction.substr(0,9) == "calculate"){
                //calculate action
                masterString += "calculate Action\n";
            }
            else{
                //end action (needs to end the child process)
                // also needs to send master string to parent (via pipe)
                masterString += "end Action\n";
            }
            if(deadline < 0){
                masterString += "process is late\n";
            }
        //     i++;
        // }
    }

    string printMasterString(){
        return masterString;
    }
    void signal(){
        sem_post(semaphore);
    }

private:
    int deadline;
    int computationTime;
    int numResources;
    sem_t *semaphore;
    int id;
    // vector<string> myActions;
    string masterString;
    vector<map<string,bool> > resourceDict;
};

int main(int argc, char** argv){
    string firstFile;
    string secondFile;

    if(argc == 3){
        for (int i = 0; i < argc; ++i) {
            // cout << argv[i] << endl;
            if(i == 1){ firstFile = argv[i]; } 
            if(i == 2){secondFile = argv[i]; } 
        }   
    }

    cout << "Input File Names:\n" + firstFile + "\n" + secondFile + "\n" << endl;


    int numProcesses;
    int numResources;
    int currentProcess = 1;
    // int deadline;
    // int compTime;
    vector<string> actionMap;
    vector<int> availRss;
    vector<vector<int> > max;
    vector<Process> processes;
    vector<map<string, bool> > resourceDict;

    ifstream inFile(firstFile);
    if (inFile.is_open()) {
        string eachLine;
        while(getline(inFile, eachLine)){
            map<string, bool> eachResource;
            // cout << eachLine << endl;
            //need to find second :
            size_t secondColonPos = eachLine.find(':', eachLine.find(':') + 1);
            if (secondColonPos != string::npos) {
                // Extract the substring after the second colon (remove the space)
                string restOfString = eachLine.substr(secondColonPos + 2);
                cout << "entire line: " << restOfString << endl;
                vector<string> items;
                stringstream ss(restOfString);
                string item;
                while (getline(ss, item, ',')) {
                    if(isspace(item[0])){ item = item.substr(1); }
                    if(!isprint(item.back())){
                        item = item.substr(0, item.size()-1);
                    }
                    eachResource[item] = false;
                }
            }
            resourceDict.push_back(eachResource);
        }
        inFile.close(); // Close the file after reading

    } else {
        cerr << "Unable to open the file...TERMINATING" << endl;
        exit(0);
    }

    for(int i = 0; i<resourceDict.size();i++){
        for (auto const &pair: resourceDict.at(i)) {
            std::cout << "{" << pair.first << ": " << pair.second << "}\n";
        }
    }

    ifstream inputFile(secondFile);
    if(inputFile.is_open()){
        cout << "file opened" << endl;
        if(inputFile >> numResources >> numProcesses){
            printf("Read numResources = %d, Read numProcesses = %d\n", numResources, numProcesses);

            cout << "available resources: " << endl;
            for(int i = 0; i < numResources; i++){
                int tempVar;
                if(inputFile >> tempVar){
                    availRss.push_back(tempVar);
                    cout << tempVar << " ";
                }else{
                    printf("available resource allocation failed...TERMINATING\n");
                    exit(0);
                }
            }
            cout << endl;
        }else{
            printf("error reading numProcesses and numResources from file\n");
        }

        sem_t semaphores[numProcesses];
        for (int i = 0; i < numProcesses; i++) {
            sem_init(&semaphores[i], 0, 0); // Initialize semaphore for each child
            int value;
            sem_getvalue(&semaphores[i], &value);
            cout << "Semaphore " << i <<" value: " << value << endl;
        }


        vector<vector<int> > tempMax(numProcesses, vector<int>(numResources));
        cout << "max rss usage for each process: " << endl;
        for(int i = 0; i < numProcesses; i++){
            for(int j = 0; j < numResources; j++){
                int tempVar;
                if(inputFile >> tempVar){
                    tempMax.at(i).at(j) = tempVar;
                    cout << tempVar << " ";
                }else{
                    printf("max resources per process allocation failed...TERMINATING\n");
                    exit(0);
                }
            }
            cout << endl;
        }
        cout << endl;
        max = tempMax;

        string line;
        int currentProcess, deadline, compTime;
        cout << "bouta start loop" << endl;
        while( getline(inputFile, line) ){
            // cout << line.substr(0, 8) << endl;
            if(line.empty() || line.size() <= 1){
                //skip line
            }else{
                if(line.substr(0, 8) == "process_"){
                    // cout << "process match" << endl;
                    currentProcess = stoi(line.substr(8));
                    // cout << currentProcess << endl;
                    if (inputFile >> deadline >> compTime) { // Read two integers after process header
                        cout << "Process " << currentProcess << " ints: " << deadline << ", " << compTime << endl;
                    }else{
                        cout << "deadline and comptime import failed...TERMINATING" << endl;
                        exit(0);
                    }
                }
                else if(line.substr(0,4) == "end."){
                    cout << "end command: " << line << endl;
                    actionMap.push_back(line);
                    processes.push_back(Process(deadline, compTime, numProcesses, actionMap, resourceDict, &semaphores[currentProcess], currentProcess));
                    actionMap.clear();
                }
                else{
                    cout << "regular command: " << line << endl;
                    actionMap.push_back(line);
                }
            }
        }
    }
    else{
        cout << "open file errored" << endl;
    }
    inputFile.close();

    for(int i = 0; i < 3; i++){
        printf("Process %d:\n", i);
        for(int j = 0; j < processes.at(i).myActions.size(); j++){
            cout << "Action " << j << ": " << processes.at(i).myActions.at(j) << endl;
        }
        
    }
    return 0;
}

