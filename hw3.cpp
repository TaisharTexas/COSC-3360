//Host: pascal.cs.uh.edu
//usr: cosc3360056
//pswd: password8656

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <string>
using namespace std;

struct Action{
    string addr;
    int p;
};

void LRU_offset(vector<int>ids, vector<string>addrs, int totalPageFrames, int framesPerProcess, int numProcesses, int offset){
    vector<Action> processLog;
    
    for(int i = 0; i < addrs.size(); i++){
        
        string key = addrs.at(i);   
        int processToCheck = ids.at(i);
        bool isAddrsLoaded = false;

        if(key == "-1"){
            printf("process %d is done\n============================\n", processToCheck);
        }
        else{

            for (const auto& item : processLog) {
                if(item.addr == key){
                    isAddrsLoaded = true;
                }
            }
            
            if(!isAddrsLoaded){
                printf("addr: %s for process %d not loaded - Page Fault\n", key.c_str(), processToCheck);
                //addrs not in log
                //need to check if theres room to add it
                bool isPFrameOk = false;
                bool isMFrameOk = false;

                
                int ticker = 0;
                for (const auto& pair : processLog) {
                    // printf("pair.addr = %d ::: processToCheck = %d\n", pair.p, processToCheck);
                    if (pair.p == processToCheck) {
                        ticker++;
                    }
                }
                // printf("ticker for %d = %d\n", processToCheck, ticker);

                //check if there's room in the process's specific frame max
                if(ticker < framesPerProcess){ isPFrameOk = true; }
                //check if theres room at all for another page
                if(processLog.size() < totalPageFrames){ isMFrameOk = true; }

                // printf("isPFrameOk = %d ::: isMFrameOk = %d\n",  isPFrameOk ? 1:0, isMFrameOk ? 1:0);

                if(isPFrameOk && isMFrameOk){
                    // both have room
                    struct Action A = {key, processToCheck};
                    processLog.push_back(A);
                    printf("\tProcess %d loaded a page with address: %s\n", processToCheck, key.c_str());
                }
                else if(isPFrameOk && !isMFrameOk){
                    //process has room but not main (need to page replace from a process)
                    printf("\tprocess has room but main does not. pop out least used element (LRU) or least used plus offset (LRUX) and then push new element - page replacement \n");
                    if (offset < processLog.size()) {
                    processLog.erase(processLog.begin() + offset);
                    } else {
                        processLog.erase(processLog.begin());
                    }
                    struct Action A = {key, processToCheck};
                    processLog.push_back(A);
                }
                else if(!isPFrameOk && isMFrameOk){
                    //process is full but main has room (need to page replace from this process)
                    printf("\tprocess is full but main has room (need to find least recently used item from this process) - page replacement\n");

                    int foundCount = 0;
                    for (auto it = processLog.begin(); it != processLog.end(); ++it) {
                        if (it->addr == key) {
                            foundCount++;
                            if (foundCount == offset) {
                                // Found the nth instance, delete it and break the loop
                                processLog.erase(it);
                                break;
                            }
                        }
                    }
                    struct Action A = {key, processToCheck};
                    processLog.push_back(A);
                    
                    
                }
                else if(!isPFrameOk && !isMFrameOk){
                    //neither the process frame or the main fram have room (need to page replace from this process)
                    printf("\tneither the process frame or main have room (need to find least recently added item from this process) - page replacement\n");
                    int foundCount = 0;
                    for (auto it = processLog.begin(); it != processLog.end(); ++it) {
                        if (it->addr == key) {
                            foundCount++;
                            if (foundCount == offset) {
                                // Found the nth instance, delete it and break the loop
                                processLog.erase(it);
                                break;
                            }
                        }
                    }
                    struct Action A = {key, processToCheck};
                    processLog.push_back(A);
                }
            }
            else{
                printf("addr: %s for process %d already loaded\n", key.c_str(), processToCheck);
                for (auto it = processLog.rbegin(); it != processLog.rend(); ++it) {
                    if (it->addr == key) {
                        // Move the found element to the end of the vector
                        processLog.emplace_back(std::move(*it));
                        processLog.erase(std::next(it.base())); // Erase the original element
                        break; // Stop iterating after the first match is found
                    }
                }

                //already have addrs in log dont need to do a page replacement
            }
            std::cout << "============================" << endl;
        }
    }

}

