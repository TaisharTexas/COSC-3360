#include "Process.h"   
#include <unistd.h> 
#include <iostream>

Process::Process(int deadline, int computationTime, int numResources, vector<string>& myActions, vector<map<string, bool> >& resourceDict, int id) {
    this->deadline = deadline;
    this->computationTime = computationTime;
    this->numResources = numResources;
    this->myActions = myActions;
    this->resourceDict = resourceDict;
    this->id = id;
    // this->semaphore = semaphore;
    masterString = "\n";
}


string Process::executeAction(string& inputAction){
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
            string updateStr = inputAction.substr(7) + "-" + to_string(deadline);
            cout << "Process.cpp #" << id << " - Sending " << updateStr.c_str() << " to Parent which is " << updateStr.size()+1 << " bytes of data to write." << endl;;
            updateRequest(updateStr);
            deadline -= 1;
            cout << "Process.cpp #" << id << " - Process " << id << " should stop and wait for the Parent <--------" << endl;
            sem_wait(semaphore);
            // ----- WAITING --------

            cout << endl << "Process.cpp #" << id << " ------- Process " << id << " kept going after a sem_wait returned." << endl;
            masterString += "requestResources Action Released";
            cout << "Process.cpp #" << id << " - Semaphore in Process " << id << " released" << endl;
            char buffer[1024];
            string rssInputFromParent;
            ssize_t bytesRead = read(pipeRead_fd, buffer, sizeof(buffer));
            cout << "Process.cpp #" << id << " - Process " << id << " read " << bytesRead << " bytes of data from pipe: " << buffer << endl;
            cout << "Process.cpp #" << id << " - Releasing Parent to continue cycling" << endl;
            sem_post(parentSem);
            if (bytesRead > 0) {
                string data(buffer, bytesRead); // Convert char buffer to string
                rssInputFromParent = data;
            } 
            cout << "Process.cpp #" << id << " - Process " << id << " sendBackString: " + rssInputFromParent << endl;
        }
        else if(inputAction.substr(0,13) == "use_resources")
        {
            //use resources action
            masterString += "useResources Action:\n";
            string usedRss = inputAction.substr(13);
            size_t commaPos = usedRss.find(',');
            int rssNum = usedRss[commaPos-1] - '0';
            int rssMultiplier = usedRss[commaPos+1] - '0';
            for (const auto& pair : resourceDict.at(rssNum)) 
            {
                // Check if the value is true for the current key
                if (pair.second) 
                {
                    string tempStr = pair.first + "x" + to_string(rssMultiplier);
                    masterString += tempStr;
                }
            }
            masterString += "\n";
        }
        else if(inputAction.substr(0,5)=="print")
        {
            //print resources used action
            masterString += "printResourcesUsed Action\n";
            deadline -= 1;
            cout << "Master String for P" << id << ":\n" << masterString << endl;

        }
        else if(inputAction.substr(0,9) == "calculate")
        {
            //calculate action
            masterString += "calculate Action\n";
            size_t numPos = inputAction.find('(');
            int calcNum = inputAction[numPos] - '0';
            deadline -= calcNum;

        }
        else
        {
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

string Process::printMasterString(){
    return masterString;
}

void Process::signal(){
    sem_post(semaphore);
}

int Process::getSemValue()
{
    int value;
    sem_getvalue( semaphore, &value);
    return value;
}

void Process::setSem(sem_t *theSem, sem_t *theSema){
    semaphore = theSem;
    parentSem = theSema;
}

void Process::setPipe(int fd0_read, int fd1_write)
{
    pipeWrite_fd = fd1_write;
    pipeRead_fd = fd0_read;
}

void Process::updateRequest(string updateStr){
    currentRequest = updateStr;
    write(pipeWrite_fd, updateStr.c_str(), updateStr.size()+1);
}

string Process::getCurrentRequest(){
    return currentRequest;
}
