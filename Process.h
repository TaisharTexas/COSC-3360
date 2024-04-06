#ifndef PROCESS_H
#define PROCESS_H
#include <vector>
#include <string.h>
#include <semaphore.h>
#include <map>

using namespace std;

class Process {
public:
    vector<string> myActions;

    // Constructor with parameters
    Process(int deadline, int computationTime, int numResources, vector<string>& myActions, vector<map<string, bool> >& resourceDict, int id);
    
    string executeAction(string& inputAction);
    
    string printMasterString();
    
    void signal();
    
    int getSemValue();
    
    void setSem(sem_t *theSem, sem_t *parentSema);
    
    void setPipe(int fd0_read, int fd1_write);
    
    void updateRequest(string updateStr);
    
    string getCurrentRequest();

private:
    int deadline;
    int computationTime;
    int numResources;
    int pipeWrite_fd;
    int pipeRead_fd;
    string currentRequest;
    sem_t *semaphore;
    sem_t *parentSem;
    int id;
    // vector<string> myActions;
    string masterString;
    vector<map<string, bool> > resourceDict;

};//END OF PROCESS CLASS
#endif
