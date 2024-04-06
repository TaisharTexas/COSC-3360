//Host: pascal.cs.uh.edu
//usr: cosc3360056
//pswd: password8656

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
#include <chrono>
#include <thread>
#include "Process.h"
using namespace std;




namespace opMode{
    pair<string, int> EDFhandler(vector<int> deadlines, vector<vector<int> > processRequests, int numResources, int numProcesses, vector<map<string, bool> >& resourceDict, vector<int>& availRss);
    pair<string, int> LLFhandler(vector<int> deadlines, vector<vector<int> > processRequests, int numResources, int numProcesses, vector<map<string, bool> >& resourceDict, vector<int>& availRss);

    void executeActions(vector<Process> &processes, vector<map<string, bool> >& resourceDict, vector<int>& avilableRss, bool useEDF){
        int numProcesses = processes.size();
        printf("\nwe hecking made it!\n\n");

        int fd[numProcesses][2];
        int pid[numProcesses];
        string masterString = "";
        string temp;
        sem_t *semaphores[numProcesses];
        sem_t *eachSema;
        // char *semaphore_name = "/my_semaphore";


        // Create the pipes
        for (int i = 0; i < numProcesses; i++) {
            if (pipe(fd[i]) == -1) { //pipe creation failed
                perror("pipe");
                exit(1);
            }
            cout << "Sempahore values in executeActions:  " << processes.at(i).getSemValue() << endl;
        }

        for (int i = 0; i < numProcesses; i++) {
            temp = "sema" + to_string(i);
            eachSema = sem_open(temp.c_str(), O_CREAT , 0666, 1);
            if (eachSema == SEM_FAILED) {
                perror("Semaphore creation failed....TERMINATING");
                exit(1);
            }
            semaphores[i] = eachSema;   

            processes.at(i).setSem( semaphores[i] );
            processes.at(i).setPipe(&fd[i][1], &fd[i][0]);
        }
        


        // CHILD Pipeline Code to Run
        for (int i = 0; i < numProcesses; i++) {
            pid[i] = fork();
            if (pid[i] < 0) { //child creation failed
                perror("fork");
                exit(1);
            } else if (pid[i] == 0) {
                // Process currentP = processes.at(i);
                int numActions = processes.at(i).myActions.size();
                string bigString;
                string currentRequest;
                printf("child %d entered\n", i+1);
                int value;
                sem_getvalue(semaphores[i], &value);
                cout << "Sempahore value in child after fork(): " << value << endl;
                string actionOut;
                for(int j = 0; j < numActions; j++){
                    //calls every action in the queue for process i 
                    actionOut = processes.at(i).executeAction(processes.at(i).myActions.at(j));

                    // //this is wrong but I dont know where to put it: (pretty sure this needs to go to a pipe to get sent to the parent)
                    // currentRequest = processes.at(i).getCurrentRequest();

                    cout << "SEmpahore value in child after fork() and inside for loop: " << processes.at(i).getSemValue() << endl;
                    if(actionOut == "done"){
                        //action finished no other actions needed
                    }
                    else if(actionOut == "end"){
                        //end action
                        // need to terminate loop/send master string to parent/terminate child...see lines 215-220
                    }
                    else{
                        //release action
                        //need to send the released resources to parent so it knows
                        //is a compiled version of the map that needs to be read in and used to update the parent map
                    }

                }
                bigString = processes.at(i).printMasterString();
                printf("child %d exited\n", i+1);
                // close(fd[i][0]);
                write(fd[i][1], bigString.c_str(), bigString.size());
                // close(fd[i][1]);
                _exit(0);

            }


        }
        //PARENT process
        printf("parent reached \n");
        string pipeReads[numProcesses];

        int ticker = 0;
        while(ticker < 5){
            //go through each pipe and if it has stuff in it, grab it
            for(int i = 0; i < numProcesses; i++){
                if(pipeReads[i].empty()){
                    fd_set readSet;
                    FD_ZERO(&readSet);
                    FD_SET(fd[i][0], &readSet);

                    struct timeval timeout;
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 0;
                    int ready = select(fd[i][0] + 1, &readSet, nullptr, nullptr, &timeout);
                    if (ready > 0) {
                        // Data is available in the pipe, read it into a string
                        char buffer[1024]; // Buffer to read into
                        // close(fd[i][1]);
                        ssize_t bytesRead = read(fd[i][0], buffer, sizeof(buffer));
                        // close(fd[i][0]);
                        if (bytesRead > 0) {
                            string data(buffer, bytesRead); // Convert char buffer to string
                            cout << "Data read from pipe " << i << ": " << data << endl;

                            pipeReads[i] = data;
                        } else {
                            cout << "Error reading from pipe "<< i << endl;
                        }
                    } else if (ready == 0) {
                        cout << "No data available in pipe "<< i << endl;
                    } else {
                        cout << "Error checking pipe for data." << endl;
                        
                    }
                }

            }

            // need to take inputs I have...determine if theyre release or request
            // do request first
            // break into () and deadline
            vector<int> processDeadlines(numProcesses);
            vector<vector<int> > processRequests(numProcesses, vector<int>(resourceDict.size()));
            
            for(int i = 0; i<numProcesses;i++){
                if(!pipeReads[i].empty()){
                    // grab deadline
                    int deadlinePos = pipeReads[i].find('-') + 1;
                    cout << "child " << i << " STOI_1: " << pipeReads[i].find('-') + 1 << endl;
                    int theDeadline = stoi(pipeReads[i].substr(deadlinePos));
                    cout << "child " << i << " STOI_2: " << pipeReads[i].substr(deadlinePos) << endl;
                    processDeadlines.at(i) = theDeadline;
                    //grab resource requests
                    string justRss = pipeReads[i].substr(1,deadlinePos-3);
                    //should be just the numbers and commas (no closing or opening parenthises)
                    int numRss = resourceDict.size();
                    int currentPos = 0;
                    for(int j = 0; j<numRss;j++){
                        processRequests.at(i).at(j) = justRss[currentPos]-'0';
                        currentPos += 2;
                    }
                    pipeReads[i].clear();
                }
                else{
                    for(int j=0; j<resourceDict.size();j++){
                        processRequests.at(i).at(j) = -1; //no resource request from this pipe (either its done or doing something else atm)
                    }
                }
            }
            
            pair<string, int> sendBackResult;
            if(useEDF){ sendBackResult = EDFhandler(processDeadlines, processRequests, resourceDict.size(), numProcesses, resourceDict, avilableRss); }
            else{ sendBackResult = LLFhandler(processDeadlines, processRequests, resourceDict.size(), numProcesses, resourceDict, avilableRss); }
            
            // close(fd[sendBackResult.second][0]);
            write(fd[sendBackResult.second][1], sendBackResult.first.c_str(), sendBackResult.first.size());
            sem_post(semaphores[sendBackResult.second]);
            // close(fd[sendBackResult.second][1]);

            ticker ++;
            this_thread::sleep_for(chrono::milliseconds(2000)); // wait between pipe checks

        }
        

        // reading final output from each child (last thing parent needs to do)
        string finalString;
        for (int i = 0; i < numProcesses; ++i) {
            int status;
            wait(&status); // Wait for child process to finish
            if (WIFEXITED(status)) {
                char buffer[1024];
                close(fd[i][1]); // Close write end of pipe
                int bytes_read = read(fd[i][0], buffer, sizeof(buffer) - 1); // Read from pipe
                close(fd[i][0]); // Close read end of pipe
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0'; // Null terminate the char array
                    string message(buffer); // Convert char array to string
                    // cout << "Child process " << i << " returned: \n" << message << endl;
                    finalString += message;
                } else {
                    cerr << "Error reading from pipe for child process " << i << endl;
                }
            } else {
                cerr << "Child process " << i << " did not exit normally" << endl;
            }
        }

