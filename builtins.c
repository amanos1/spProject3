#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* ListOfCommands[6];

ListOfCommands[0] = "bg";
ListOfCommands[1] = "cd";
ListOfCommands[2] = "exit";
ListOfCommands[3] = "fg";
ListOfCommands[4] = "jobs";
ListOfCommands[5] = "kill";


//Run a suspended job in background
int background(char* jobId){
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
int changeDir(char* path){
	//Given absolute path
	if(path[0] == '/'){
		if(chdir(path) == -1){
			printf("%s : %s\n", path, sterror(errno));
		}
		setenv("PWD", path, 1);
	}
	//Given relative path
	else if(path[0] == '.'){
		if(chdir(path) == -1){
			printf("%s : %s\n", path, sterror(errno));
		}
		setenv("PWD", path, 1);
	}
	//Given no path
	else if(path == NULL){
		if(chdir(getenv("HOME")) == -1){
			printf("%s : %s\n", path, sterror(errno));
		}
		setenv("PWD", getenv("HOME"));
	}
	return 0;
}	

//Exit shell
int exit(){
	
	exit(0);
}	

//Run a suspended or background job in the foreground
int foreground(pid_t pid){
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
	

}


//Send SIGTERM to the given job
int kill(pid_t pid){
	printf("Killing process %d", pid);
	kill(pid[i], SIGTERM);

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
		exit();
		return 1;
	} if(strcmp(ListOfCommands[3], cmd[0]) == 0){
		foreground(cmd);
		return 1;
	} if(strcmp(ListOfCommands[4], cmd[0]) == 0){
		jobs();
		return 1;
	} if(strcmp(ListOfCommands[5], cmd[0]) == 0){
		kill(cmd);
		return 1;
	}
	return 0;
}
