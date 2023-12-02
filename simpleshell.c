// Author: Sofia Gratny
// Date: 9/17/23
// Purpose: PA1

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define SIZE 2048  // larger buffer size for the input
#define TOKENS 128 // larger amount of maximum tokens allowed
#define USERNAME "sgratny"

// function prototypes
char *getHostName();
char *getUserInput(char *currentDirectory, char *hostName);
int tokenizeInput(char *input, char *arguments[]);
int executeShellCommand(char *command, char *args[]);
int changeDirectory(const char *path);
int getCurrentDirectory(char *currentDirectory, size_t size);

int main()
{
    char currentDirectory[SIZE];
    char *hostName = getHostName(); // getting the host name of the system.
    if (USERNAME == NULL || hostName == NULL)
 {
        fprintf(stderr, "Error: Unable to retrieve username or host name.\n");
        return 1;
 }
    while (1)
    {
        // getting the current working directory
        if (getCurrentDirectory(currentDirectory, sizeof(currentDirectory)) != 0)
    {
            fprintf(stderr, "Error: Unable to retrieve current working directory.\n");
            return 1;
    }
        if (currentDirectory == NULL)
    {
            fprintf(stderr, "Error: Unable to retrieve current working directory.\n");
            return 1;
    }
        // prompt the user for input and read it
        char *userInput = getUserInput(currentDirectory, hostName);
        if (userInput != NULL)
        {
            char *arguments[TOKENS];
            int argumentCount = tokenizeInput(userInput, arguments);

            if (argumentCount > 0)
            {
                if (strcmp(arguments[0], "cd") == 0)
            {
                if (argumentCount != 2)
            {
                fprintf(stderr, "Usage: cd <directory_path>\n");
            }
                    else
                     {
                        int result = changeDirectory(arguments[1]);
                        if (result != 0)
                         {
                        fprintf(stderr, "Error: Unable to change directory.\n");
                         }
                     }
                 }
            else if (strcmp(arguments[0], "exit") == 0)
                {
                    // manage exit command
                    free(userInput);
                    break;
                    // exit the loop
                }
            else
                {
                    // non-cd and non-exit commands
                    int result = executeShellCommand(arguments[0], arguments);
                    if (result != 0)
                        {
                    fprintf(stderr, "Error: Unable to execute the command.\n");
             }
             }
        }
    free(userInput);
    }
    }
    free(hostName);
return 0;
}
char *getHostName()
{
size_t bufferSize = 64;
char *hostName = (char *)malloc(bufferSize);
    if (hostName != NULL)
    {
    if (gethostname(hostName, bufferSize) != 0)
    {
        perror("gethostname");
        free(hostName);
        return NULL;
    }
    }
return hostName;
}
char *getUserInput(char *currentDirectory, char *hostName)
{
    printf("%s@%s:%s$ ", USERNAME, hostName, currentDirectory);
    char userInput[SIZE];
    if (fgets(userInput, sizeof(userInput), stdin) == NULL)
    {
    perror("fgets");
    return NULL;
    }
    size_t userInputLength = strlen(userInput);
    if (userInputLength > 0 && userInput[userInputLength - 1] == '\n')
    {
    userInput[userInputLength - 1] = '\0';
    }
    return strdup(userInput);
}
int tokenizeInput(char *input, char *arguments[])
{
    int argumentCount = 0;
    if (input == NULL)
    {
    return -1;
    }
    // strtok splits the input into arguments based on whitespace
    char *argument = strtok(input, " \t\n");

    while (argument != NULL)
    {
        if (argumentCount >= TOKENS)
        {
            fprintf(stderr, "Error: Too many arguments (maximum %d allowed).\n", TOKENS - 1);
        return -1;
        }
        arguments[argumentCount++] = strdup(argument);
        argument = strtok(NULL, " \t\n");
    }
    if (argumentCount == 0)
    {
        fprintf(stderr, "Error: No command provided.\n");
    return -1;
    }
    // setting last element to NULL to indicate the end
    arguments[argumentCount] = NULL;
    return argumentCount;
}
int executeShellCommand(char *command, char *args[])
{
    pid_t pid = fork();
    if (pid == -1)
    {
     perror("fork failed");
    return -1;
    }
    else if (pid == 0)
    {
        // child process
        if (execvp(command, args) == -1)
        {
        fprintf(stderr, "Error: Unable to execute the command '%s'.\n", command);
        _exit(1); // rid the child process after exec failure
        }
    }
    else
    {
        // parent process
        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
        perror("wait failed");
        return -1;
        }
        if (WIFEXITED(status))
        {
        // child process rid normally
        return WEXITSTATUS(status); // exit status of the child process
        }
        else
        {
        // child process rid abnormally
        fprintf(stderr, "Error: Child Process Terminated Abnormally.\n");
        return -1;
    }
    }
}
 
int changeDirectory(const char *path)
{
    if (chdir(path) == -1)
    {
        perror("chdir failed");
        return -1;
    }
    if (path == NULL || strlen(path) == 0)
    {
        fprintf(stderr, "Error: Invalid directory path.\n");
        return -1;
    }
    return 0;
}

int getCurrentDirectory(char *currentDirectory, size_t size)
{
    if (getcwd(currentDirectory, size) == NULL)
    {
        perror("getcwd");
    return -1;
    }
    return 0;
}
