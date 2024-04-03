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
using namespace std;
// using namespace strLib;

/// @brief this is where im storing the string modification methods I needed
namespace strLib{

    /// @brief Splits a string into an array by a specified delimiter
    /// @param input the string to be split up
    /// @param strLibiter the delimiter to divide the string by
    /// @param count number of elements in the array/string
    /// @return the array of substrings 
    char** splitString(const char* input, const char* strLibiter, int* count) {
        // Make a copy of the input string since strtok modifies the original
        char* inputCopy = strdup(input);

        // Initialize variables
        int capacity = 10;  // Initial capacity of the array
        *count = 0;         // Number of elements in the array
        char** result = (char**)malloc(capacity * sizeof(char*));

        // Tokenize the input string
        char* token = strtok(inputCopy, strLibiter);
        while (token != NULL) {
            // Check if the array needs to be resized
            if (*count == capacity) {
                capacity *= 2;
                result = (char**)realloc(result, capacity * sizeof(char*));
            }

            // Allocate memory for the current token and copy it
            result[*count] = strdup(token);

            // Move to the next token
            (*count)++;
            token = strtok(NULL, strLibiter);
        }

        // Free the temporary copy of the input string
        free(inputCopy);

        return result;
    }

    /// @brief Converts a string to a character array so that the char array methods in C can be used
    /// @param str the string being converted
    /// @return the character array
    char* convertStrToCharArr(string str){
        char newCharArr[str.length() + 1];
        strcpy(newCharArr, str.c_str());  
        cout << "inside methodL: ";
        cout << newCharArr << endl;
        
        
        return newCharArr;
    }
    
    /// @brief Specifically supposed to remove the labels "internal_var" and "input_var"  from in front of the variables names I need to use
    /// @param tempLine the source string
    /// @param subString the substring being removed
    /// @return the source string minus the substring
    char* removeSuffix(char* tempLine, char* subString){
        char* result = strstr(tempLine,subString);

        size_t preLen = result - tempLine;
        size_t postLen = strlen(result + strlen(subString));

        char newString[preLen + postLen + 1];
        strncpy(newString, tempLine, preLen);
        strcpy(newString + preLen, result + strlen(subString));

        printf("Original String: %s\n", tempLine);
        printf("Modified String: %s\n", newString);
        return newString;

    }

}

