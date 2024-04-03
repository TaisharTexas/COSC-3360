#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char* argv[]){

    int x = 12;
    int y = 1;
    int z = 7;

    int pfd1[2];
    int pfd2[2]; //pipe descriptors

    int processVar; //variable used across all the processes
     int temp;

    pipe(pfd1);
    pipe(pfd2); // pipes

    switch(fork()){//child 1, p0
        case -1:
            printf("error");
            exit(1);
        case 0:
            //do cool things here uwu
            processVar = x;
            close(pfd1[0]);
            write(pfd1[1], &processVar, sizeof(int));
            close(pfd1[1]);
            printf("p0 = %d\n", processVar);
            _exit(0);
        default:
            break;
    }

    switch(fork()){//child 2, p1
        case -1:
            printf("error");
            exit(1);
        case 0:
            //more dope stuff :)
            processVar = y;
            temp = processVar + z;
            
            close(pfd2[0]);
            write(pfd2[1], &processVar, sizeof(int));
            close(pfd2[1]);
            printf("p1 = %d\n", processVar);
            _exit(0);
        default:
            break;
    }
   

    switch(fork()){//child 3, p2
        case -1:
            printf("error");
            exit(1);
        case 0:
            //more dope stuff :)
            close(pfd1[1]);
            close(pfd2[1]);
            read(pfd2[0], &processVar, sizeof(int));//read p1
            //operator is a minus
            temp = processVar;
            read(pfd1[0], &processVar, sizeof(int));
            processVar = temp - processVar;
            printf("p2 = %d\n", processVar);
            _exit(0);
        default:
            break;
    }
    

    // printf("now parent closes all pipes -- children close themselves");

    // int id = fork(); //after this everything will run in both processes -- later, make an int array of length == length of pipeline vector. each index in array will hold the id for a fork
    // if(id == -1){ return -200; }//-200 means fork 1 failed -- later when using array, make the error start at 200 and iterate up by 1 for each index or something
    // int id2 = -1;
    // if(id != 0){ //if ID not 0 then we are the parent
    //     // id = -1000;
    //     id2 = fork();
    //     if(id2 == -1){ return -201; }//-201 means fork 2 failed
    //     printf("parent\n\tID: (%d, %d)\n", id, id2);
    //     fflush(stdout);
    //     //pipe output from parent to processVar in child 2
    //     //pipe output from child1 to processVar in child 2

    // }
    // //assuming both forks worked (no reason they shouldnt), there is one parent process with 2 child processes (each running duplicates of the following instrutions)
    
    // // printf("TEST LINE");

    // if(id == 0 && id2 !=0){//1st Child
    //     printf("1st Child\n\tID: (%d, %d)\n", id, id2);
    //     fflush(stdout);
    //     processVar = x;
    // }
    // // else if (id == -1000 && id2 != 0){//PARENT
    // //     printf("parent\n\tID: (%d, %d)\n", id, id2);
    // //     fflush(stdout);
    // //     processVar = y;
    // //     processVar += z;
    // // }
    // else if(id !=0 && id2 == 0){//2nd child
    //     printf("2nd Child\n\tID: (%d, %d)\n", id, id2);
    //     fflush(stdout);

    //     processVar = y;
    //     processVar += z;
    // }

    return 0;
}






// ///PIPES AND FORKS
// int main(int argc, char* argv[]){
//     int arr[] = {1, 2, 3, 4, 1, 2};
//     int arrSize = sizeof(arr) / sizeof(int);
//     int start, end;
//     int fd[2];
//     // printf("dodad");
//     if(pipe(fd) == -1){
//         return 1;
//     }
//     int id = fork();
//     if(id == -1){ return 2; }

//     if(id==0){
//         start = 0;
//         end = arrSize/2;
//     } else{
//         start = arrSize/2;
//         end = arrSize;
//     }

//     int sum = 0;
//     int i;
//     for(i = start; i< end; i++){
//         sum += arr[i];
//     }
//     printf("calculated partial sum: %d\n", sum);

//     if(id==0){
//         close(fd[0]);
//         write(fd[1], &sum, sizeof(sum));
//         close(fd[1]);
//     } else{
//         int sumFromChild;
//         close(fd[1]);
//         read(fd[0], &sumFromChild, sizeof(sumFromChild));

//         int totalSum = sum + sumFromChild;
//         printf("total sum: %d\n", totalSum);
//         wait(NULL);
//     }

//     return 0;
// }

// ///PIPES AND FORKS
// int main(int argc, char* argv[]){
//     //fd[0] - read
//     //fd[1] - write
//     int fd[2];
//     if(pipe(fd) == -1){
//         printf("effor occured with opening pipe");
//         return 1;
//     }

//     int id = fork(); //should check to make sure id doesnt == -1
//     if(id==0){
//         close(fd[0]);
//         int x;
//         printf("input a num\n");
//         scanf("%d", &x);
//         write(fd[1], &x, sizeof(int)); //should check to make sure write doesnt return -1
//         close(fd[1]);
//     }else{
//         close(fd[1]);
//         int y;
//         read(fd[0], &y, sizeof(int)); //should checkk to make sure read doesnt return -1
//         close(fd[0]);
//         printf("Got from child process %d\n", y);
//     }

//     return 0;
// }





// ///FORK + WAIT
// int main(int argc, char* argv[]){
//     int id = fork();
//     if(id == 0){
//         sleep(1);
//     }
//     printf("Current ID: %d, parent ID: %d\n", getpid(), getppid());

//     int res = wait(NULL);
//     if(res == -1){
//         printf("No children to wait for\n");
//     }else{
//         printf("%d finished execution\n", res);
//     }


//     return 0;
// }


// ///FORK DEMO
// int main(int argc, char* argv[]){
//     int id = fork();
//     int n;

//     if(id == 0){
//         n=1;
//     }
//     else{
//         n=6;
//     }

//     if(id != 0){
//         wait(NULL);
//     }

//     int i;
//     for(i=n; i<n+5;i++){
//         printf("%d ", i);
//         fflush(stdout);
//     }
//     printf("\n");
//     // printf("Hello world");

//     return 0;
// }