#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXINPUT 512
  
// This is a bare-bone TEDShell
// You need to write your code to work as described in the document.

// Use following command to compile:
// g++ TEDShell.c -o TEDShell

// To run the program
// ./TEDShell

// To run testcases
// ./TEDShell-tester


void readline(char* buffer){
    fgets(buffer, 100, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

// Function to execute builtin commands
int ownCmdHandler(char** parsed)
{
    int numberOfCmd = 3, i, here = 0;
    char* Commands[numberOfCmd];
    char* username;

    Commands[0] = "exit";
    Commands[1] = "cd";
    Commands[2] = "path";
	char s[100];
  
    for (i = 0; i < numberOfCmd; i++) {
        if (strcmp(parsed[0], Commands[i]) == 0) {
            here = i + 1;
            break;
        }
    }

    switch (here) {
    case 1:
        printf("\nGoodbye\n");
        printf("case 1 %d\n", here);
        exit(0);
    case 2:
    	if(parsed[1] == NULL)
    	{
    		printf("error\n");
		}
		else
		{
			// printing current working directory
	    	printf("before %s\n", getcwd(s, 100));
	    	// using the command
	    	int returned = chdir(parsed[1]);
	    	// printing current working directory
	    	printf("after %s\n", getcwd(s, 100));
	    	if(returned < 0)
	    	{
	    		perror("chdir() failed");
			}
		}
	    return 1;

    default:
		//printf("default %s\n", parsed[0]);
        break;
    }
  
    return 0;
}

// function for parsing command words
void parseSpace(char* str, char** parsed)
{
    int i;
  
    for (i = 0; i < 100; i++) {
        parsed[i] = strsep(&str, " ");
  		
        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }

}
int processString(char* str, char** parsed)
{

    //char* strpiped[2];
    //int piped = 0;
  
    //piped = parsePipe(str, strpiped);
  
    /*if (piped) {
        parseSpace(strpiped[0], parsed);
        parseSpace(strpiped[1], parsedpipe);
  
    } 
	else {
  
        parseSpace(str, parsed);
    }*/
  	parseSpace(str, parsed);
    if (ownCmdHandler(parsed))
        return 0;
    else
        //return 1 + piped;
        return 1;
}

int path(char* str)
{
	int usable = access(str, X_OK);
	if(usable == -1)
	{
		printf("error in path\n");
		return 0;
	}
	else
	{
		
		printf("true\n");
		return 1;
	}
	return 0;
}

// Function where the system command is executed
void execArgs(char** parsed)
{
    // Forking a child
    pid_t pid = fork(); 
  	
	int status;
    char *args[2];
	 
	  
    if (pid == -1) {
        printf("\nFailed forking child..");
        return;
    } 
	else if (pid == 0) 
	{

        int file = open("cout.txt", O_WRONLY | O_CREAT, 0777);
        if(file == -1){
            return 2;
        }
        int file2 = dup2(file,STDOUT_FILENO);
        close(file);

		if(strcmp(parsed[0], "path") == 0)
		{
			int isPath = path(parsed[1]);
			if (isPath == 0) 
			{
	            printf("\nCould not execute command..");
	        }
	        else
	        {
	        	args[0] = parsed[1]; 
    			args[1] = NULL;  
    			if(execvp(args[0], args) < 0)
    			{
    				perror("\nCould not execute command..");
				}
                else
                {
        	        execv(args[0], args);
		        }
	        	exit(0);
			}
		}
		else
		{
			if(parsed[0] == NULL)
			{
				perror("\nNo input.");
				exit(0);
			}
			if(parsed[1] != NULL)
			{
                /*
                printf("0:%s\n",parsed[0]);
                printf("1:%s\n",parsed[1]);
                printf("2:%s\n",parsed[2]);
                */

                /*
                int file = open("cout.txt", O_WRONLY | O_CREAT, 0777);
                if(file == -1){
                    return 2;
                }
                int file2 = dup2(file,STDOUT_FILENO);
                close(file);
                */

                
				args[0] = "/bin/ls"; 
	    		args[1] = NULL;
		        if(execvp(parsed[0], parsed) < 0)
	    		{
	    			perror("\nCould not execute command..");
				} 
                else
                {
        	        execv(args[0], args);
		        }
                
				exit(0);
			}
			else
			{

                printf("a0:%s\n",parsed[0]);
                printf("1:%s\n",parsed[1]);
                printf("2:%s\n",parsed[2]);

				args[0] = "/bin/ls"; 
	    		args[1] = NULL;  
		        if(execvp(parsed[0], parsed) < 0)
	    		{
	    			perror("\nCould not execute command..");
				} 
                else
                {
                    
        	        execv(args[0], args);
		        }
				exit(0);
			}			
		}

        exit(0);
    } 
	else 
	{
        // waiting for child to terminate 
		wait(&status);
        //wait(NULL); 
        return;
    }
}

void executeCmds(char input[], char *parsedArgs[])
{
    int isExe = 0;
    isExe = processString(input,parsedArgs);
    if(isExe == 1)
    {
        execArgs(parsedArgs);
    }
}

int batch(char* fileName) //cd /mnt/c/users/hmert/desktop/unix
{
    char line[MAXINPUT + 1];
    char *parsedArgs[MAXINPUT + 1];
    FILE *file;
    file = fopen(fileName, "r"); 

    if(file) {
        fread(line, (MAXINPUT + 1), 1, file);
        char* pch = NULL;
        pch = strtok(line, "\r\n");
        while (pch != NULL)
        {
            executeCmds(pch,parsedArgs);
            pch = strtok(NULL, "\r\n");
        }
        fclose(file);
    }
    else {
        perror("fopen");
        fclose(file);
    }
    return 1;
}
  
int main(int argc, char *argv[])
{
    char *parsedArgs[100];
    char input[MAXINPUT + 1];

    if(argc >1){
        batch(argv[1]);
    }
    else {

        printf("TEDshell> ");
        readline(input);

        while (strcmp(input, "exit") != 0)
        {

            executeCmds(input,parsedArgs);

            printf("TEDShell> ");
            readline(input);
        }
    }
    return 0;
}