/// @brief This is where the pipeline is actually being run
namespace opMode{
    /// @brief Takes in the parsed inputs and executes them in respective processes
    /// @param letterVars the integer inputs
    /// @param p_Vars internal variables 
    /// @param actionMap the variables assignments and arithmetic operations to execute
    void executeActions(map<string, int> &letterVars, map<string, int> p_Vars, vector<vector<string> > actionMap){
        printf("\nwe hecking made it!\n\n");



        
 
        cout << "Action Map:" << endl;
        for (const auto& innerVector : actionMap) {
            cout << "Operation: ";
            for (const auto& element : innerVector) {
                cout << element << " ";
            }
            cout << endl;
            
        }
        cout << endl;

        int i;
        int fd[p_Vars.size()][2];
        int pid[p_Vars.size()];

        // Create the pipes
        for (i = 0; i < p_Vars.size(); i++) {
            if (pipe(fd[i]) == -1) {
                perror("pipe");
                exit(1);
            }
        }

        // Create the child processes
        for (i = 0; i < p_Vars.size(); i++) {
            pid[i] = fork();
            // cout << pid[i] << endl;

            if (pid[i] < 0) {
                perror("fork");
                exit(1);
            } else if (pid[i] == 0) {
                // Child process
                int processVar;
                switch(i){
                    /////////////////////////////
                    case 0:
                        // cout << "case0" << endl;
                        for(int r = 0; r < actionMap.size(); r++){
                            if(actionMap[r][1] == "p0"){
                                int LHS;
                                if(actionMap[r][0].size() > 1){
                                    //skip
                                }
                                else{
                                    // cout << "case 0 LHS asigned. LHS = "<<LHS << endl;
                                    LHS = letterVars.at(actionMap[r][0]);
                                }
                                //LHS is a letter-var, can run now
                                if(actionMap[r][2] == "0"){
                                    processVar = LHS;
                                }
                                else if(actionMap[r][2] == "+"){
                                    //addition
                                    processVar += LHS;
                                }
                                else if(actionMap[r][2] == "-"){
                                    //subtraction
                                    processVar -= LHS;
                                }
                                else if(actionMap[r][2] == "*"){
                                    //multiplication
                                    processVar *= LHS;
                                }
                                else if(actionMap[r][2] == "/"){
                                    //division
                                    processVar /= LHS;
                                }
                                
                            }
                        }
                        // cout << "case 0 processVar: " << processVar << endl;
                        close(fd[i][0]);
                        write(fd[i][1], &processVar, sizeof(int));
                        close(fd[i][1]);
                        // cout << "case 0 made it to exit" << endl;
                        _exit(0);
                    case 1:
                        // cout << "case1" << endl;
                        for(int r = 0; r < actionMap.size(); r++){
                            if(actionMap[r][1] == "p1"){
                                int LHS;
                                if(actionMap[r][0].size() > 1){
                                    //skip
                                }
                                else{
                                    // cout << "case 1 LHS assigned. LHS = "<< LHS << endl;
                                    LHS = letterVars.at(actionMap[r][0]);
                                }
                                //LHS is a letter-var, can run now
                                if(actionMap[r][2] == "0"){
                                    processVar = LHS;
                                }
                                else if(actionMap[r][2] == "+"){
                                    //addition
                                    processVar += LHS;
                                }
                                else if(actionMap[r][2] == "-"){
                                    //subtraction
                                    processVar -= LHS;
                                }
                                else if(actionMap[r][2] == "*"){
                                    //multiplication
                                    processVar *= LHS;
                                }
                                else if(actionMap[r][2] == "/"){
                                    //division
                                    processVar /= LHS;
                                }
                                // printf("Cycle #%d\n\tLHS: %d\n\tprocessVar: %d\n", r, LHS, processVar);
                                
                            }
                        }
                        // cout << processVar << endl;
                        close(fd[i][0]);
                        write(fd[i][1], &processVar, sizeof(int));
                        close(fd[i][1]);
                        // cout << "case 1 made it to exit" << endl;
                        _exit(0);
                    case 2:
                        // cout << "case2" << endl;
                        // cout << "acttionmapsize: "<< actionMap.size() << endl;
                        for(int r = 0; r < actionMap.size(); r++){
                            // cout << "case 2 shoud be p2:: " << actionMap.at(r).at(1) << endl;
                            if(actionMap[r][1] == "p2"){
                                int LHS;
                                if(actionMap[r][0].size() > 1){
                                    //skip
                                }
                                else{
                                    // cout << "case 2 LHS assigned. LHS = " << LHS << endl;
                                    LHS = letterVars.at(actionMap[r][0]);
                                }
                                //LHS is a letter-var, can run now
                                if(actionMap[r][2] == "0"){
                                    processVar = LHS;
                                }
                                else if(actionMap[r][2] == "+"){
                                    //addition
                                    processVar += LHS;
                                }
                                else if(actionMap[r][2] == "-"){
                                    //subtraction
                                    processVar -= LHS;
                                }
                                else if(actionMap[r][2] == "*"){
                                    //multiplication
                                    processVar *= LHS;
                                }
                                else if(actionMap[r][2] == "/"){
                                    //division
                                    processVar /= LHS;
                                }
                                // printf("Cycle #%d\n\tLHS: %d\n\tprocessVar: %d\n", r, LHS, processVar);
                                
                            }
                        }
                        // cout << processVar << endl;
                        close(fd[i][0]);
                        write(fd[i][1], &processVar, sizeof(int));
                        close(fd[i][1]);
                        // cout << "case 2 made it to exit" << endl;
                        _exit(0);
                    case 3:
                        // cout << "case3" << endl;
                        // cout << "acttionmapsize: "<< actionMap.size() << endl;
                        for(int r = 0; r < actionMap.size(); r++){
                            // cout << "case 3 shoud be p3:: " << actionMap.at(r).at(1) << endl;
                            if(actionMap[r][1] == "p3"){
                                int LHS;
                                if(actionMap[r][0].size() > 1){
                                    //skip
                                }
                                else{
                                    // cout << "case 3 LHS assigned. LHS = " << LHS << endl;
                                    LHS = letterVars.at(actionMap[r][0]);
                                }
                                //LHS is a letter-var, can run now
                                if(actionMap[r][2] == "0"){
                                    processVar = LHS;
                                }
                                else if(actionMap[r][2] == "+"){
                                    //addition
                                    processVar += LHS;
                                }
                                else if(actionMap[r][2] == "-"){
                                    //subtraction
                                    processVar -= LHS;
                                }
                                else if(actionMap[r][2] == "*"){
                                    //multiplication
                                    processVar *= LHS;
                                }
                                else if(actionMap[r][2] == "/"){
                                    //division
                                    processVar /= LHS;
                                }
                                // printf("Cycle #%d\n\tLHS: %d\n\tprocessVar: %d\n", r, LHS, processVar);
                                
                            }
                        }
                        // cout << processVar << endl;
                        close(fd[i][0]);
                        write(fd[i][1], &processVar, sizeof(int));
                        close(fd[i][1]);
                        // cout << "case 3 made it to exit" << endl;
                        _exit(0);
                    case 4:
                        // cout << "case5" << endl;
                        // cout << "acttionmapsize: "<< actionMap.size() << endl;
                        for(int r = 0; r < actionMap.size(); r++){
                            // cout << "case 5 shoud be p5:: " << actionMap.at(r).at(1) << endl;
                            if(actionMap[r][1] == "p5"){
                                int LHS;
                                if(actionMap[r][0].size() > 1){
                                    //skip
                                }
                                else{
                                    // cout << "case 4 LHS assigned. LHS = " << LHS << endl;
                                    LHS = letterVars.at(actionMap[r][0]);
                                }
                                //LHS is a letter-var, can run now
                                if(actionMap[r][2] == "0"){
                                    processVar = LHS;
                                }
                                else if(actionMap[r][2] == "+"){
                                    //addition
                                    processVar += LHS;
                                }
                                else if(actionMap[r][2] == "-"){
                                    //subtraction
                                    processVar -= LHS;
                                }
                                else if(actionMap[r][2] == "*"){
                                    //multiplication
                                    processVar *= LHS;
                                }
                                else if(actionMap[r][2] == "/"){
                                    //division
                                    processVar /= LHS;
                                }
                                // printf("Cycle #%d\n\tLHS: %d\n\tprocessVar: %d\n", r, LHS, processVar);
                                
                            }
                        }
                        // cout << processVar << endl;
                        close(fd[i][0]);
                        write(fd[i][1], &processVar, sizeof(int));
                        close(fd[i][1]);
                        // cout << "case 4 made it to exit" << endl;
                        _exit(0);
                    case 5:
                    // cout << "case5" << endl;
                        // cout << "acttionmapsize: "<< actionMap.size() << endl;
                        for(int r = 0; r < actionMap.size(); r++){
                            // cout << "case 5 shoud be p5:: " << actionMap.at(r).at(1) << endl;
                            if(actionMap[r][1] == "p5"){
                                int LHS;
                                if(actionMap[r][0].size() > 1){
                                    //skip
                                }
                                else{
                                    // cout << "case 5 LHS assigned. LHS = " << LHS << endl;
                                    LHS = letterVars.at(actionMap[r][0]);
                                }
                                //LHS is a letter-var, can run now
                                if(actionMap[r][2] == "0"){
                                    processVar = LHS;
                                }
                                else if(actionMap[r][2] == "+"){
                                    //addition
                                    processVar += LHS;
                                }
                                else if(actionMap[r][2] == "-"){
                                    //subtraction
                                    processVar -= LHS;
                                }
                                else if(actionMap[r][2] == "*"){
                                    //multiplication
                                    processVar *= LHS;
                                }
                                else if(actionMap[r][2] == "/"){
                                    //division
                                    processVar /= LHS;
                                }
                                // printf("Cycle #%d\n\tLHS: %d\n\tprocessVar: %d\n", r, LHS, processVar);
                                
                            }
                        }
                        // cout << processVar << endl;
                        close(fd[i][0]);
                        write(fd[i][1], &processVar, sizeof(int));
                        close(fd[i][1]);
                        // cout << "case 5 made it to exit" << endl;
                        _exit(0);
                    case 6:
                    // cout << "case6" << endl;
                        // cout << "acttionmapsize: "<< actionMap.size() << endl;
                        for(int r = 0; r < actionMap.size(); r++){
                            // cout << "case 6 shoud be p6:: " << actionMap.at(r).at(1) << endl;
                            if(actionMap[r][1] == "p6"){
                                int LHS;
                                if(actionMap[r][0].size() > 1){
                                    //skip
                                }
                                else{
                                    // cout << "case 6 LHS assigned. LHS = " << LHS << endl;
                                    LHS = letterVars.at(actionMap[r][0]);
                                }
                                //LHS is a letter-var, can run now
                                if(actionMap[r][2] == "0"){
                                    processVar = LHS;
                                }
                                else if(actionMap[r][2] == "+"){
                                    //addition
                                    processVar += LHS;
                                }
                                else if(actionMap[r][2] == "-"){
                                    //subtraction
                                    processVar -= LHS;
                                }
                                else if(actionMap[r][2] == "*"){
                                    //multiplication
                                    processVar *= LHS;
                                }
                                else if(actionMap[r][2] == "/"){
                                    //division
                                    processVar /= LHS;
                                }
                                // printf("Cycle #%d\n\tLHS: %d\n\tprocessVar: %d\n", r, LHS, processVar);
                                
                            }
                        }
                        // cout << processVar << endl;
                        close(fd[i][0]);
                        write(fd[i][1], &processVar, sizeof(int));
                        close(fd[i][1]);
                        // cout << "case 6 made it to exit" << endl;
                        _exit(0);
                    case 7:
                    // cout << "case7" << endl;
                        // cout << "acttionmapsize: "<< actionMap.size() << endl;
                        for(int r = 0; r < actionMap.size(); r++){
                            // cout << "case 7 shoud be p7:: " << actionMap.at(r).at(1) << endl;
                            if(actionMap[r][1] == "p7"){
                                int LHS;
                                if(actionMap[r][0].size() > 1){
                                    //skip
                                }
                                else{
                                    // cout << "case 7 LHS assigned. LHS = " << LHS << endl;
                                    LHS = letterVars.at(actionMap[r][0]);
                                }
                                //LHS is a letter-var, can run now
                                if(actionMap[r][2] == "0"){
                                    processVar = LHS;
                                }
                                else if(actionMap[r][2] == "+"){
                                    //addition
                                    processVar += LHS;
                                }
                                else if(actionMap[r][2] == "-"){
                                    //subtraction
                                    processVar -= LHS;
                                }
                                else if(actionMap[r][2] == "*"){
                                    //multiplication
                                    processVar *= LHS;
                                }
                                else if(actionMap[r][2] == "/"){
                                    //division
                                    processVar /= LHS;
                                }
                                // printf("Cycle #%d\n\tLHS: %d\n\tprocessVar: %d\n", r, LHS, processVar);
                                
                            }
                        }
                        // cout << processVar << endl;
                        close(fd[i][0]);
                        write(fd[i][1], &processVar, sizeof(int));
                        close(fd[i][1]);
                        // cout << "case 7 made it to exit" << endl;
                        _exit(0);
                    case 8:
                    // cout << "case8" << endl;
                        // cout << "acttionmapsize: "<< actionMap.size() << endl;
                        for(int r = 0; r < actionMap.size(); r++){
                            // cout << "case 8 shoud be p8:: " << actionMap.at(r).at(1) << endl;
                            if(actionMap[r][1] == "p8"){
                                int LHS;
                                if(actionMap[r][0].size() > 1){
                                    //skip
                                }
                                else{
                                    // cout << "case 8 LHS assigned. LHS = " << LHS << endl;
                                    LHS = letterVars.at(actionMap[r][0]);
                                }
                                //LHS is a letter-var, can run now
                                if(actionMap[r][2] == "0"){
                                    processVar = LHS;
                                }
                                else if(actionMap[r][2] == "+"){
                                    //addition
                                    processVar += LHS;
                                }
                                else if(actionMap[r][2] == "-"){
                                    //subtraction
                                    processVar -= LHS;
                                }
                                else if(actionMap[r][2] == "*"){
                                    //multiplication
                                    processVar *= LHS;
                                }
                                else if(actionMap[r][2] == "/"){
                                    //division
                                    processVar /= LHS;
                                }
                                // printf("Cycle #%d\n\tLHS: %d\n\tprocessVar: %d\n", r, LHS, processVar);
                                
                            }
                        }
                        // cout << processVar << endl;
                        close(fd[i][0]);
                        write(fd[i][1], &processVar, sizeof(int));
                        close(fd[i][1]);
                        // cout << "case 8 made it to exit" << endl;
                        _exit(0);
                    case 9:
                        // cout << "case9" << endl;
                        // cout << "acttionmapsize: "<< actionMap.size() << endl;
                        for(int r = 0; r < actionMap.size(); r++){
                            // cout << "case 9 shoud be p9:: " << actionMap.at(r).at(1) << endl;
                            if(actionMap[r][1] == "p9"){
                                int LHS;
                                if(actionMap[r][0].size() > 1){
                                    //skip
                                }
                                else{
                                    // cout << "case 9 LHS assigned. LHS = " << LHS << endl;
                                    LHS = letterVars.at(actionMap[r][0]);
                                }
                                //LHS is a letter-var, can run now
                                if(actionMap[r][2] == "0"){
                                    processVar = LHS;
                                }
                                else if(actionMap[r][2] == "+"){
                                    //addition
                                    processVar += LHS;
                                }
                                else if(actionMap[r][2] == "-"){
                                    //subtraction
                                    processVar -= LHS;
                                }
                                else if(actionMap[r][2] == "*"){
                                    //multiplication
                                    processVar *= LHS;
                                }
                                else if(actionMap[r][2] == "/"){
                                    //division
                                    processVar /= LHS;
                                }
                                // printf("Cycle #%d\n\tLHS: %d\n\tprocessVar: %d\n", r, LHS, processVar);
                                
                            }
                        }
                        // cout << processVar << endl;
                        close(fd[i][0]);
                        write(fd[i][1], &processVar, sizeof(int));
                        close(fd[i][1]);
                        // cout << "case 9 made it to exit" << endl;
                        _exit(0);
                    default:
                        break;
                }

            
                // // Perform some computation
                // int result = i * i;

                // // Write the result to the pipe
                // write(fd[i][1], &result, sizeof(result));

                // close(fd[i][1]);  // Close the write end of the pipe
                // exit(0);
            }
        }
/////////////////////////////////////////////////////////////////////////////
        // Parent process
        // cout << "made it to parent" << endl;
        auto it = p_Vars.begin();
        for (i = 0; i < p_Vars.size()-1; i++) {
            int status;
            // cout << "waiting for: " << pid[i] << endl;
            waitpid(pid[i], &status, 0);
            // cout << "done waiting" << endl;

            if (WIFEXITED(status)) {
                int child_result;
                const std::string& key = it->first;
                close(fd[i][1]);  // Close the write end of the pipe

                // Read the results from the pipes
                read(fd[i][0], &child_result, sizeof(child_result));

                printf("Child process %d returned: %d\n", i, child_result);
                p_Vars.at(key) = child_result;

                close(fd[i][0]);  // Close the read end of the pipe
                ++it;
            }
        }
        cout << endl;
        int processVar;
        for(int r = 0; r < actionMap.size(); r++){
            auto lastItem = p_Vars.rbegin();
            // cout << "parent: final key = " << lastItem->first << endl;
            if(actionMap[r][1] == lastItem->first){
                int LHS;
                if(actionMap[r][0].size() > 1){
                    //LHS is a p-var, wait for correct pid to show complete
                    if(actionMap[r][0] == "p0"){
                        // cout << "reading p0: " << LHS << endl;
                        LHS = p_Vars.at("p0");
                    }
                    else if(actionMap[r][0] == "p1"){
                        // cout << "reading p1: " << LHS << endl;
                        LHS = p_Vars.at("p1");
                    }
                    else if(actionMap[r][0] == "p2"){

                        LHS = p_Vars.at("p2");
                    }
                    else if(actionMap[r][0] == "p3"){

                        LHS = p_Vars.at("p3");
                    }
                    else if(actionMap[r][0] == "p4"){

                        LHS = p_Vars.at("p4");
                    }
                    else if(actionMap[r][0] == "p5"){

                        LHS = p_Vars.at("p5");
                    }
                    else if(actionMap[r][0] == "p6"){
                        LHS = p_Vars.at("p6");
                    }
                    else if(actionMap[r][0] == "p7"){

                        LHS = p_Vars.at("p7");
                    }
                    else if(actionMap[r][0] == "p8"){

                        LHS = p_Vars.at("p8");
                    }
                    else if(actionMap[r][0] == "p9"){

                        LHS = p_Vars.at("p9");
                    }
                    // cout << LHS << endl;
                }
                else{
                    // cout << "parent LHS asigned. LHS = "<<LHS << endl;
                    LHS = letterVars.at(actionMap[r][0]);
                }
                // cout << "last item first: " << lastItem->first <<endl;
                //LHS is a letter-var, can run now
                if(actionMap[r][2] == "0"){
                    p_Vars.at(lastItem->first) = LHS;
                    // cout << "LHS == 0 :: " << LHS << endl;
                }
                else if(actionMap[r][2] == "+"){
                    //addition
                    p_Vars.at(lastItem->first) = p_Vars.at(lastItem->first) + LHS;
                }
                else if(actionMap[r][2] == "-"){
                    //subtraction
                    p_Vars.at(lastItem->first) = p_Vars.at(lastItem->first) - LHS;
                }
                else if(actionMap[r][2] == "*"){
                    //multiplication
                    p_Vars.at(lastItem->first) = p_Vars.at(lastItem->first) * LHS;
                    // cout << "LHS = " << LHS << endl;
                    
                }
                else if(actionMap[r][2] == "/"){
                    //division
                    p_Vars.at(lastItem->first) = p_Vars.at(lastItem->first) / LHS;
                }
                // cout << "p2 = " << p_Vars.at("p2") << endl;
                
            }
        }
        // auto lastItem = p_Vars.rbegin();


        // cout << "parent made it to end" << endl;

        cout << "Letter Vars Pairs:" << endl;
        for (const auto& pair : letterVars) {
            const string& key = pair.first;
            const int& value = pair.second;

            cout << key << ": " << value << endl;
        }
        cout << "P-Vars Pairs:" << endl;
        for (const auto& pair : p_Vars) {
            const string& key = pair.first;
            const int& value = pair.second;
            cout << key << ": " << value << endl;
        }

    }
}

