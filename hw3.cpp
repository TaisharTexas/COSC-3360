//Host: pascal.cs.uh.edu
//usr: cosc3360056
//pswd: password8656

#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

int main() {
    // Open the file for reading
    ifstream infile("inputs/Assignment3_Input.txt");

    // Variables to store the numbers
    int totalPageFrames, pageSize, framesPerProcess, lookaheadWindow, minPool, maxPool, numProcesses;
    vector<int> processIds;
    vector<int> processAddrs;

    // Read each var from the file
    infile >> totalPageFrames >> pageSize >> framesPerProcess >> lookaheadWindow >> minPool >> maxPool >> numProcesses;

    printf("Total Page Frames:  %d\n", totalPageFrames);
    printf("Page Size:          %d\n", pageSize);
    printf("Frames Per Process: %d\n", framesPerProcess);
    printf("Lookahead Window:   %d\n", lookaheadWindow);
    printf("Min Free Pool Size: %d\n", minPool);
    printf("Max Free Pool Size: %d\n", maxPool);
    printf("Num Processes:      %d\n", numProcesses);

    
    int tempId, tempAddr;
    infile >> tempId >> tempAddr; // clean inputs
    for(int i = 0; i< numProcesses; i++){
        printf("Read: tempId: %d, tempAddr: %d\n", tempId, tempAddr);
        infile >> tempId >> tempAddr;
        processIds.push_back(tempId);
        processAddrs.push_back(tempAddr);
    }

    // Close the file
    infile.close();


    return 0;
}
