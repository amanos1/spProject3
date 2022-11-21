#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct Child {
	int pid;
	int status; //corresponds with the "statuses" array
	int bg;
	int alive; //0 if the child has been terminated, else 1
	char *command;
} Child;

char *statuses[] = {"Stopped", "Running"};

int childCount; // amount of children 
int maxChildren;
Child **childList;

/**********************************************************/
/* Prints the alive children when the jobs command is run */
/**********************************************************/
void printChildren() {
	for(int i = 0; i < childCount; i++) {
		Child *c = childList[i];
		if(c->alive == 0) continue;

		if(c->bg == 0)
			printf("[%i] %i %s %s\n", i, c->pid, statuses[c->status], c->command);
		else
			printf("[%i] %i %s %s &\n", i, c->pid, statuses[c->status], c->command);
	}
}

/****************************************/
/* Adds process to the list of children */
/****************************************/
void addChild(int id, char **input, int bg) {
	//initalize the array if it has not been already
	if(maxChildren == 0) {
		maxChildren = 2;
		childList = malloc(sizeof(Child *) * maxChildren);
	}

	int stringLen = 0;
	int strAmt;
	for(strAmt = 0; input[strAmt] != NULL; strAmt++) {
		stringLen += strlen(input[strAmt]);
	}

	Child *c = malloc(sizeof(Child));
	c->command = malloc(sizeof(char) * (stringLen + strAmt + 1));
	c->pid = id;
	c->status = 1;
	c->bg = bg;

	for(int i = 0; i < strAmt; i++) {
		if(i == 0) { 
			strcpy(c->command, input[i]);
			if(i < strAmt - 1) strcat(c->command, " ");
			continue;
		}
		strcat(c->command, input[i]);
		if(i < strAmt - 1) strcat(c->command, " ");
	}

	if(childCount >= maxChildren - 1) {
		maxChildren *= 2;
		Child **newChildren = realloc(childList, sizeof(Child*) * maxChildren);
		childList = newChildren;
	}

	childList[childCount++] = c;
	printf("[%i] %i\n", childCount-1, id);
}

int getPid(int jid) {
	if(jid < childCount)
		return childList[jid]->pid;
	return 0;
}

/**********************************************************/
/* Find all running processes*/ 
/**********************************************************/
int sighupIt(){
	for(int i = 0; i < childCount; i++){
		if(childList[i]->status == 1){
			kill(childList[i]->pid, SIGHUP);
		}
	}
	return 0;
}

/**********************************************************/
/* Find all stopped processes*/ 
/**********************************************************/
int sigcontIt(){
	for(int i = 0; i < childCount; i++){
		if(childList[i]->status == 0){
			kill(childList[i]->pid, SIGHUP);
			kill(childList[i]->pid, SIGCONT);
		}
	}
	return 0;
}

/*************************************************************/
/* If the process indicated by pid exists in the child list, */
/* the function will return the same pid                     */
/*                                                           */
/* Elsewise, it will return 0                                */ 
/*************************************************************/
int findCommand(int pid){
	for(int i = 0; i < childCount; i++){
		if(childList[i]->pid == pid){
			return childList[i]->pid;
		}
	}
	return 0;
}

/**********************************************************/
/* Update background tag NEEDS A LOT OF WORK*/
/**********************************************************/
int updateBackground(int jid){
	for(int i = 0; i < childCount; i++){
		if(childList[i]->pid == jid){
			childList[i]->bg = 1;
			break;
		}
	}
	return 0;
	
}

/*************************************************************************************/
/* Frees all neccesary data so there are no memory leaks when the program terminates */
/*************************************************************************************/
void freeThemAll() {
	for(int i = 0; i < childCount; i++) {
		if(childList[i]->status == 1) {
			kill(childList[i]->pid, SIGTERM);
		}

		free(childList[i]->command);
		free(childList[i]);
	}
	free(childList);
}

/*************************************************/
/* Resets the list after all child processes die */
/*************************************************/
void emptyOut(int newStart) {
	for(int i = childCount-1; i >= newStart; i--) {
		char *command = childList[i]->command;
		childList[i]->command = NULL;
		Child *c = childList[i];
		childList[i] = NULL;
		free(command);
		free(c);
	}
	childCount = newStart;
}

/*******************************************/
/* Removes process to the list of children */
/*******************************************/
int unaliveChild(int pid) {
	int pidpos = -1;
	int clearStart = 0;
	for(int i = 0; i < childCount; i++) {
		if(pid == childList[i]->pid) {
			childList[i]->alive = 0;
			pidpos = i;
			break;
		} else if(childList[i]->alive == 1) {
			clearStart = i + 1;
		}
	}

	if(clearStart != childCount) emptyOut(clearStart);
	return pidpos;
}

void childKilled(int pid, int sig) {
	int jobId = unaliveChild(pid);
	printf("[%i] %i terminated by signal %i", jobId, pid, sig);
}

void childStopped(int pid) {
	for(int i = 0; i < childCount; i++) {
		if(pid == childList[i]->pid) {
			childList[i]->status = 0;
			return;
		}
	}
}

void childContinues(int pid, int bg) {
	for(int i = 0; i < childCount; i++) {
		if(pid == childList[i]->pid) {
			childList[i]->status = 1;
			childList[i]->bg = bg;
			return;
		}
	}
}

/********************************************/
/* Checks all the children in the list      */
/* to make sure none of them hav terminated */
/********************************************/
void checkOnKids() {
	for(int i = 0; i < childCount; i++) {
		if(childList[i]->status == 1) {
			int status = waitpid(childList[i]->pid, NULL, WNOHANG);
			if(status > 0)
				unaliveChild(childList[i]->pid);
		}
	}
}

