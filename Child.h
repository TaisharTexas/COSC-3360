#ifndef CHILD_H
#define CHILD_H
#include <vector>
#include <string.h>
#include <semaphore.h>
#include <map>

using namespace std;

class Child
{
    public:
    vector<string> myActions;

    // Constructor with parameters
    Child(int deadline, int computationTime, int numResources, vector<string>& myActions, vector<map<string, bool> >& resourceDict, int id);
    
    string executeAction(string& inputAction);
    
    void run();

    string printMasterString();
    
    void signal();
    
    int getSemValue();
    
    void setSem(sem_t *theSem, sem_t *parentSema);
    
    void setPipe(int fd0_read, int fd1_write);
    
    void updateRequest(string updateStr);

    void setUseDeadline(bool flag);
    
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
    bool useDeadline;
    // vector<string> myActions;
    string masterString;
    vector<map<string, bool> > resourceDict;
}





#endif