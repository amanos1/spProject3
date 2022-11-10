#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

/*************************************************************/
/* This program is a crude implementation of a linnux shell. */
/* It can do many of the basic things a shell can.           */
/*                                                           */
/* Date: Nov 18, 2022                                        */
/*************************************************************/

void runLine(char *line);

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

		if(end == 1) break;

		werd[len] = '\0';

		if(fork() == 0) {
			runLine(werd);
			end = 1;
			break;
		} else {
			wait(NULL);
		}
	}

	free(werd);

	return 0;
}

/********************************************/
/* Processes the  command the user inputted */
/********************************************/
void runLine(char *line) {
	char spaceBar[2] = " ";
	char *each;

	each = strtok(line, spaceBar);

	if(access(each, F_OK) != 0) {
		char *newShit = malloc(10 + strlen(each));
		strcpy(newShit, "/bin/");
		strcat(newShit, each);
		if(access(newShit, F_OK) != 0) {
			strcpy(newShit, "/usr/bin/");
			strcat(newShit, each);
			if(access(newShit, F_OK) != 0) {
				printf("%s: No such file or directory\n", each);
				free(newShit);
				return;
			}
		}
		free(newShit);
	}

	size_t max = 2;
	int i;
	char **args = malloc(sizeof(char *) * max);
	//char *env[] = { (char *)0 };

	for(i = 0; each != NULL; i++) {
		if(i + 1 >= max) {
			max *= 2;
			args = realloc(args, sizeof(char *) * max);
		}

		args[i] = each;

		each = strtok(NULL, spaceBar);
	}

	args[i] = NULL;
	execvp(args[0], args);

	free(args);
}
