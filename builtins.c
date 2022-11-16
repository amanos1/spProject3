#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>

void printChildren();

char *ListOfCommands[6] = {"bg", "cd", "exit", "fg", "jobs", "kill"};

//Run a suspended job in background
int background(char **argv){
	//argv[0] should be the program name
	//argv should be the array of arguments for the program
	//jobId will be used to look up information above
	pid_t child_process = fork();
	if(child_process < 0){
		printf("Cannot fork\n");
		return 1;
	}
	else if(child_process == 0){
		if(execvp(argv[0], argv) < 0){
			printf("Cannot run program\n");
			return 1;
		}
	}
	return 0;
}

//Change directory
int changeDir(char **argv){
	char *path = argv[1];
	//Given absolute path
	if(path[0] == '/'){
		if(chdir(path) == -1){
			printf("%s : %s\n", path, strerror(errno));
		}
		setenv("PWD", path, 1);
	}
	//Given relative path
	else if(path[0] == '.'){
		if(chdir(path) == -1){
			printf("%s : %s\n", path, strerror(errno));
		}
		setenv("PWD", path, 1);
	}
	//Given no path
	else if(path == NULL){
		if(chdir(getenv("HOME")) == -1){
			printf("%s : %s\n", path, strerror(errno));
		}
		setenv("PWD", getenv("HOME"), 1);
	}
	return 0;
}	

//Exit shell
int leave(){
	
	exit(0);
}	

//Run a suspended or background job in the foreground
int foreground(char **argv){
	int status;
	//argv[0] should be the program name
	//argv should be the array of arguments for the program
	//jobId will be used to look up information above
	pid_t child_process = fork();
	if(child_process < 0){
		printf("Cannot fork\n");
		return 1;
	}
	else if(child_process == 0){
		if(execvp(argv[0], argv) < 0){
			printf("Cannot run program\n");
		}
		return 1;
	}
	else{
		while(wait(&status) != child_process){
			;
		}
	}
	return 0;

}

//List jobs with job id, process id, current status, and command
int jobs(){
	printChildren();
	return 0;
}


//Send SIGTERM to the given job
int murder(char **argv){
	//Might need to change status in childList?
	int jobId;
	sscanf(argv[1], "%i", &jobId);
	printf("Killing process %d", jobId);
	kill(jobId, SIGTERM);

	return 0;
}

//Find a builtin
int lookup(char** cmd){
	if(strcmp(ListOfCommands[0], cmd[0]) == 0){
		background(cmd);
		return 1;
	} if(strcmp(ListOfCommands[1], cmd[0]) == 0){
		changeDir(cmd);
		return 1;
	} if(strcmp(ListOfCommands[2], cmd[0]) == 0){
		leave();
		return 1;
	} if(strcmp(ListOfCommands[3], cmd[0]) == 0){
		foreground(cmd);
		return 1;
	} if(strcmp(ListOfCommands[4], cmd[0]) == 0){
		jobs();
		return 1;
	} if(strcmp(ListOfCommands[5], cmd[0]) == 0){
		murder(cmd);
		return 1;
	}
	return 0;
}