void MRU(vector<int>ids, vector<string>addrs, int totalPageFrames, int framesPerProcess, int numProcesses){
    vector<Action> processLog;
    int numFaults = 0;
    int numReplacements = 0;
    
    for(int i = 0; i < addrs.size(); i++){
        
        string key = addrs.at(i);   
        int processToCheck = ids.at(i);
        bool isAddrsLoaded = false;

        if(key == "-1"){
            printf("process %d is done\n============================\n", processToCheck);
        }
        else{

            for (const auto& item : processLog) {
                if(item.addr == key){
                    isAddrsLoaded = true;
                }
            }
            
            if(!isAddrsLoaded){
                printf("addr: %s for process %d not loaded - Page Fault\n", key.c_str(), processToCheck);
                numFaults++;
                //addrs not in log
                //need to check if theres room to add it
                bool isPFrameOk = false;
                bool isMFrameOk = false;

                
                int ticker = 0;
                for (const auto& pair : processLog) {
                    // printf("pair.addr = %d ::: processToCheck = %d\n", pair.p, processToCheck);
                    if (pair.p == processToCheck) {
                        ticker++;
                    }
                }
                // printf("ticker for %d = %d\n", processToCheck, ticker);

                //check if there's room in the process's specific frame max
                if(ticker < framesPerProcess){ isPFrameOk = true; }
                //check if theres room at all for another page
                if(processLog.size() < totalPageFrames){ isMFrameOk = true; }

                // printf("isPFrameOk = %d ::: isMFrameOk = %d\n",  isPFrameOk ? 1:0, isMFrameOk ? 1:0);

                if(isPFrameOk && isMFrameOk){
                    // both have room
                    struct Action A = {key, processToCheck};
                    processLog.push_back(A);
                    printf("\tProcess %d loaded a page with address: %s\n", processToCheck, key.c_str());
                }
                else if(isPFrameOk && !isMFrameOk){
                    //process has room but not main (need to page replace from a process)
                    printf("\tprocess has room but main does not. pop out last used element (MRU) and then push new element - page replacement \n");
                    numReplacements++;
                    processLog.pop_back();
                    struct Action A = {key, processToCheck};
                    processLog.push_back(A);
                }
                else if(!isPFrameOk && isMFrameOk){
                    //process is full but main has room (need to page replace from this process)
                    printf("\tprocess is full but main has room (need to find most recently added item from this process) - page replacement\n");
                    numReplacements++;

                    for (auto it = processLog.rbegin(); it != processLog.rend(); ++it) {
                        if (it->p == processToCheck) {
                            // std::cout << "Found first instance where p = " << processToCheck << ": " << it->addr << std::endl;
                            processLog.erase((it + 1).base());
                            struct Action A = {key, processToCheck};
                            processLog.push_back(A);
                            break; // Exit loop after finding the first instance
                        }
                    }
                    
                    
                }
                else if(!isPFrameOk && !isMFrameOk){
                    //neither the process frame or the main fram have room (need to page replace from this process)
                    printf("\tneither the process frame or main have room (need to find most recently added item from this process) - page replacement\n");
                    numReplacements++;
                    for (auto it = processLog.rbegin(); it != processLog.rend(); ++it) {
                        if (it->p == processToCheck) {
                            // std::cout << "Found first instance where p = " << processToCheck << ": " << it->addr << std::endl;
                            processLog.erase((it + 1).base());
                            struct Action A = {key, processToCheck};
                            processLog.push_back(A);
                            break; // Exit loop after finding the first instance
                        }
                    }
                }
            }
            else{
                printf("addr: %s for process %d already loaded\n", key.c_str(), processToCheck);
                for (auto it = processLog.rbegin(); it != processLog.rend(); ++it) {
                    if (it->addr == key) {
                        // Move the found element to the end of the vector
                        processLog.emplace_back(std::move(*it));
                        processLog.erase(std::next(it.base())); // Erase the original element
                        break; // Stop iterating after the first match is found
                    }
                }

                //already have addrs in log dont need to do a page replacement
            }
            std::cout << "============================" << endl;
        }
    }
    printf("MRU \nNum Faults: %d\nNum Replacements: %d\n", numFaults, numReplacements);

}

