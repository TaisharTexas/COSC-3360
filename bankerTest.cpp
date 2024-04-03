#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int MAX_PROCESSES = 10;
const int MAX_RESOURCES = 5;

vector<int> available(MAX_RESOURCES);
vector<vector<int> > maximum(MAX_PROCESSES, vector<int>(MAX_RESOURCES));
vector<vector<int> > allocation(MAX_PROCESSES, vector<int>(MAX_RESOURCES));
vector<vector<int> > need(MAX_PROCESSES, vector<int>(MAX_RESOURCES));
vector<bool> finished(MAX_PROCESSES, false);

void initialize() {
    // Initialize available resources
    cout << "Enter the number of available resources for each type:" << endl;
    for (int i = 0; i < MAX_RESOURCES; ++i) {
        cin >> available[i];
    }

    // Initialize maximum resources for each process
    cout << "Enter the maximum resources required for each process:" << endl;
    for (int i = 0; i < MAX_PROCESSES; ++i) {
        cout << "Process " << i << ":" << endl;
        for (int j = 0; j < MAX_RESOURCES; ++j) {
            cin >> maximum[i][j];
            need[i][j] = maximum[i][j];
        }
    }
}

bool isSafeState() {
    vector<int> work = available;
    vector<bool> safeSequence(MAX_PROCESSES, false);
    int count = 0;

    while (count < MAX_PROCESSES) {
        bool found = false;
        for (int i = 0; i < MAX_PROCESSES; ++i) {
            if (!finished[i]) {
                bool canExecute = true;
                for (int j = 0; j < MAX_RESOURCES; ++j) {
                    if (need[i][j] > work[j]) {
                        canExecute = false;
                        break;
                    }
                }
                if (canExecute) {
                    for (int j = 0; j < MAX_RESOURCES; ++j) {
                        work[j] += allocation[i][j];
                    }
                    safeSequence[count++] = true;
                    finished[i] = true;
                    found = true;
                }
            }
        }
        if (!found) {
            break; // Unsafe state
        }
    }

    if (count == MAX_PROCESSES) {
        cout << "Safe sequence:" << endl;
        for (int i = 0; i < MAX_PROCESSES; ++i) {
            if (safeSequence[i]) {
                cout << i << " ";
            }
        }
        cout << endl;
        return true; // Safe state
    } else {
        cout << "System is in an unsafe state!" << endl;
        return false; // Unsafe state
    }
}

void requestResources(int processId, vector<int>& request) {
    cout << "Process " << processId << " is requesting resources:" << endl;
    for (int i = 0; i < MAX_RESOURCES; ++i) {
        if (request[i] > need[processId][i] || request[i] > available[i]) {
            cout << "Invalid request!" << endl;
            return;
        }
    }

    for (int i = 0; i < MAX_RESOURCES; ++i) {
        available[i] -= request[i];
        allocation[processId][i] += request[i];
        need[processId][i] -= request[i];
    }

    if (isSafeState()) {
        cout << "Resources granted." << endl;
    } else {
        // Rollback changes
        for (int i = 0; i < MAX_RESOURCES; ++i) {
            available[i] += request[i];
            allocation[processId][i] -= request[i];
            need[processId][i] += request[i];
        }
        cout << "Request denied to avoid unsafe state." << endl;
    }
}

int main() {
    initialize();

    while (true) {
        int processId;
        cout << "Enter process id (0-" << MAX_PROCESSES - 1 << ") or -1 to exit: ";
        cin >> processId;

        if (processId == -1) {
            break;
        }

        if (processId < 0 || processId >= MAX_PROCESSES) {
            cout << "Invalid process id!" << endl;
            continue;
        }

        vector<int> request(MAX_RESOURCES);
        cout << "Enter resource request for process " << processId << ":" << endl;
        for (int i = 0; i < MAX_RESOURCES; ++i) {
            cin >> request[i];
        }

        requestResources(processId, request);
    }

    return 0;
}
