//Host: pascal.cs.uh.edu
//usr: cosc3360056
//pswd: password8656

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

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
    cout << "collected processes" << endl;

    cout << "ids: " << endl;
    for(int i= 0; i<processIds.size(); i++){
        cout << processIds.at(i) << endl;
    }
    cout << "sizes: "<< endl;
    for(int i= 0; i<processSizes.size(); i++){
        cout << processSizes.at(i) << endl;
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

    return 0;
}
