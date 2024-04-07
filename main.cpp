//Host: pascal.cs.uh.edu
//usr: cosc3360056
//pswd: password8656

#include "Process.h"
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
                    cout << "main.cpp - EDFHandler - Requesting " << processRequests.at(i).at(j) << " resource.  I dont have enough." << endl;
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

    pair<string, int> LLFhandler(vector<int> criticalTimes, vector<vector<int> > processRequests, int numResources, int numProcesses, vector<map<string, bool> >& resourceDict, vector<int>& availRss)
    {
        // cout << "main.cpp - Executing LLF method on " << numProcesses << " processes" << endl;
        int smallestSlack = 1000;
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
                    cout << "main.cpp - LLFHandler - Requesting " << processRequests.at(i).at(j) << " resource.  I have " << availRss.at(j) << " available resources." << endl;
                    //we have enough of this particular rss
                }else{
                    goodRequest = false;
                    cout << "main.cpp - LLFHandler - Requesting " << processRequests.at(i).at(j) << " resource.  I dont have enough." << endl;
                    // not enough, cannot service this request rn
                }
            }
            if(goodRequest){ processIndGoodToService.push_back(i); }
            // cout << "finished first EDF step" << endl;
        }
        //find process with smallest deadline out of the processes we can service (uses index)
        cout << "main.cpp - LLFHandler - Found " << processIndGoodToService.size() << " requests to process."  << endl;
        indexOfSmallestProcess = 0;
        for(int i = 0; i < processIndGoodToService.size(); i++)
        {
            // cout << "main.cpp - LLFHandler - Looking at this slacktime: " << criticalTimes.at(processIndGoodToService.at(i)) << endl;
            if(criticalTimes.at(processIndGoodToService.at(i)) < smallestSlack)
            {
                smallestSlack = criticalTimes.at(processIndGoodToService.at(i));
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


        // cout << "main.cpp - LLFHandler - finished third LLF step" << endl;
        cout << "main.cpp - LLFHandler - output sending back to parent is: " << sendBackString << " from process " << indexOfSmallestProcess << endl;
        return make_pair(sendBackString, indexOfSmallestProcess);
    }

    void executeActions(vector<Process> &processes, vector<map<string, bool> >& resourceDict, vector<int>& avilableRss, bool useEDF)
    {
        int numProcesses = processes.size();

        cout << endl 
             << "----------------------------------------------------------------------------------------------------------------" << endl 
             << "main.cpp - Starting the opMode::executeActions method on the Processes that were just made.  We hecking made it!" << endl 
             << "----------------------------------------------------------------------------------------------------------------" << endl 
             << endl ;

        int fd_P2C[numProcesses][2];
        int fd_C2P[numProcesses][2];
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
            if (pipe(fd_P2C[i]) == -1) 
            { //pipe creation failed
                perror("pipe");
                exit(1);
            }
            if (pipe(fd_C2P[i]) == -1) 
            { //pipe creation failed
                perror("pipe");
                exit(1);
            }
            // cout << "main.cpp - Sempahore values in executeActions:  " << processes.at(i).getSemValue() << endl;
        }

        ///////////////////////
        // Open the semaphores
        ///////////////////////
        sem_t* parentSema = sem_open("parentSem", O_CREAT, 0666, 1);
        for (int i = 0; i < numProcesses; i++) 
        {
            temp = "/sema_" + to_string(i);
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

            semaphores[i] = eachSema;   

            processes.at(i).setSem( semaphores[i], parentSema);
            processes.at(i).setPipe(fd_P2C[i][0], fd_C2P[i][1]);
            processes.at(i).setUseDeadline(useEDF);
            cout << "main.cpp - Semaphore for Process " << i << " is opened, locked, and assigned to the Process." << endl;
            cout << "main.cpp - Pipe for Process " << i << " is opened and read/write fds are assigned to the Process." << endl;

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
                cout << endl << "main.cpp ------------------------ Child process " << i << " executing ------------------------------------------" << endl;
                int value;
                sem_getvalue(semaphores[i], &value);
                // cout << "main.cpp - Sempahore value in child after fork(): " << value << endl;
                string actionOut;
                for(int j = 0; j < numActions; j++)
                {
                    //calls every action in the queue for process i 
                    // cout << "main.cpp - About to call Process::executeAction() with the Child process going to send this action to the method: " << processes.at(i).myActions.at(j) << endl;
                    actionOut = processes.at(i).executeAction(processes.at(i).myActions.at(j));

                    // //this is wrong but I dont know where to put it: (pretty sure this needs to go to a pipe to get sent to the parent)
                    // currentRequest = processes.at(i).getCurrentRequest();

                    // cout << "main.cpp - Sempahore value in child after fork() and inside for loop: " << processes.at(i).getSemValue() << endl;
                    if(actionOut == "done")
                    {
                        // cout << "main.cpp - Child " << i << " finished its Process::executeAction() and is done." << endl;
                        //action finished no other actions needed
                    }
                    else if(actionOut == "end")
                    {
                        //end action
                        // need to terminate loop/send master string to parent/terminate child...see lines 215-220
                        write(fd_C2P[i][1], actionOut.c_str(), actionOut.size()+1);
                        cout << "main.cpp - Child " << i << " has reached END and is waiting for go ahead to publish master string" << endl;
                        sem_wait(semaphores[i]);
                    }
                    else
                    {
                        //release action
                        //need to send the released resources to parent so it knows
                        //is a compiled version of the map that needs to be read in and used to update the parent map
                        write(fd_C2P[i][1], actionOut.c_str(), actionOut.size()+1);
                        cout << "main.cpp - Child " << i << " wrote " << actionOut << " to Parent and will now lock" << endl;
                        sem_wait(semaphores[i]);
                        cout << "main.cpp - Child " << i << " unlocked after Parent processed release action" << endl;
                    }

                }
                bigString = processes.at(i).printMasterString();
                // cout << "main.cpp - Child process " << i << "'s string is: " << bigString << endl;
                write(fd_C2P[i][1], bigString.c_str(), bigString.size()+1);
                cout << "main.cpp - Child process " << i << " exiting!!" << endl;
                _exit(0);
            }
        }

        ///////////////////////
        //PARENT process
        ///////////////////////

        cout << endl << endl << "main.cpp ---------------------- Parent executing ---------------------------------------------------" << endl;
        string pipeReads[numProcesses];

        int ticker = 0;           
        bool iAmDone = false;
        vector<bool> numEnds(numProcesses, false);

        do {
            //go through each pipe and if it has stuff in it, grab it
            for(int i = 0; i < numProcesses; i++)
            {
                if(pipeReads[i].empty())
                {
                    fd_set readSet;
                    FD_ZERO(&readSet);
                    FD_SET(fd_C2P[i][0], &readSet);

                    struct timeval timeout;
                    timeout.tv_sec = 0;
                    timeout.tv_usec = 0;

                    cout << "main.cpp - Parent checking pipes for data from child " << i << endl;

                    int ready = select(fd_C2P[i][0] + 1, &readSet, nullptr, nullptr, &timeout);

                    if (ready > 0) 
                    {
                        // Data is available in the pipe, read it into a string
                        char buffer[1024]; // Buffer to read into
                        // close(fd_P2C[i][1]);
                        ssize_t bytesRead = read(fd_C2P[i][0], buffer, sizeof(buffer));
                        // close(fd_P2C[i][0]);
                        if (bytesRead > 0) 
                        {
                            string data(buffer, bytesRead); // Convert char buffer to string
                            cout << "main.cpp - Parent read " << bytesRead << " bytes of data from pipe " << i << ": " << data << endl;

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
            vector<int>computationTimes(numProcesses);
            vector<vector<int> > processRequests(numProcesses, vector<int>(resourceDict.size()));
            bool didIRelease = false;
            

            for(int i = 0; i<numProcesses;i++)
            {
                if(!pipeReads[i].empty())
                {
                    cout << "main.cpp - Parent got data from child " << i << ".  Will parse now." << endl;

                    if(pipeReads[i].substr(0,8) == "release-"){
                        // cout << "main.cpp - Parent got a release from child " << i << endl;
                        didIRelease = true;
                        string releaseStr = pipeReads[i].substr(8);
                        // cout << "main.cpp - release string from child " << i << ": " << releaseStr << endl;
                        size_t pos = 0;

                        while ((pos = releaseStr.find(",")) != string::npos) {
                            int index = 0;
                            string eachInstance = releaseStr.substr(0, pos);
                            for (auto& resourceMap : resourceDict) {
                                // Check if the target key exists in the current map
                                auto it = resourceMap.find(eachInstance);
                                if (it != resourceMap.end()) {
                                // If the key is found, set the corresponding boolean flag to true (which means this instance is now available again :)
                                    it->second = true;
                                    avilableRss.at(index)++;
                                    break;  // Stop searching once the key is found
                                }
                                index++;
                            }
                            releaseStr.erase(0, pos + 1);
                        }
                        pipeReads[i].clear();
                        sem_post(semaphores[i]);
                        cout << "main.cpp - Parent posted semaphore " << i << " after releasing" << endl;
                    }
                    else if(pipeReads[i].substr(0,3) == "end"){
                        numEnds.at(i) = true;
                        pipeReads[i].clear();
                        for(int j=0; j<resourceDict.size();j++)

                        {
                            processRequests.at(i).at(j) = -1; //no resource request from this pipe (either its done or doing something else atm)
                        }                        
                        cout << "main.cpp - child " << i << " has an END in it" << endl;
                    }
                    else{//assume its a request
                        // grab deadline
                        int deadlinePos = pipeReads[i].find('-') + 1;
                        if (deadlinePos > 0)
                        { 
                            // cout << "main.cpp - child " << i << " STOI_1: " << pipeReads[i].find('-') + 1 << endl;
                            int theDeadline = stoi(pipeReads[i].substr(deadlinePos));
                            cout << "main.cpp - child " << i << " DEADLINE: " << pipeReads[i].substr(deadlinePos) << endl;
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
                            cout << "main.cpp - Parent did not get valid request data from child " << i << ". Must be doing something else, or it could be done!." << endl;                    
                        }
                    }
                }
                else
                {
                    for(int j=0; j<resourceDict.size();j++)
                    {
                        processRequests.at(i).at(j) = -1; //no resource request from this pipe (either its done or doing something else atm)
                    }
                    cout << "main.cpp - Parent did not get data from child " << i << ". Must be doing something else.  Maybe the process done." << endl;
                }
            }

            bool allEnded = true;
            for (bool value : numEnds) {
                if (!value) {
                    allEnded = false;
                    break;
                }
            }

            if(allEnded){
                for(int n = 0; n < numProcesses; n++){
                    sem_post(semaphores[n]);
                }
                iAmDone = true;
                cout << "main.cpp --------------- All children returned ENDS.  Must be all done.  STOPPING. --------------------" << endl;

            }
            else if(!didIRelease)
            {
                // if ( !iAmDone )
                // {
                cout << "main.cpp - Parent going to process ED/LL First scheduler." << endl;
                pair<string, int> sendBackResult;
                if(useEDF){ sendBackResult = EDFhandler(processDeadlines, processRequests, resourceDict.size(), numProcesses, resourceDict, avilableRss); }
                else{ sendBackResult = LLFhandler(processDeadlines, processRequests, resourceDict.size(), numProcesses, resourceDict, avilableRss); }
                
                if(sendBackResult.first.size() > 0){
                    pipeReads[sendBackResult.second].clear();
                    // close(fd_P2C[sendBackResult.second][0]);

                    // cout << "main.cpp - Parent finished EDF.  Going to send back these resources: " << sendBackResult.first.c_str() << endl;
                    cout << "main.cpp - Parent sending back: " << sendBackResult.first.c_str() << " which is " << sendBackResult.first.size() + 1 << " bytes to child " << sendBackResult.second << endl;
                    write(fd_P2C[sendBackResult.second][1], sendBackResult.first.c_str(), sendBackResult.first.size()+1);
                    sem_post(semaphores[sendBackResult.second]);
                    // cout << "main.cpp - Parent just wrote the EDF result to child " << sendBackResult.second << "'s pipe.  And posted its semaphore." << endl;
                    // cout << "Main.cpp - Parent calling sem_wait(parentSema) to wait for child " << sendBackResult.second << " to read pipe and sem_post." << endl;
                    sem_wait(parentSema);
                }
                else{
                    cout << "main.cpp - EDF returned nothing. No valid processes to service" << endl;
                }
                    // close(fd_P2C[sendBackResult.second][1]);
                // }
                // else
                // {
                //     
                // }
            }

            ticker ++;
            this_thread::sleep_for(chrono::milliseconds(100)); // wait between pipe checks


        } while( ticker < 200 && !iAmDone);

        // reading final output from each child (last thing parent needs to do)
        string finalString;
        string fileNameCust;
        if(useEDF){fileNameCust = "outputEDF.txt"; }
        else{ fileNameCust = "outputLLF.txt"; }
        ofstream outputFile(fileNameCust);
        for (int i = 0; i < numProcesses; ++i) {
            int status;
            wait(&status); // Wait for child process to finish
            // cout << "main.cpp - Parent done waiting for child process " << i << ".  Time to grab its last string from the pipe." << endl;
            // cout << "main.cpp - Before reading from the pipe, let's look at pipereads[" << i << "]: " << pipeReads[i] << endl;
            finalString += pipeReads[i];
            if (WIFEXITED(status)) {
                char buffer[1024];
                close(fd_P2C[i][0]);
                close(fd_P2C[i][1]);
                close(fd_C2P[i][1]); // Close write end of pipe
                // cout << "main.cpp - Parent about to read from child " << i << "'s pipe." << endl;
                int bytes_read = read(fd_C2P[i][0], buffer, sizeof(buffer)-1 ); // Read from pipe
                cout << "main.cpp - Parent read " << bytes_read << " bytes from child " << i << "'s pipe." << endl;
                close(fd_C2P[i][0]); // Close read end of pipe
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

        // cout << "\nFinal String: " << finalString << endl << "note.... I used pipeReads to build this." << endl;
        if (outputFile.is_open()) {
            // Write the masterString to the file
            outputFile << finalString;

            // Close the file
            outputFile.close();

        } else {
            std::cerr << "Error opening the file." << std::endl;
        }
    }

}//END OF NAMESPACE

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