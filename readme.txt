Taven and Dr Cheng, 
my apologies for how late this is. 

If possible please USE 2 GRACE DAYs (I'll take the 5-point penalty for the third late day). If not possible then go ahead and use all 3 grace days. I'm ok with whatever you decide.

I would be really curious to see how this was "supposed to be solved." I say this because I have a second entire version of this that I had to scrap because it relied on synchronous communication via pipes between specific children and I could not for the life of me get it to work. So instead I did this one where the parent kind of arbitrates it all. I spent waaaaay too much time on the other design because I liked it lol. Ended up restarting today because I was out of time.

The methods and namespaces all have description comments. Everything important is described for the most part. Uncommenting the couts and printf calls will also give you insight into the progression of the program (honestly I use print commands as comments to myself a lot of the time and theres hundreds of them still in there from me debugging...all commented out ofc). Im sorry a lot of my non-final variables (ie, variables that dont recieve final values to pass to the next stage of the program) have generic un-descriptive names. A lof of them are one or two time use and I didnt invest the time to trim them down to the bare essentials. I just made it work. 


RUNTIME DETAILS:

Please only use comma-deliniated txt format for the integer input file and the input_var and internal_var definitions. 

Its setup to use the argc and argv stuff. Just give it the file names when you run the program. Give the integer input file and then the pipeline input file (in that order).

I noticed the whitespace on the pipeline files are diferent from each other. So designed my parser to be able to handle both formats of whitespace (fingers crossed). 




SOURCES:

https://stackoverflow.com/questions/6877697/communicating-between-two-child-processes-with-pipes 

https://www.programiz.com/c-programming/library-function/string.h/strcpy     
https://www.digitalocean.com/community/tutorials/convert-string-to-char-array-c-plus-plus#1-the-c-_str-and-strcpy-function-in-c 
https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c 

https://favtutor.com/blogs/cpp-dictionary

https://towardsdev.com/understanding-inter-process-communication-in-linux-fork-pipes-shared-memory-and-message-queues-2503a9e334ad 

I used a ton more websites looking up super basic stuff about C (vector and array documentation, string handling, too many random things about C to even try to list -- again im a java guy so I code C like I would code Java and when that breaks down I have to go find out why). I made sure to jot down any sites where I copied code from. 