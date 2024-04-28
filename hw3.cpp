//Host: pascal.cs.uh.edu
//usr: cosc3360056
//pswd: password8656

#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Open the file for reading
    ifstream infile("inputs/Assignment3_Input.txt");

    // Variables to store the numbers
    int totalPageFrames, pageSize, framesPerProcess, lookaheadWindow, minPool, maxPool, numProcesses;

    // Read each number from the file
    infile >> totalPageFrames >> pageSize >> framesPerProcess >> lookaheadWindow >> minPool >> maxPool >> numProcesses;
    

    // Close the file
    infile.close();

    cout << totalPageFrames << endl;
    cout << pageSize << endl;
    cout << framesPerProcess << endl;
    cout << lookaheadWindow << endl;
    cout << minPool << endl;
    cout << maxPool << endl;
    cout << numProcesses << endl;


    return 0;
}
