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
    Process(int deadline, int computationTime, int numResources, vector<string>& myActions, vector<map<string, bool> >& resourceDict, sem_t *semaphore, int id) {
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
        int i = 0;
        while(i < myActions.size()){

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
                sem_wait(semaphore);
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
            i++;
        }
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
    vector<map<string, bool> > resourceDict;

    void calculate(int x){
        deadline -= x; //literally just burns x computation cycles
        printf("calculate action\n");
    }

    void requestResources(){
        deadline -= 1; // a request consumes a single cycle regardless of whether its accepted or not
    }

    /* 
    A `use_resources(x,y)' instruction with two parameters ``uses'' the allocated resources by inserting y times the English words 
    found within each resource (type) in the master string while maintaining the words alphabetically sorted, 
    and its computation time is indicated by x.
    */
    void useResources(int x, int y){
        deadline -= x; // burns x computation cycles
        printf("adding to master string: %d", y);
        //need to look at current resources this child process has been given and print them y times to the master string
    }

    void releaseResources(){
        deadline -= 1; // consumes a single cycle
    }

    /*
    Also, print the process number and its current master string whenever the `print_resources_used' instruction is executed, which takes 1 unit of computation time. 
    */
    void printResources(){
        deadline -= 1; // consumes a single cycle
        printf("printing current state of the master string for the specific child process this is called in");

    }
    /*
    The `end’ command should release any allocated resources and stop the execution of the relevant process. The `end’ command does not take any computation time.
    */
    void end(){

    }
};



namespace opMode{
    
    void executeActions(vector<Process> &processes, vector<map<string, bool> >& resourceDict){
        int numProcesses = processes.size();
        printf("\nwe hecking made it!\n\n");

        int fd[numProcesses][2];
        int pid[numProcesses];
        string masterString = "";

        // Create the pipes
        for (int i = 0; i < numProcesses; i++) {
            if (pipe(fd[i]) == -1) { //pipe creation failed
                perror("pipe");
                exit(1);
            }
        }

        // CHILD Pipeline Code to Run
        for (int i = 0; i < numProcesses; i++) {
            pid[i] = fork();
            if (pid[i] < 0) { //child creation failed
                perror("fork");
                exit(1);
            } else if (pid[i] == 0) {
                Process currentP = processes.at(i);
                int numActions = currentP.myActions.size();
                string bigString;
                printf("child %d entered\n", i);
                for(int j = 0; j < numActions; j++){
                    currentP.executeAction(currentP.myActions.at(j));
                }
                bigString = currentP.printMasterString();
                printf("child %d exited\n", i);
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
    sem_t semaphores[numProcesses];
    int deadline;
    int compTime;

    for (int i = 0; i < numProcesses; i++) {
        sem_init(&semaphores[i], 0, 0); // Initialize semaphore for each child
    }

    deadline = 40;
    compTime = 14;
    // actionMap.push_back("request(1,0,0)");
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
    actionMap.push_back("request(1,1,1)");
    // actionMap.push_back("request(1,1,1)");
    // actionMap.push_back("request(1,1,1)");
    processes.push_back(Process(deadline, compTime, numProcesses, actionMap, resourceDict, &semaphores[currentProcess] ,currentProcess));
    actionMap.clear();
    currentProcess += 1;
    
    deadline = 30;
    compTime = 11;
    // actionMap.push_back("request(0,0,2)");
    // actionMap.push_back("use_resources(1,3)");
    // actionMap.push_back("print_resources_used");
    // actionMap.push_back("use_resources(1,1)");
    // actionMap.push_back("request(2,2,0)");
    // actionMap.push_back("use_resources(1,2)");
    // actionMap.push_back("release(2,2,2)");
    // actionMap.push_back("print_resources_used");
    // actionMap.push_back("end");
    actionMap.push_back("request(1,1,1)");
    // actionMap.push_back("request(1,1,1)");
    // actionMap.push_back("request(1,1,1)");
    processes.push_back(Process(deadline, compTime, numProcesses, actionMap, resourceDict, &semaphores[currentProcess], currentProcess));
    actionMap.clear();
    currentProcess += 1;

    deadline = 40;
    compTime = 14;
    // actionMap.push_back("request(1,0,0)");
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
    actionMap.push_back("request(1,1,1)");
    // actionMap.push_back("request(1,1,1)");
    // actionMap.push_back("request(1,1,1)");
    processes.push_back(Process(deadline, compTime, numProcesses, actionMap, resourceDict, &semaphores[currentProcess], currentProcess));
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