/// @brief All of the file parsing happens in main. The pipeline is actually executed in a method so that the forks dont duplicate the hundreds of lines of parsing code I needed to prepare the inputs for execution
/// @param argc command argument count
/// @param argv command arguments themselves
/// @return the return doesnt signify anything besides the completion of everything in main
int main(int argc, char** argv) {

    string varVals = "";
    char** varInptVals;
    int varInptVals_len;
    string pipelineVals = "";
    char** pipelineInstructionVals;
    int pipelineInstrVals_len;


    // cout << "This program has " << argc << " arguments:" << endl;
    //checking if there are at least two file inputs
    if(argc == 3){
        for (int i = 0; i < argc; ++i) {
            // cout << argv[i] << endl;
            if(i == 1){ varVals = argv[i]; } //assign varVals with the file name for the variable input values file
            if(i == 2){pipelineVals = argv[i]; } //assign pipelineVals with the file name for the process pipeline file
        }   
    }

    cout << "Input File Names:\n" + varVals + "\n" + pipelineVals + "\n" << endl;

    //check to make sure the command line arguements are as expected
    if(varVals.length() > 1 && pipelineVals.length() > 0){
        
        //OPEN AND PARSE VARIABLE VALUES.
        std::ifstream inputFile(varVals);
        if (!inputFile.is_open()) {
            std::cerr << "Error opening file: " << varVals << std::endl;
            return 1; // Return an error code
        }
        std::string line;
        while (std::getline(inputFile, line)) {
            // Process each line as needed (for this file there should only be one line)
            // std::cout << line << std::endl;
            
            //convert the str line to an equivalent char arr
            char newCharArr[line.length() + 1];
            strcpy(newCharArr, line.c_str());  
            // cout<< newCharArr << endl;
            int count;
            char** myArr = strLib::splitString(newCharArr, ",", &count);
            //find length of array
            int lengthMyArr = 0;
            while (myArr[lengthMyArr] != NULL) {
                lengthMyArr++;
            }
            //print out values just to verify theyre right
            for(int i = 0; i < lengthMyArr; i++){
                cout << myArr[i] << endl;
            }
            varInptVals = myArr;
            varInptVals_len = lengthMyArr;
        }
        inputFile.close();

        //Can declare letter vars (x, y, z, etc) and pipes (p0, p1, p2, etc) based on length of varInptVals
        //OPEN AND PARSE PIPELINE 
        std::ifstream inputFile2(pipelineVals);
        if (!inputFile2.is_open()) {
            std::cerr << "Error opening file: " << pipelineVals << std::endl;
            return 1; // Return an error code
        }
        std::string line2;
        vector<string> pipelineLines_str;
        int lineTicker = 1;
        while (std::getline(inputFile2, line2)) {
            // Process each line as needed
            std::cout << line2 << std::endl;
            //skip the first two lines
            pipelineLines_str.push_back(line2);
            lineTicker++;
        }

        inputFile.close();
        //I now have a vector where each item in the vector is a line (each line is an operation in the pipeline)
        //I want to convert my vector of strings into an array of char arrays
        char** tempArrayOfStrs = new char*[pipelineLines_str.size()];
        for(int i = 0; i <pipelineLines_str.size(); i++){
            tempArrayOfStrs[i] = new char[pipelineLines_str[i].size() + 1]; 
            strcpy(tempArrayOfStrs[i], pipelineLines_str[i].c_str());
        }
        int lengthMyPipe = 0;
        while (tempArrayOfStrs[lengthMyPipe] != NULL) {
            lengthMyPipe++;
        }

        ///////////////////////////////////////////////////
        //now tempArrayOfStrs is my array of char arrays. Each char array in tempArrayOfStrs is a pipeline operation
        pipelineInstructionVals = tempArrayOfStrs;
        int pipelineInstrVals_len = lengthMyPipe;

        map<string, int> input_vars;
        map<string, int> internal_vars;
        vector<vector<string> > actionMap;

        for(int i = 0; i< pipelineLines_str.size(); i++){
            // printf("Loop #%d start.\n", i);
            char* tempLine = pipelineInstructionVals[i];
           
            if(strstr(tempLine, "input_var") != NULL){ //we found the line of input vars
                // printf("Input Vars: %s\n", tempLine);
                //remove the identifer prefix from the line
                char* result = strstr(tempLine,"input_var ");

                size_t preLen = result - tempLine;
                size_t postLen = strlen(result + strlen("input_var "));

                char newString[preLen + postLen + 1];
                strncpy(newString, tempLine, preLen);
                strcpy(newString + preLen, result + strlen("input_var "));

                // printf("new string: %s\n", newString);
                int count;
                char** segmentedStr = strLib::splitString(newString, ",", &count);
                
                int segmentedStr_len = 0;
                while(segmentedStr[segmentedStr_len] != NULL){
                    // cout << segmentedStr[segmentedStr_len] << endl;
                    // printf("each item: %c\t", segmentedStr[segmentedStr_len][0]);
                    // cout << varInptVals[segmentedStr_len] << endl;
                    char* tempNumVar = varInptVals[segmentedStr_len];
                    // cout << tempNumVar[0] << endl;
                    string tempHolder;
                    tempHolder += segmentedStr[segmentedStr_len][0];
                    input_vars[tempHolder] = stoi(tempNumVar);
                    segmentedStr_len++;

                }
                // printf("\n");

                //print all key-value pairs in the map
                // cout << "Map Key-Value Pairs:\n";
                    for (const auto& pair : input_vars) {
                        const string& key = pair.first;
                        const int& value = pair.second;

                        // cout << "Key: " << key << ", Value: " << value << endl;
                    }
  
            }
            else if(strstr(tempLine, "internal_var") != NULL){ //we found the line of internal vars
                // printf("Internal Vars: %s\n", tempLine);

                // printf("Internal Vars: %s\n", tempLine);
                //remove the identifer prefix from the line
                char* result = strstr(tempLine,"internal_var ");

                size_t preLen = result - tempLine;
                size_t postLen = strlen(result + strlen("internal_var "));

                char newString[preLen + postLen + 1];
                strncpy(newString, tempLine, preLen);
                strcpy(newString + preLen, result + strlen("internal_var "));

                // printf("new string: %s\n", newString);
                int count;
                char** segmentedStr = strLib::splitString(newString, ",", &count);
                
                int segmentedStr_len = 0;
                while(segmentedStr[segmentedStr_len] != NULL){
                    // cout << segmentedStr[segmentedStr_len] << endl;
                    // printf("each item: %s\t", segmentedStr[segmentedStr_len]);
                    if(strstr(segmentedStr[segmentedStr_len], ";") != NULL){   
                        string tempStr;
                        tempStr += segmentedStr[segmentedStr_len][0];
                        tempStr += segmentedStr[segmentedStr_len][1];
                        // cout << tempStr << endl;
                        internal_vars[tempStr] = -1000;
                    }else{
                        internal_vars[segmentedStr[segmentedStr_len]] = -1000;
                    }
                    segmentedStr_len++;

                }
                // printf("\n");

                //print all key-value pairs in the map
                // cout << "Map Key-Value Pairs:\n";
                    for (const auto& pair : internal_vars) {
                        const string& key = pair.first;
                        const int& value = pair.second;

                        // cout << "Key: " << key << ", Value: " << value << endl;
                    }
            }
            else if(strstr(tempLine, "write")!= NULL){ //we found the write line
                // printf("write cmd: %s\n", tempLine);
            }
            else{ //its an operation
                // printf("operation: %s\n", tempLine);
                char* subStrPointer = strstr(tempLine, " -> ");
                int targetIndex = (int)(subStrPointer - tempLine);
                // cout << targetIndex << endl;
                int currentIndex = 0;
                string LHS;
                string RHS;
                string operation;
                if(targetIndex < 2 || tempLine[targetIndex-2] == ' '){//input var to internal var
                    LHS += tempLine[targetIndex-1];
                    RHS += tempLine[targetIndex+3+1];
                    RHS += tempLine[targetIndex+3+2];
                    if(targetIndex < 3){
                        operation = "0";
                    }
                    else{
                        operation += tempLine[targetIndex-3];
                    }
                    if(operation == " "){//No operation
                        operation = "0";
                    }
                    else{//we have some type of operation 
                        
                    }
                    vector<string> action;
                    action.push_back(LHS);
                    action.push_back(RHS);
                    action.push_back(operation);
                    actionMap.push_back(action);

                }
                else if(targetIndex >= 2 && tempLine[targetIndex-2] != ' '){//internal var to internal var
                    LHS += tempLine[targetIndex-2];
                    LHS += tempLine[targetIndex-1];
                    RHS += tempLine[targetIndex+3+1];
                    RHS += tempLine[targetIndex+3+2];
                    if(targetIndex < 4){
                        operation = "0";
                    }else{
                        operation = tempLine[targetIndex-4];
                    }
                    if(operation == " "){//No operation
                        operation = "0";
                    }
                    else{//We have some kind of operation

                    }
                    vector<string> action;
                    action.push_back(LHS);
                    action.push_back(RHS);
                    action.push_back(operation);
                    actionMap.push_back(action);
                }
            }


        }

        opMode::executeActions(input_vars, internal_vars, actionMap);

    } else{
        cout<<"Command Line arguements do not match expected input"<<endl;
    }
 
    return 0;
}   


