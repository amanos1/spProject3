#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* ListOfCommands[4];

ListOfCommands[0] = "bg";
ListOfCommands[1] = "cd";
ListOfCommands[2] = "exit";
ListOfCommands[3] = "fg";
ListOfCommands[4] = "jobs";
ListOfCommands[5] = "kill";


//Find a builtin
int lookup(char* cmd){
	for(int i = 0; i < 6; i++){
		if(strcmp(ListOfCommands[i], cmd) == 0){
			//Call function ?? idk how we can pass just the first part
			//and then the additional
		}
	}
	return 0;
}



//Run a suspended job in background
int background(char* jobId){



}
//Change directory
int changeDir(char* path){
	//Given absolute path
	if(path[0] == '/'){
		if(chdir(path) == -1){
			printf("%s : %s\n", path, sterror(errno);
		}
		setenv("PWD", path, 1);
	}
	//Given relative path
	else if(path[0] == '.'){
		if(chdir(path) == -1){
			printf("%s : %s\n", path, sterror(errno);
		}
		setenv("PWD", path, 1);
	}
	//Given no path
	else if(path == NULL){
		if(chdir(getenv("HOME")) == -1){
			printf("%s : %s\n", path, sterror(errno);
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
