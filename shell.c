#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void runLine(char *line) {
	char spaceBar[2] = " ";
	char *each;

	each = strtok(line, spaceBar);

	if(access(each, F_OK) != 0) {
		printf("%s: No such file or directory\n", each);
		return;
	}

	size_t max = 2;
	int i;
	char **args = malloc(sizeof(char *) * max);
	char *env[] = { (char *)0 };

	for(i = 0; each != NULL; i++) {
		if(i + 1 >= max) {
			max *= 2;
			args = realloc(args, sizeof(char *) * max);
		}

		args[i] = each;

		each = strtok(NULL, spaceBar);
	}

	args[i] = NULL;
	execve(args[0], args, env);

	free(args);
}

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

	while(1) {
		size_t len = 0;

		printf("> ");

		while (1) {
			if((c = getchar()) == EOF) {
				end = 1;
				break;
			} else if(c == '\n') break;

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

		runLine(werd);
	}

	free(werd);

	return 0;
}
