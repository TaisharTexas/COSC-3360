#include "Process.h"
#include <vector>
#include <iostream>
#include <unistd.h> 
#include <iostream>
#include <thread>
#include <fcntl.h>

namespace opMode
{

    pair<string, int> EDFhandler(vector<int> deadlines, vector<vector<int> > processRequests, int numResources, int numProcesses, vector<map<string, bool> >& resourceDict, vector<int>& availRss)
    {
        cout << "main.cpp - Executing EDF method on " << numProcesses << " processes" << endl;
        int smallestDeadline = 1000;
        int indexOfSmallestProcess;
        vector<int> processIndGoodToService;
        string sendBackString = "";
        bool foundRequestToService = false;
        bool rssFlag = true;

        //need to check which requests can be serviced at all rn
        for(int i = 0; i < numProcesses; i++){
            bool goodRequest = true;
            for(int j = 0; j < numResources; j++){
                if(processRequests.at(i).at(j) >= 0 && processRequests.at(i).at(j) <= availRss.at(j)){
                    cout << "main.cpp - EDFHandler - Requesting " << processRequests.at(i).at(j) << " resource.  I have " << availRss.at(j) << " available resources." << endl;
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
        cout << "main.cpp - EDFHandler - Found " << processIndGoodToService.size() << " requests to process."  << endl;
        indexOfSmallestProcess = 0;
        for(int i = 0; i < processIndGoodToService.size(); i++)
        {
            cout << "main.cpp - EDFHandler - Looking at this deadline: " << deadlines.at(processIndGoodToService.at(i)) << endl;
            if(deadlines.at(processIndGoodToService.at(i)) < smallestDeadline)
            {
                smallestDeadline = deadlines.at(processIndGoodToService.at(i));
                indexOfSmallestProcess = processIndGoodToService.at(i);
            }
        }
        // cout << "finished second EDF step" << endl;
        //assign specific instances of each resource for this process
        vector<int> processToService = processRequests.at(indexOfSmallestProcess);
       
        for(int i = 0; i < numResources; i++)
        {
            if(processToService.at(i) > 0)
            {
                availRss.at(i) = availRss.at(i) - processToService.at(i);//update avail resources based on whats being allocated
                int ticker = 0;
                for (const auto& pair : resourceDict.at(i)) 
                {
                    if(pair.second && ticker < processToService.at(i))
                    { //true means available false means taken
                        sendBackString += pair.first + ",";
                        resourceDict.at(i)[pair.first] = false;
                        ticker++;

                    }
                }
            }
        }
        cout << "main.cpp - EDFHandler - finished third EDF step" << endl;
        cout << "main.cpp - EDFHandler - output sending back to parent is: " << sendBackString << " from process " << indexOfSmallestProcess << endl;
        return make_pair(sendBackString, indexOfSmallestProcess);
    }

    pair<string, int> LLFhandler(vector<int> deadlines, vector<vector<int> > processRequests, int numResources, int numProcesses, vector<map<string, bool> >& resourceDict, vector<int>& availRss)
    {

    }

    void executeActions(vector<Process> &processes, vector<map<string, bool> >& resourceDict, vector<int>& avilableRss, bool useEDF)
    {
        int numProcesses = processes.size();

        cout << endl << "main.cpp - Starting the executeActions method on the Processes that were just made.  We hecking made it!" << endl 
                     << "--------------------------------------------------------------------------------------------------------" << endl << endl ;

        int fd[numProcesses][2];
        int pid[numProcesses];
        string masterString = "";
        string temp;
        sem_t *semaphores[numProcesses];
        sem_t *eachSema;

        ///////////////////////
        // Create the pipes
        ///////////////////////
        for (int i = 0; i < numProcesses; i++) 
        {
            if (pipe(fd[i]) == -1) 
            { //pipe creation failed
                perror("pipe");
                exit(1);
            }
            cout << "main.cpp - Sempahore values in executeActions:  " << processes.at(i).getSemValue() << endl;
        }

        ///////////////////////
        // Open the semaphores
        ///////////////////////
        sem_t* parentSema = sem_open("parentSem", O_CREAT, 0666, 1);
        for (int i = 0; i < numProcesses; i++) 
        {
            temp = "sema" + to_string(i);
            eachSema = sem_open(temp.c_str(), O_CREAT , 0666, 1);
            if (eachSema == SEM_FAILED) 
            {
                perror("Semaphore creation failed....TERMINATING");
                exit(1);
            }
            while ( sem_trywait(eachSema) >=0 )
            {
                // nothing
            }
            cout << "main.cpp - trywait on semaphore just opened: " << sem_trywait( eachSema) << endl;

            semaphores[i] = eachSema;   

            processes.at(i).setSem( semaphores[i], parentSema);
            processes.at(i).setPipe(fd[i][1], fd[i][0]);
        }
        
        

        ///////////////////////
        // CHILD Code to Run
        ///////////////////////
        for (int i = 0; i < numProcesses; i++) 
        {
            pid[i] = fork();
            if (pid[i] < 0) 
            { //child creation failed
                perror("fork");
                exit(1);
            } 
            else if (pid[i] == 0) 
            {
                // Process currentP = processes.at(i);
                int numActions = processes.at(i).myActions.size();
                string bigString;
                string currentRequest;
                cout << endl << "main.cpp - Child process " << i << " executing" << endl
                     << "------------------------------------------" << endl;
                int value;
                sem_getvalue(semaphores[i], &value);
                // cout << "main.cpp - Sempahore value in child after fork(): " << value << endl;
                string actionOut;
                for(int j = 0; j < numActions; j++)
                {
                    //calls every action in the queue for process i 
                    cout << "main.cpp - About to call Process::executeAction() with the Child process going to send this action to the method: " << processes.at(i).myActions.at(j) << endl;
                    actionOut = processes.at(i).executeAction(processes.at(i).myActions.at(j));

                    // //this is wrong but I dont know where to put it: (pretty sure this needs to go to a pipe to get sent to the parent)
                    // currentRequest = processes.at(i).getCurrentRequest();

                    // cout << "main.cpp - Sempahore value in child after fork() and inside for loop: " << processes.at(i).getSemValue() << endl;
                    if(actionOut == "done")
                    {
                        cout << "main.cpp - Child " << i << " finished its Process::executeAction() and is done." << endl;
                        //action finished no other actions needed
                    }
                    else if(actionOut == "end")
                    {
                        //end action
                        // need to terminate loop/send master string to parent/terminate child...see lines 215-220
                    }
                    else
                    {
                        //release action
                        //need to send the released resources to parent so it knows
                        //is a compiled version of the map that needs to be read in and used to update the parent map
                    }

                }
                bigString = processes.at(i).printMasterString();
                cout << " main.cpp - child process " << i << "'s string is: " << bigString << endl;
                printf("main.cpp - child %d exited\n", i);
                write(fd[i][1], bigString.c_str(), bigString.size());
                _exit(0);
            }
        }

        ///////////////////////
        //PARENT process
        ///////////////////////

        cout << endl << endl << "main.cpp - Parent executing" << endl
                     << "---------------------------" << endl;
        string pipeReads[numProcesses];

        int ticker = 0;
        while(ticker < 5){
            //go through each pipe and if it has stuff in it, grab it
            for(int i = 0; i < numProcesses; i++)
            {
                if(pipeReads[i].empty())
                {
                    fd_set readSet;
                    FD_ZERO(&readSet);
                    FD_SET(fd[i][0], &readSet);

                    struct timeval timeout;
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 0;

                    cout << "main.cpp - Parent checking pipes for data from child " << i << endl;

                    int ready = select(fd[i][0] + 1, &readSet, nullptr, nullptr, &timeout);

                    if (ready > 0) 
                    {
                        // Data is available in the pipe, read it into a string
                        char buffer[1024]; // Buffer to read into
                        // close(fd[i][1]);
                        ssize_t bytesRead = read(fd[i][0], buffer, sizeof(buffer));
                        // close(fd[i][0]);
                        if (bytesRead > 0) 
                        {
                            string data(buffer, bytesRead); // Convert char buffer to string
                            cout << "main.cpp - Data read from pipe " << i << ": " << data << endl;

                            pipeReads[i] = data;
                        } 
                        else 
                        {
                            cout << "Error reading from pipe "<< i << endl;
                        }
                    } 
                    else if (ready == 0) 
                    {
                        cout << "main.cpp - No new data available in pipe "<< i << endl;
                    } 
                    else 
                    {
                        cout << "Error checking pipe for data." << endl;
                    }
                }

            }

            // need to take inputs I have...determine if theyre release or request
            // do request first
            // break into () and deadline
            vector<int> processDeadlines(numProcesses);
            vector<vector<int> > processRequests(numProcesses, vector<int>(resourceDict.size()));
            
            for(int i = 0; i<numProcesses;i++)
            {
                if(!pipeReads[i].empty())
                {
                    cout << "main.cpp - Got data from child.  Will parse now." << endl;
                    // grab deadline
                    int deadlinePos = pipeReads[i].find('-') + 1;
                    if (deadlinePos > 0)
                    { 
                        cout << "main.cpp - child " << i << " STOI_1: " << pipeReads[i].find('-') + 1 << endl;
                        int theDeadline = stoi(pipeReads[i].substr(deadlinePos));
                        cout << "main.cpp - child " << i << " STOI_2: " << pipeReads[i].substr(deadlinePos) << endl;
                        processDeadlines.at(i) = theDeadline;
                        //grab resource requests
                        string justRss = pipeReads[i].substr(1,deadlinePos-3);
                        //should be just the numbers and commas (no closing or opening parenthises)
                        int numRss = resourceDict.size();
                        int currentPos = 0;
                        for(int j = 0; j<numRss;j++)
                        {
                            processRequests.at(i).at(j) = justRss[currentPos]-'0';
                            currentPos += 2;
                        }
                        // pipeReads[i].clear();
                    }
                    else
                    {
                        for(int j=0; j<resourceDict.size();j++)
                        {
                            processRequests.at(i).at(j) = -1; //no resource request from this pipe (either its done or doing something else atm)
                        }
                        cout << "main.cpp - Did not get valid request data from child " << i << ". Must be doing something else." << endl;                    
                    }
                }
                else
                {
                    for(int j=0; j<resourceDict.size();j++)
                    {
                        processRequests.at(i).at(j) = -1; //no resource request from this pipe (either its done or doing something else atm)
                    }
                    cout << "main.cpp - Did not get data from child " << i << ". Must be doing something else." << endl;
                }
            }
            
            cout << "main.cpp - Parent going to process Earliest Deadline First scheduler." << endl;
            pair<string, int> sendBackResult;
            if(useEDF){ sendBackResult = EDFhandler(processDeadlines, processRequests, resourceDict.size(), numProcesses, resourceDict, avilableRss); }
            else{ sendBackResult = LLFhandler(processDeadlines, processRequests, resourceDict.size(), numProcesses, resourceDict, avilableRss); }
            
            pipeReads[sendBackResult.second].clear();
            // close(fd[sendBackResult.second][0]);

            cout << "main.cpp - Finished EDF.  Going to send back this: " << sendBackResult.first.c_str() << " to process num: " << sendBackResult.second << endl;
            write(fd[sendBackResult.second][1], sendBackResult.first.c_str(), sendBackResult.first.size());
            sem_post(semaphores[sendBackResult.second]);
            cout << "Main.cpp - Locking parent to wait for child to read pipe" << endl;
            sem_wait(parentSema);
            cout << "main.cpp - Just wrote the EDF result to the appropriate child's pipe.  And posted its semaphore." << endl;
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

}//END OF NAMESPACE
int main()
{

    int numProcesses = 3;
    int numResources = 3;
    int currentProcess = 0;
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

    cout << endl << "main.cpp - Setting up the Processes and loading in their actions." << endl 
                 << "-----------------------------------------------------------------" << endl << endl ;

    for(int i = 0; i < 3; i++)
    {
        cout << "main.cpp - Process: " << i << " being set up with its actions. " << endl;
        for(int j = 0; j < processes.at(i).myActions.size(); j++){
            cout << "main.cpp - Process " << i << " Action " << j << ": " << processes.at(i).myActions.at(j) << endl;
        }
    }

    opMode::executeActions(processes, resourceDict, availRss, true);

    return 0;
}