        cout << "\nFinal String: " << finalString << endl;


    }

    pair<string, int> EDFhandler(vector<int> deadlines, vector<vector<int> > processRequests, int numResources, int numProcesses, vector<map<string, bool> >& resourceDict, vector<int>& availRss){
        cout << "incide EDF method"<< endl;
        int smallestDeadline;
        int indexOfSmallestProcess;
        vector<int> processIndGoodToService;
        string sendBackString = "";
        bool foundRequestToService = false;
        bool rssFlag = true;

        //need to check which requests can be serviced at all rn
        for(int i = 0; i < numProcesses; i++){
            bool goodRequest = true;
            for(int j = 0; j < numResources; j++){
                if(processRequests.at(i).at(j) <= availRss.at(j)){
                    //we have enough of this particular rss
                }else{
                    goodRequest = false;
                    // not enough, cannot service this request rn
                }
            }
            if(goodRequest){ processIndGoodToService.push_back(i); }
            // cout << "finished first EDF step" << endl;
        }
        //find process with smallest deadline out of the processes we can service (uses index)
        indexOfSmallestProcess = 0;
        for(int i = 0; i < processIndGoodToService.size(); i++){
            if(deadlines.at(processIndGoodToService.at(i)) < smallestDeadline){
                smallestDeadline = deadlines.at(processIndGoodToService.at(i));
                indexOfSmallestProcess = processIndGoodToService.at(i);
            }
        }
        // cout << "finished second EDF step" << endl;
        //assign specific instances of each resource for this process
        vector<int> processToService = processRequests.at(indexOfSmallestProcess);
       
        for(int i = 0; i < numResources; i++){
            if(processToService.at(i) > 0){
                availRss.at(i) = availRss.at(i) - processToService.at(i);//update avail resources based on whats being allocated
                int ticker = 0;
                for (const auto& pair : resourceDict.at(i)) {
                    if(pair.second && ticker < processToService.at(i)){ //true means available false means taken
                        sendBackString += pair.first + ",";
                        resourceDict.at(i)[pair.first] = false;
                        ticker++;

                    }
                }
            }
        }
        cout << "finished third EDF step" << endl;
        cout << "output sending back to parent is: " << sendBackString << "== " << indexOfSmallestProcess << endl;
        return make_pair(sendBackString, indexOfSmallestProcess);
    }

    pair<string, int> LLFhandler(vector<int> deadlines, vector<vector<int> > processRequests, int numResources, int numProcesses, vector<map<string, bool> >& resourceDict, vector<int>& availRss){

    }

}//END OF NAMESPACE



