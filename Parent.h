#ifndef PARENT_H
#define PARENT_H


#include <vector>
#include <map>

using namespace std;

class Parent
{
    public:

    pair<string, int> scheduleProcess(vector<int> deadlines, vector<vector<int> > processRequests, int numResources, int numProcesses, vector<map<string, bool> >& resourceDict, vector<int>& availRss);
    void createChildProcess();



    private:


}





#endif