void LIFO(vector<int>ids, vector<string>addrs, int totalPageFrames, int framesPerProcess, int numProcesses){
    vector<Action> processLog;
    int numFaults=0;
    int numReplacements=0;
    
    
    for(int i = 0; i < addrs.size(); i++){
        
        string key = addrs.at(i);   
        int processToCheck = ids.at(i);
        bool isAddrsLoaded = false;

        if(key == "-1"){
            printf("process %d is done\n============================\n", processToCheck);
        }
        else{

            for (const auto& item : processLog) {
                if(item.addr == key){
                    isAddrsLoaded = true;
                }
            }
            
            if(!isAddrsLoaded){
                printf("addr: %s for process %d not loaded - Page Fault\n", key.c_str(), processToCheck);
                numFaults++;
                //addrs not in log
                //need to check if theres room to add it
                bool isPFrameOk = false;
                bool isMFrameOk = false;

                
                int ticker = 0;
                for (const auto& pair : processLog) {
                    // printf("pair.addr = %d ::: processToCheck = %d\n", pair.p, processToCheck);
                    if (pair.p == processToCheck) {
                        ticker++;
                    }
                }
                // printf("ticker for %d = %d\n", processToCheck, ticker);

                //check if there's room in the process's specific frame max
                if(ticker < framesPerProcess){ isPFrameOk = true; }
                //check if theres room at all for another page
                if(processLog.size() < totalPageFrames){ isMFrameOk = true; }

                // printf("isPFrameOk = %d ::: isMFrameOk = %d\n",  isPFrameOk ? 1:0, isMFrameOk ? 1:0);

                if(isPFrameOk && isMFrameOk){
                    // both have room
                    struct Action A = {key, processToCheck};
                    processLog.push_back(A);
                    printf("\tProcess %d loaded a page with address: %s\n", processToCheck, key.c_str());
                }
                else if(isPFrameOk && !isMFrameOk){
                    //process has room but not main (need to page replace from a process)
                    printf("\tprocess has room but main does not. pop out last element (LIFO) and then push new element - page replacement \n");
                    numReplacements++;
                    processLog.pop_back();
                    struct Action A = {key, processToCheck};
                    processLog.push_back(A);
                }
                else if(!isPFrameOk && isMFrameOk){
                    //process is full but main has room (need to page replace from this process)
                    printf("\tprocess is full but main has room (need to find most recently added item from this process) - page replacement\n");
                    numReplacements++;

                    for (auto it = processLog.rbegin(); it != processLog.rend(); ++it) {
                        if (it->p == processToCheck) {
                            // std::cout << "Found first instance where p = " << processToCheck << ": " << it->addr << std::endl;
                            processLog.erase((it + 1).base());
                            struct Action A = {key, processToCheck};
                            processLog.push_back(A);
                            break; // Exit loop after finding the first instance
                        }
                    }
                    
                    
                }
                else if(!isPFrameOk && !isMFrameOk){
                    //neither the process frame or the main fram have room (need to page replace from this process)
                    printf("\tneither the process frame or main have room (need to find most recently added item from this process) - page replacement\n");
                    numReplacements++;
                    for (auto it = processLog.rbegin(); it != processLog.rend(); ++it) {
                        if (it->p == processToCheck) {
                            // std::cout << "Found first instance where p = " << processToCheck << ": " << it->addr << std::endl;
                            processLog.erase((it + 1).base());
                            struct Action A = {key, processToCheck};
                            processLog.push_back(A);
                            break; // Exit loop after finding the first instance
                        }
                    }
                }
            }
            else{
                printf("addr: %s for process %d already loaded\n", key.c_str(), processToCheck);
                // for (auto it = processLog.rbegin(); it != processLog.rend(); ++it) {
                //     if (it->addr == key) {
                //         // Move the found element to the end of the vector
                //         processLog.emplace_back(std::move(*it));
                //         processLog.erase(std::next(it.base())); // Erase the original element
                //         break; // Stop iterating after the first match is found
                //     }
                // }

                //already have addrs in log dont need to do a page replacement
            }
            std::cout << "============================" << endl;
        }
    }
    printf("LIFO\nNum Faults: %d\nNum Replacements: %d\n", numFaults, numReplacements);
}

