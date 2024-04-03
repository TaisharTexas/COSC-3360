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
using namespace std;

class Process {
public:
    vector<string> myActions;

    // Constructor with parameters
    Process(int deadline, int computationTime, int numResources, vector<string>& myActions, vector<map<string, bool> >& resourceDict, int id) {
        this->deadline = deadline;
        this->computationTime = computationTime;
        this->numResources = numResources;
        this->myActions = myActions;
        this->resourceDict = resourceDict;
        this->id = id;
        // this->semaphore = semaphore;
        masterString = "\n";
    }


    string executeAction(string& inputAction, sem_t *sem){
        string doneString = "done";
            // int val;
            // sem_getvalue(sem, &val);
            // cout << "Semaphore value at top of executeAction(): " << val << endl;
            // if ( val ) { sem_wait(sem);}
        if(inputAction.substr(0,7) == "release"){
            //release resources action 
            // (communicate which resources are released through pipe to the parent function so it can update what resources are currently available)
            masterString += "releaseResources Action\n";
            doneString = "resources to be released";
        }
        else if(inputAction.substr(0,7) == "request"){
            //request action (requires a semaphore and pipe communication)
            //when request is sent, process is held up with a semaphore and does not proceed until semaphore is released by parent
            masterString += "requestResources Action\n";
            updateRequest(inputAction.substr(7));
            deadline -= 1;
            cout << "p" << id << " should stop here forever" << endl;
            int val;
            sem_getvalue(sem, &val);
            cout << "Semaphore value: " << val << endl;
            sem_wait(sem);
            cout << "p" << id << " kept going" << endl;
            masterString += "requestResources Action Released";
            printf("request sema in p%d released\n", id);

        }
        else if(inputAction.substr(0,13) == "use_resources"){
            //use resources action
            masterString += "useResources Action:\n";
            string usedRss = inputAction.substr(13);
            size_t commaPos = usedRss.find(',');
            int rssNum = usedRss[commaPos-1] - '0';
            int rssMultiplier = usedRss[commaPos+1] - '0';
            for (const auto& pair : resourceDict.at(rssNum)) {
                // Check if the value is true for the current key
                if (pair.second) {
                    string tempStr = pair.first + "x" + to_string(rssMultiplier);
                    masterString += tempStr;
                }
            }
            masterString += "\n";



        }
        else if(inputAction.substr(0,5)=="print"){
            //print resources used action
            masterString += "printResourcesUsed Action\n";
            deadline -= 1;
            cout << "Master String for P" << id << ":\n" << masterString << endl;

        }
        else if(inputAction.substr(0,9) == "calculate"){
            //calculate action
            masterString += "calculate Action\n";
            size_t numPos = inputAction.find('(');
            int calcNum = inputAction[numPos] - '0';
            deadline -= calcNum;

        }
        else{
            //end action (needs to end the child process)
            // also needs to send master string to parent (via pipe)
            masterString += "end Action\n";
            doneString = "end";

        }
        if(deadline < 0){
            masterString += "process is late\n";
        }
        return doneString;
    }

    string printMasterString(){
        return masterString;
    }
    void signal(){
        sem_post(semaphore);
    }
    int getSemValue()
    {
        int value;
        sem_getvalue( semaphore, &value);
        return value;
    }
    void setSem(sem_t *theSem){
        semaphore = theSem;
    }

    void updateRequest(string updateStr){
        currentRequest = updateStr;
    }
    string getCurrentRequest(){
        return currentRequest;
    }

private:
    int deadline;
    int computationTime;
    int numResources;
    string currentRequest;
    sem_t *semaphore;
    int id;
    // vector<string> myActions;
    string masterString;
    vector<map<string, bool> > resourceDict;
};



namespace opMode{
    
