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

int main() {
    const char *semaphore_name = "/my_semaphore";
    const int NUM_FORKS = 5;

    // Create semaphore
    sem_t *sem = sem_open(semaphore_name, O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED) {
        cerr << "Semaphore creation failed." << endl;
        return 1;
    }

    // Create pipes
    int pipes[NUM_FORKS][2];
    for (int i = 0; i < NUM_FORKS; ++i) {
        if (pipe(pipes[i]) == -1) {
            cerr << "Pipe creation failed." << endl;
            return 1;
        }
    }

    // Fork processes
    for (int i = 0; i < NUM_FORKS; ++i) {
        pid_t pid = fork();

        if (pid == -1) {
            cerr << "Fork failed." << endl;
            return 1;
        } else if (pid == 0) {
            // Child process
            close(pipes[i][0]); // Close read end of pipe

            // Synchronize using semaphore
            sem_wait(sem);

            // Write to pipe
            write(pipes[i][1], "Hello from child", sizeof("Hello from child"));
            close(pipes[i][1]); // Close write end of pipe

            sem_post(sem); // Release semaphore
            return 0;
        } else {
            // Parent process
            close(pipes[i][1]); // Close write end of pipe
        }
    }

    // Parent process reads from pipes
    char buffer[100];
    for (int i = 0; i < NUM_FORKS; ++i) {
        read(pipes[i][0], buffer, sizeof(buffer));
        cout << "Received message from child: " << buffer << endl;
        close(pipes[i][0]); // Close read end of pipe
    }

    // Cleanup
    sem_unlink(semaphore_name);
    sem_close(sem);

    return 0;
}
