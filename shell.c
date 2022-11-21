#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

/************************************************************/
/* This program is a crude implementation of a linux shell. */
/* It can do many of the basic things a shell can.          */
/*                                                          */
/* Date: Nov 18, 2022                                       */
/************************************************************/

/*
When the passed line is not given with a relative or absolute path, 
just check for built-in then in usr/bin and /bin and then say command 
not found, current is routing to looking for file and printing no file;
*/

void runLine(char **args);
char **parseLine(char *line, int *and);
int getPid(int jid);
int lookup(char** cmd);
void freeThemAll();
void unaliveChild(int pid);
int addChild(int id, char **input, int bg);
void checkOnKids();
void suspend(int sig);
void terminate(int sig);
void childStopped(int pid);
void childKilled(int pid, int sig);
void werj();

int parent = 1;
int favoriteChild;



int main() {
	char *werd = malloc(2);

	if (!werd) {
		printf("Cannot allocate string.\n");
		return 1;
	}

	int c;
	char *newWerd;
	int end = 0;
	size_t buf = 2;

	signal(SIGTSTP, suspend);
	signal(SIGINT, terminate);

	//read strings until we reach an EOF
	while(1) {
		size_t len = 0;

		printf("> ");

		//reads the string one character at a time
		while (1) {
			if((c = getchar()) == EOF) {
				end = 1;
				break;
			} else if(c == '\n') break;

			//resize the string if it exceeds the current maximum length
			if (len + 1 == buf) {
				buf *= 2;
				newWerd = realloc(werd, buf);

				if (!newWerd) {
					printf("Cannot reallocate string.\n");
					free(werd);
					return 1;
				}

				werd = newWerd;
			}

			werd[len++] = c;
		}


		werd[len] = '\0';

		checkOnKids();

		int ampersand = 0;
		char **args = parseLine(werd, &ampersand);

		if(lookup(args) == 1) continue;
		
		if(end == 1 || strcmp(args[0], "exit") == 0) {
			freeThemAll();
			free(args);
			break;
		}		
		
		int myChild = fork();
		if(myChild == 0) {
			//werj();
			runLine(args);
			free(args);
			break;
		} else {
			parent = 1;
			addChild(myChild, args, ampersand);
			if(ampersand == 0) {
				printf("waaaa\n");
				favoriteChild = myChild;
				int status;
				waitpid(myChild, &status, 0);
				unaliveChild(myChild);
				favoriteChild = 0;
			} else {
				//signal(SIGCHLD, )
			}
		}

		free(args);
	}

	free(werd);

	return 0;
}

char **parseLine(char *line, int *and) {
	char spaceBar[2] = " ";
	char *each;

	each = strtok(line, spaceBar);

	size_t max = 2;
	int i;
	char **args = malloc(sizeof(char *) * max);

	for(i = 0; each != NULL; i++) {
		if(i + 1 >= max) {
			max *= 2;
			args = realloc(args, sizeof(char *) * max);
		}

		args[i] = each;

		each = strtok(NULL, spaceBar);
	}

	if(strcmp(args[i-1], "&") == 0) {
		*and = 1;
		args[i-1] = NULL;
		return args;
	}

	if (args[i-1][strlen(args[i-1])-1] == '&') {
		*and = 1;
		args[i-1][strlen(args[i-1])-1] = '\0';
	}

	args[i] = NULL;
	return args;
}

/********************************************/
/* Processes the  command the user inputted */
/********************************************/
void runLine(char **args) {
	if(access(args[0], F_OK) != 0) {
		char *newShit = malloc(10 + strlen(args[0]));
		strcpy(newShit, "/bin/");
		strcat(newShit, args[0]);
		if(access(newShit, F_OK) != 0) {
			strcpy(newShit, "/usr/bin/");
			strcat(newShit, args[0]);
			if(access(newShit, F_OK) != 0) {
				//Args has some extra stuff that prints that I can remove
				printf("%s: No such file or directory\n", args[0]);
				free(newShit);
				return;
			}
		}
		free(newShit);
	}

	execvp(args[0], args);
}

void suspend(int sig) {
	if(favoriteChild > 0) {
		kill(favoriteChild, SIGSTOP);
		childStopped(favoriteChild);
	}
}

void terminate(int sig) {
	if(parent == 0) {
		exit(0);
	}
	if(favoriteChild > 0) {
		kill(favoriteChild, SIGINT);
		childKilled(favoriteChild, sig);
	}
}
