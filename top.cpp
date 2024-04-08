#include <vector>
#include <iostream>

#include <unistd.h> 
#include <sys/wait.h>
#include <iostream>
#include <thread>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

using namespace std;

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
    cout << "main.cpp - Input File Names:\n" + firstFile + "\n" + secondFile + "\n" << endl;

    int numProcesses;
    int numResources;
    int currentProcess = 1;
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
                // cout << "entire line: " << restOfString << endl;
                vector<string> items;
                stringstream ss(restOfString);
                string item;
                while (getline(ss, item, ',')) {
                    if(isspace(item[0])){ item = item.substr(1); }
                    if(!isprint(item.back())){
                        item = item.substr(0, item.size()-1);
                    }
                    eachResource[item] = true;
                }
            }
            resourceDict.push_back(eachResource);
        }
        inFile.close(); // Close the file after reading

    } else {
        cerr << "Unable to open the file...TERMINATING" << endl;
        exit(0);
    }

    ifstream inputFile(secondFile);
    if(inputFile.is_open()){
        // cout << "file opened" << endl;
        if(inputFile >> numResources >> numProcesses){
            // printf("Read numResources = %d, Read numProcesses = %d\n", numResources, numProcesses);

            // cout << "available resources: " << endl;
            for(int i = 0; i < numResources; i++){
                int tempVar;
                if(inputFile >> tempVar){
                    availRss.push_back(tempVar);
                    // cout << tempVar << " ";
                }else{
                    printf("available resource allocation failed...TERMINATING\n");
                    exit(0);
                }
            }
            cout << endl;
        }else{
            printf("error reading numProcesses and numResources from file\n");
        }


        vector<vector<int> > tempMax(numProcesses, vector<int>(numResources));
        // cout << "max rss usage for each process: " << endl;
        for(int i = 0; i < numProcesses; i++){
            for(int j = 0; j < numResources; j++){
                int tempVar;
                if(inputFile >> tempVar){
                    tempMax.at(i).at(j) = tempVar;
                    // cout << tempVar << " ";
                }else{
                    printf("max resources per process allocation failed...TERMINATING\n");
                    exit(0);
                }
            }
            // cout << endl;
        }
        // cout << endl;
        max = tempMax;

        string line;
        int currentProcess, deadline, compTime;
        // cout << "bouta start loop" << endl;
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
                        // cout << "Process " << currentProcess << " ints: " << deadline << ", " << compTime << endl;
                    }else{
                        cout << "deadline and comptime import failed...TERMINATING" << endl;
                        exit(0);
                    }
                }
                else if(line.substr(0,4) == "end."){
                    // cout << "end command: " << line << endl;
                    actionMap.push_back(line);
                    Process myProc2(deadline, compTime, numProcesses, actionMap, resourceDict, currentProcess );
                    processes.push_back( myProc2);
                    actionMap.clear();
                }
                else{
                    line = line.substr(0,line.size()-2);
                    // cout << "regular command: " << line << endl;
                    actionMap.push_back(line);
                }
            }
        }
    }
    else{
        cout << "open file errored" << endl;
    }
    inputFile.close();


    cout << endl
         << "-----------------------------------------------------------------" << endl 
         << "------------------- STARTING RUN --------------------------------" << endl 
         << "-----------------------------------------------------------------" << endl 
         << endl
         << "-----------------------------------------------------------------" << endl 
         << "main.cpp - Setting up the Processes and loading in their actions." << endl 
         << "-----------------------------------------------------------------" << endl 
         << endl ;

    for(int i = 0; i < 3; i++)
    {
        cout << "main.cpp - Process: " << i << " being set up with its actions. " << endl;
        for(int j = 0; j < processes.at(i).myActions.size(); j++){
            cout << "main.cpp - Process " << i << " Action " << j << ": " << processes.at(i).myActions.at(j) << endl;
        }
    }

    // cin.get();


    cout << endl
         << "-----------------------------------------------------------------" << endl 
         << "------------------- STARTING EDF RUN ----------------------------" << endl 
         << "-----------------------------------------------------------------" << endl;

    opMode::executeActions(processes, resourceDict, availRss, true);

    

    cout << endl
        << "-----------------------------------------------------------------" << endl 
        << "------------------- STARTING LLF RUN ----------------------------" << endl 
        << "-----------------------------------------------------------------" << endl;

    opMode::executeActions(processes, resourceDict, availRss, false);

    return 0;
}