int main() {
    // Open the file for reading
    ifstream infile("inputs/Assignment3_Input.txt");

    // Variables to store inputs
    int totalPageFrames, pageSize, framesPerProcess, lookaheadWindow, minPool, maxPool, numProcesses;
    
    // The unique ids for the processes that will be requesting addresses
    vector<int> processIds;
    // the number of page frames on disk for each ID
    vector<int> processSizes;
    
    // The queue is two vectors, one vector holds the ID of the process and the other holds the address being requested
    // the id of the process making the request
    vector<int> processIdQueue;
    // the address being requested
    vector<string> processAddrQueue;

    // Read each var from the file
    infile >> totalPageFrames >> pageSize >> framesPerProcess >> lookaheadWindow >> minPool >> maxPool >> numProcesses;

    printf("Total Page Frames:  %d\n", totalPageFrames);
    printf("Page Size:          %d\n", pageSize);
    printf("Frames Per Process: %d\n", framesPerProcess);
    printf("Lookahead Window:   %d\n", lookaheadWindow);
    printf("Min Free Pool Size: %d\n", minPool);
    printf("Max Free Pool Size: %d\n", maxPool);
    printf("Num Processes:      %d\n", numProcesses);

    
    int tempId, tempSize;
    for(int i = 0; i< numProcesses; i++){
        infile >> tempId >> tempSize;
        // printf("Read: tempId: %d, tempAddr: %d\n", tempId, tempSize);
        processIds.push_back(tempId);
        processSizes.push_back(tempSize);
    }
    std::cout << "collected processes" << endl;

    std::cout << "ids: " << endl;
    for(int i= 0; i<processIds.size(); i++){
        std::cout << processIds.at(i) << endl;
    }
    std::cout << "sizes: "<< endl;
    for(int i= 0; i<processSizes.size(); i++){
        std::cout << processSizes.at(i) << endl;
    }

    //grab queue of requests
    string eachAddr;
    int eachId;
    printf("Queue of process requests: \n");
    while(infile >> eachId >> eachAddr){
        processIdQueue.push_back(eachId);
        processAddrQueue.push_back(eachAddr);
        printf("Id: %d, Addr: %s\n", eachId, eachAddr.c_str());
    }

    // Close the file
    infile.close();


    //
    printf("LIFO RUN STARTING\n");
    LIFO(processIdQueue, processAddrQueue, totalPageFrames, framesPerProcess, numProcesses);
    printf("\n\n\nMRU RUN STARTING\n");
    MRU(processIdQueue, processAddrQueue, totalPageFrames, framesPerProcess, numProcesses);
    printf("\n\n\nLRU-0 RUN STARTING\n");
    // LRU_offset(processIdQueue, processAddrQueue, totalPageFrames, framesPerProcess, numProcesses, 0);
    // printf("\n\n\nLRU-%d RUN STARTING\n",lookaheadWindow);
    // LRU_offset(processIdQueue, processAddrQueue, totalPageFrames, framesPerProcess, numProcesses, lookaheadWindow);
    
    



    return 0;
}