// TEST MAIN THAT DOES NOT READ FROM FILES
int main(){

    int numProcesses = 3;
    int numResources = 3;
    int currentProcess = 1;
    // int availRss[3] = {3, 3, 3}; // Available instances per resource type (3 resources each with 3 instances available)
    vector<int>availRss(3);
    availRss.at(0) = 3;
    availRss.at(1) = 3;
    availRss.at(2) = 3;

    //max resources of each type each process will use
                  // {R1,R2,R3}
    int max[3][3] = {{2, 3, 3},  // P1  
                     {2, 2, 2},  // P2  
                     {3, 3, 3}}; // P3
    /*
    R1: Hotel: Hilton, Marriott, Omni
    R2: fruit: orange, mango, pear
    R3: car: Ford, Mercedes, BMW
    */
   vector<map<string, bool> > resourceDict(numResources);
   resourceDict.at(0)["hilton"] = true;
   resourceDict.at(0)["marriott"] = true;
   resourceDict.at(0)["omni"] = true;
   resourceDict.at(1)["orange"] = true;
   resourceDict.at(1)["mango"] = true;
   resourceDict.at(1)["pear"] = true;
   resourceDict.at(2)["ford"] = true;
   resourceDict.at(2)["mercedes"] = true;
   resourceDict.at(2)["bmw"] = true;


    // I want to build each processs first from the txt file
    vector<Process> processes;
    vector<string> actionMap;
    int deadline;
    int compTime;


    deadline = 40;
    compTime = 14;
    actionMap.push_back("request(1,0,0)");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("request(0,2,0)");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("request(0,0,1)");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("request(0,0,1)");
    // actionMap.push_back("request(0,1,0)");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("request(1,0,0)");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("release(2,3,3)");
    // actionMap.push_back("print_resources_used");
    // actionMap.push_back("end");
    Process myProc(deadline, compTime, numProcesses, actionMap, resourceDict, currentProcess );
    processes.push_back( myProc);
    // processes.push_back(Process(deadline, compTime, numProcesses, actionMap, resourceDict, &semaphores[currentProcess] ,currentProcess));
    actionMap.clear();
    currentProcess += 1;
    
    deadline = 30;
    compTime = 11;
    actionMap.push_back("request(0,0,2)");
    // actionMap.push_back("use_resources(1,3)");
    // actionMap.push_back("print_resources_used");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("request(2,2,0)");
    // actionMap.push_back("use_resources(1,2)");
    // actionMap.push_back("release(2,2,2)");
    // actionMap.push_back("print_resources_used");
    // actionMap.push_back("end");
    Process myProc2(deadline, compTime, numProcesses, actionMap, resourceDict, currentProcess );
    processes.push_back( myProc2);
    // processes.push_back(Process(deadline, compTime, numProcesses, actionMap, resourceDict, &semaphores[currentProcess], currentProcess));
    actionMap.clear();
    currentProcess += 1;

    deadline = 40;
    compTime = 14;
    actionMap.push_back("request(1,0,0)");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("request(0,2,1)");
    // actionMap.push_back("print_resources_used");
    // actionMap.push_back("request(0,0,1)");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("request(0,0,1)");
    // actionMap.push_back("print_resources_used");
    // actionMap.push_back("request(0,1,0)");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("request(2,0,0)");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("release(3,3,3)");
    // actionMap.push_back("print_resources_used");
    // actionMap.push_back("end");
    Process myProc3(deadline, compTime, numProcesses, actionMap, resourceDict, currentProcess );
    processes.push_back( myProc3);
    // processes.push_back(Process(deadline, compTime, numProcesses, actionMap, resourceDict, &semaphores[currentProcess], currentProcess));
    actionMap.clear();

    for(int i = 0; i < 3; i++){
        printf("Process %d:\n", i);
        for(int j = 0; j < processes.at(i).myActions.size(); j++){
            cout << "Action " << j << ": " << processes.at(i).myActions.at(j) << endl;
        }
    }

    opMode::executeActions(processes, resourceDict, availRss, true);

    return 0;
}


// NEW MAIN THAT READS IN FILES
/*
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
*/