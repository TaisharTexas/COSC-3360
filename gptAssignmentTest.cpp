#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;

int MAX_RESOURCES = 3;
sem_t *sem;

void requestResource(int processId, int requested) {
    sem_wait(sem); // Wait for semaphore

    if (requested <= MAX_RESOURCES) {
        cout << "Process " << processId << " is requesting " << requested << " resource(s)." << endl;
        cout << "Resource granted." << endl;
        MAX_RESOURCES -= requested; // Update available resources
    } else {
        cout << "Process " << processId << " is requesting too many resources. Request denied." << endl;
    }

    sem_post(sem); // Release semaphore
}

int main() {
    sem = sem_open("/my_semaphore", O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED) {
        cerr << "Semaphore creation failed." << endl;
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        cerr << "Fork failed." << endl;
        return 1;
    } else if (pid == 0) {
        // Child process 1
        requestResource(1, 2);
    } else {
        // Parent process
        pid_t pid2 = fork();

        if (pid2 == -1) {
            cerr << "Fork failed." << endl;
            return 1;
        } else if (pid2 == 0) {
            // Child process 2
            requestResource(2, 1);
        } else {
            // Parent process
            wait(NULL);
            wait(NULL);
            sem_unlink("/my_semaphore");
            sem_close(sem);
        }
    }

    return 0;
}
