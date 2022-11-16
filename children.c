#include <stdio.h>
#include <string.h>

typedef struct Child {
	int pid;
	int status; //corresponds with the "statuses" array
	int bg;
	int alive; //0 if the child has been terminated, else 1
	char *command;
}

char **statuses[2] = {"Stopped", "Running"};

int childCount; // amount of children 
int maxChildren;
Child *childList[];

/****************************************/
/* Adds process to the list of children */
/****************************************/
int addChild(int id, char **input, int bg) {
	//initalize the array if it has not been already
	if(maxChildren == 0) {
		maxChildren = 2;
		childList = malloc(sizeof(Child*) * maxChildren);
	}

	int stringLen = 0;
	int strAmt;
	for(strAmt = 0; input[strAmt] != NULL; strAmt++) {
		stringLen += strlen(input[strAmt]);
	}

	Child *c = malloc(sizeof(Child));
	c->command = malloc(sizeof(stringLen + strAmt + 2));
	c->pid = id;
	c->status = 1;
	c->bg = bg;

	for(int i = 0; i < strAmt; i++) {
		strcat(c->command, input[i]);
		if(i < strAmt - 1) strcat(c->command, " ");
	}

	if(childCount >= maxChildren - 1) {
		maxChildren *= 2;
		Child **newChildren = realloc(childList, sizeof(Child*) * maxChildren);
		childList = newChildren;
	}

	childList[childCount++] = c;
	return childCount;
}

int getPid(int jid) {
	if(jid < childCount)
		return childList[jid]->pid;
	return 0;
}
/**********************************************************/
/* Return command NEEDS A LOT OF WORK*/ 
/**********************************************************/

int findCommand(int jid){
	for(int i = 0; i < childCount; i++){
		if(childList[i]->pid == jid){
			return childList[i]->pid;
		}
	}
	return 1;
	
}

/**********************************************************/
/* Update background tag NEEDS A LOT OF WORK*/
/**********************************************************/
void updateBackground(int jid){
	for(int i = 0; i < childCount; i++){
		if(childList[i]->pid == jid){
			childList[i]->bg = 1;
			break;
		}
	}
	return 0;
	
}

/**********************************************************/
/* Prints the alive children when the jobs command is run */
/**********************************************************/
void printChildren() {
	for(int i = 0; i < childCount; i++) {
		Child *c = childList[i];
		if(c->alive == 0) continue;

		printf("[%i] %i %s %s\n", i, c->pid, statuses[c->status], c->command);
	}
}

/*************************************************************************************/
/* Frees all neccesary data so there are no memory leaks when the program terminates */
/*************************************************************************************/
void freeThemAll() {
	for(int i = 0; i < childCount; i++) {
		free(childList[i]->command);
		free(childList[i]);
	}
	free(childList);
}

/*************************************************/
/* Resets the list after all child processes die */
/*************************************************/
void emptyOut() {
	for(int i = 0; i < childCount; i++) {
		char *command = childList[i]->command;
		childList[i]->command = NULL;
		Child c = childList[i];
		childList[i] = NULL;
		free(command);
		free(c);
	}
	childCount = 0;
}

/*******************************************/
/* Removes process to the list of children */
/*******************************************/
void unaliveChild(int pid) {
	int allDead = 1;
	for(int i = 0; i < childCount; i++) {
		if(pid == childList[i]->pid) {
			childList[i]->alive = 0;
			break;
		} else if(childList[i]->alive == 1) {
			allDead = 0;
		}
	}

	if(allDead == 1) emptyOut();
}