    void executeActions(vector<Process> &processes, vector<map<string, bool> >& resourceDict){
        int numProcesses = processes.size();
        printf("\nwe hecking made it!\n\n");

        int fd[numProcesses][2];
        int pid[numProcesses];
        string masterString = "";
        sem_t semaphores[numProcesses];
        const char *semaphore_name = "/my_semaphore";


        // Create the pipes
        for (int i = 0; i < numProcesses; i++) {
            if (pipe(fd[i]) == -1) { //pipe creation failed
                perror("pipe");
                exit(1);
            }
            cout << "Sempahore values in executeActions:  " << processes.at(i).getSemValue() << endl;
        }

        sem_t *sem = sem_open(semaphore_name, O_CREAT , 0666, 1);
        if (sem == SEM_FAILED) {
            perror("Semaphore creation failed.");
            exit(1);
        }

        int value;
        sem_getvalue(sem, &value);
        cout << "Semaphore value: " << value << endl;
        //semaphore creation
        for (int i = 0; i < numProcesses; i++) {
            // sem_init(&semaphores[i], 0, 0); // Initialize semaphore for each child

            processes.at(i).setSem( sem );
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
                sem_getvalue(sem, &value);
                cout << "Sempahore value in child after fork(): " << value << endl;
                // sem_wait(sem);
                string actionOut;
                for(int j = 0; j < numActions; j++){
                    //calls every action in the queue for process i 
                    actionOut = processes.at(i).executeAction(processes.at(i).myActions.at(j), sem);

                    //this is wrong but I dont know where to put it: (pretty sure this needs to go to a pipe to get sent to the parent)
                    currentRequest = processes.at(i).getCurrentRequest();

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
                close(fd[i][0]);
                write(fd[i][1], bigString.c_str(), bigString.size());
                close(fd[i][1]);
                _exit(0);
                // switch(i){
                //     case 0: //p1
                //         printf("running p1\n");
                //         for(int j = 0; j < numActions; j++){
                //             currentP.executeAction(currentP.myActions.at(j));
                //         }
                //         bigString = currentP.printMasterString();
                //         close(fd[i][0]);
                //         write(fd[i][1], bigString.c_str(), bigString.size());
                //         close(fd[i][1]);
                //         _exit(0);
                //     case 1: //p2
                //         printf("running p2\n");
                //         for(int j = 0; j < numActions; j++){
                //             currentP.executeAction(currentP.myActions.at(j));
                //         }
                //         bigString = currentP.printMasterString();
                //         close(fd[i][0]);
                //         write(fd[i][1], bigString.c_str(), bigString.size());
                //         close(fd[i][1]);
                //         _exit(0);
                //     case 2: //p3
                //         printf("running p3\n");
                //         for(int j = 0; j < numActions; j++){
                //             currentP.executeAction(currentP.myActions.at(j));
                //         }
                //         bigString = currentP.printMasterString();
                //         close(fd[i][0]);
                //         write(fd[i][1], bigString.c_str(), bigString.size());
                //         close(fd[i][1]);
                //         _exit(0);
                //     case 3: //p4
                //     case 4: //p5
                //     case 5: //p6
                //     case 6: //p7
                //     case 7: //p8
                //     case 8: //p9
                //     case 9: //p10
                //     default:
                //         break;
                // }
            }


        }
        //PARENT process
        printf("parent reached \n");

        // while(true){
        //     int choice;
        //     int status;
        //     pid_t result = waitpid(pid[choice], &status, WNOHANG);
        //     cout << "Enter 1, 2, or 3 to signal a child process: ";
        //     cin >> choice;

        //     cout << result << endl;

        //     if(choice < numProcesses && result > 0){
        //         processes.at(choice).signal();
        //     }
        //     else if( choice < -1000){
        //         break;
        //     }
        //     else{
        //         cout << "Invalid choice. Try again." << endl;
        //         continue;
        //     }


        // }

        // // wait for each child to complete
        // for(int i = 0; i<numProcesses; i++){
        //     waitpid(pid[i], nullptr, 0);
        // }
        

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
}



// TEST MAIN THAT DOES NOT READ FROM FILES
int main(){

    int numProcesses = 3;
    int numResources = 3;
    int currentProcess = 1;
    int availRss[3] = {3, 3, 3}; // Available instances per resource type (3 resources each with 3 instances available)

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
   resourceDict.at(0)["hilton"] = false;
   resourceDict.at(0)["marriott"] = false;
   resourceDict.at(0)["omni"] = false;
   resourceDict.at(1)["orange"] = false;
   resourceDict.at(1)["mango"] = false;
   resourceDict.at(1)["pear"] = false;
   resourceDict.at(2)["ford"] = false;
   resourceDict.at(2)["mercedes"] = false;
   resourceDict.at(2)["bmw"] = false;


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

    opMode::executeActions(processes, resourceDict);

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
*/