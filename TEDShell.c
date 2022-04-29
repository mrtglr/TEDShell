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

char* yourPath[100];

void initialPath(char *yourPath[])
{
    //an array filled with /bin and 99 empty strings
    yourPath[0] =(char *) malloc(strlen("/bin")+1);
    strcpy(yourPath[0], "/bin");
    int i = 0; 
    for( i = 1; i < 100; i++)
    {
        yourPath[i] =(char *) malloc(strlen(" ")+1);
        strcpy(yourPath[i], " ");     
    }       
}
void readline(char* buffer){
    fgets(buffer, 100, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

int check(char *parsed[], int lenParsed)
{
    int isTrue = 0;
    int i = 1;
    if(lenParsed == 1)
    {
        return 1;
    }
    for(i = 1; i < lenParsed; i++)
    { 
        int j = 0;
           
        for(j = 0; j < 100; j++)
        { 
            if(strcmp(yourPath[j]," ") == 0)
            {
                break;
            }
            else if(strcmp(parsed[i],yourPath[j]) == 0)
            {
                isTrue = 1;
                return isTrue;
            }
            else
            {
                continue;
            } 
        }
    }
    return isTrue;
}
int path(char* str)
{
	int usable = access(str, X_OK);
	if(usable == -1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
	return 0;
}

void update_path(char * parsed[])
{
    int i = 1;
    while(1)
    {
        if(parsed[i] == NULL)
        {
            yourPath[i] = NULL;
            break;
        }
        else
        {
            if(path(parsed[i])==1)
            {
                yourPath[i-1] =(char *) malloc(strlen(parsed[i]) + 1);
                strcpy(yourPath[i-1], parsed[i]);           
            }
        }
        i++;
    }

}

// Function to execute builtin commands
int ownCmdHandler(char** parsed, int lenParsed)
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
        exit(0);
    case 2:
    	if(parsed[1] == NULL)
    	{
    		printf("An error has occurred\n");
		}
		else
		{
            if(check(parsed, lenParsed) == 1)
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
            else
            {
                printf("not in search path\n");
            }

		}
	    return 1;
    case 3:
        if(parsed[1] == NULL)
    	{
    		printf("An error has occurred\n");
		}
        else
        {
            update_path(parsed);
        }
        return 0;
    default:
        break;
    }
  
    return 0;
}

// function for parsing command words
void parseSpace(char* str, char** parsed, int* len)
{
    int i;
    for (i = 0; i < 100; i++) {
        parsed[i] = strsep(&str, " ");
  		(*len)++;
        if (parsed[i] == NULL){
            (*len)--;
            break;
        }
        if (strlen(parsed[i]) == 0){
            (*len)--;
            i--;
        }    
    }
}
int processString(char* str, char** parsed, int* len)
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
  	int lenParsed = 0;
    parseSpace(str, parsed,len);
    lenParsed = *len;
    
    if (ownCmdHandler(parsed, lenParsed))
        return 1;

    else
        //return 1 + piped;
        return 1;
}
/*int lenghtParsed(int leng)
{
    leng = parseSpace(str, parsed,len,leng);
    return leng;
}*/

int redirect() {
    int file = open("cout.txt", O_WRONLY | O_CREAT, 0777);
    if(file == -1){
        return 2;
    }
    int file2 = dup2(file,STDOUT_FILENO);
    close(file);
    return 0;
}

// Function where the system command is executed
void execArgs(char** parsed,int* len)
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
        if((*len)-2>0 && strcmp(parsed[(*len)-2],">")==0 ){
            int file = open(strcat(parsed[(*len)-1],".txt") , O_WRONLY | O_CREAT, 0777);
            if(file == -1){
                return;
            }
            int file2 = dup2(file,STDOUT_FILENO);
            close(file);
        }

        
		if((strcmp(parsed[0], "path") == 0))
		{
            printf("path has been updated\n");
		}
		else if((strcmp(parsed[0], "cd") != 0))
		{
			if(parsed[0] == NULL)
			{
				perror("\nNo input.");
				exit(0);
			}
			if(parsed[1] != NULL)
			{
                if((*len)-2>0 && strcmp(parsed[(*len)-2],">")==0 ){
                    
                    parsed[(*len)-1]=NULL;
                    parsed[(*len)-2]=NULL;
                    
                    if(execvp(parsed[0], parsed) < 0)
                    {
                        perror("\nCould not execute command..");
                    } 
                }
                else
                {
                    if(check(parsed, *len) == 1)
                    {
                        if(execvp(parsed[0], parsed) < 0)
                        {
                            perror("\nCould not execute command..");
                        } 
                    }
                    else
                    {
                        printf("not in search path\n");
                    }

                }

				exit(0);
			}
			else
			{
                char p[100];
                getcwd(p, 100);
                char *temp[2];
                temp[0] = p;
                temp[1] = p;
                if(check(temp,*len) == 1)
                {                   
                    if(execvp(parsed[0], parsed) < 0)
                    {
                        perror("\nCould not execute command..[parsed[1] nullsa]");
                    } 
                }
                else
                {
                    perror("not in search path\n");
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
    int len=0;
    isExe = processString(input,parsedArgs,&len);
    if(isExe == 1)
    {
        execArgs(parsedArgs,&len);
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

    int i = 0; 
    for( i = 1; i < 100; i++)
    {
        yourPath[i] =(char *) malloc(strlen(" ")+1);
        strcpy(yourPath[i], " ");     
    }   
    char input[MAXINPUT + 1];
    initialPath(yourPath);
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