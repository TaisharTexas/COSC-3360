#include <iostream>
#include <vector>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

const int NUM_CHILDREN = 3;
int child_vars[NUM_CHILDREN] = {3, 3, 3};
sem_t semaphores[NUM_CHILDREN];

void childProcess(int index) {
    while (child_vars[index] > 0) {
        sem_wait(&semaphores[index]); // Wait for semaphore signal
        child_vars[index]--; // Decrement the variable
        cout << "Child " << index + 1 << " decremented: " << child_vars[index] << endl;
    }
}

int main() {
    vector<pid_t> childPids;
    // Initialize semaphores
    for (int i = 0; i < NUM_CHILDREN; i++) {
        sem_init(&semaphores[i], 0, 0); // Initialize semaphore for each child
    }

    // Create child processes
    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            childProcess(i);
            return 0;
        } else if (pid > 0) { // Parent process
            childPids.push_back(pid);
        } else { // Fork failed
            cerr << "Fork failed." << endl;
            return 1;
        }
    }

    // Parent process logic
    while (!childPids.empty()) {
        int userInput;
        cout << "Enter a number (1, 2, or 3): ";
        cin >> userInput;

        if (userInput >= 1 && userInput <= NUM_CHILDREN) {
            sem_post(&semaphores[userInput - 1]); // Post semaphore for the selected child
            waitpid(childPids[userInput - 1], NULL, 0); // Wait for the selected child to finish
            childPids.erase(childPids.begin() + userInput - 1); // Remove finished child from the list
        } else {
            cerr << "Invalid input." << endl;
        }
    }

    // Clean up semaphores
    for (int i = 0; i < NUM_CHILDREN; i++) {
        sem_destroy(&semaphores[i]);
    }

    return 0;
}
