/*
 * Ruiwen Liang
 * rliang4
 * cs111-asgn1
 * argshell.c
 * A unix shell program
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h> 

extern char ** get_args();
pid_t pid, pid1, wpid;
char init_dir[256];
char *dist_dir;
int i, status;
int file_in, file_out;
int file_pipe[2];

int child_proc(char **args) {
	if(args[0] == NULL) {
		exit(0);
	} else {
		for (i = 0; args[i] != NULL; i++) {
			if(*args[i] == '<') {
				// Open input file, print error message
				file_in = open(args[i+1], O_RDONLY);
				if(file_in == -1) {
					perror("Cannot open input file\n");
					exit(1);
				}
				// Redirect input file
				if(dup2(file_in, 0) == -1) {
					perror("Cannot redirect input file\n");
					exit(1);
				}
				args[i] = args[i++] = NULL;
				i++;
			} else if(*args[i] == '>') {
				// Open or create output file, print error message
				file_out = open(args[i+1], O_WRONLY | O_TRUNC | O_CREAT, 0644);
				if(file_out == -1) {
					perror("Cannot open or create output file\n");
					exit(1);
				}
				// Redirect output file
				if(dup2(file_out, 1) == -1) {
					perror("Cannot redirect output file\n");
					exit(1);
				}
				args[i] = args[i++] = NULL;
				i++;
			} else if(args[i] == ">>") {
				file_out = open(args[i+1], O_WRONLY | O_APPEND | O_CREAT, 0644);
				if(file_out == -1) {
					perror("Cannot open or create output file\n");
					exit(1);
				}
				if(dup2(file_out, 1) == -1) {
					perror("Cannot redirect output file\n");
					exit(1);
				}
				args[i] = args[i++] = NULL;
				i++;
			} else if(*args[i] == '|') {
				if(pipe(file_pipe) == -1) {
					perror("Pipe failed\n");
					exit(1);
				}
				pid1 = fork();
				if(pid1 == -1) {
					perror("Cannot duplicate process\n");
					exit(1);
				} else if(pid1 == 0) {
					dup2(file_pipe[1], 1);
					close(file_pipe[0]);
				} else {
					dup2(file_pipe[0], 0);
					close(file_pipe[1]);
					while((pid = wait(&status)) != -1);
				}
				args[i++] = NULL;
				i++;
			} else if(args[i] == ">&" || args[i] == ">>&") {
				// Open or create output file, print error message
				file_out = open(args[i+1], O_WRONLY | O_CREAT, 0644);
				if(file_out == -1) {
					perror("Cannot open or create output file\n");
					exit(1);
				}
				// Redirect output file
				if(dup2(file_out, 1) == -1) {
					perror("Cannot redirect output file\n");
					exit(1);
				}
				// Redirect standard error
				if(dup2(file_out, 2) == -1) {
					perror("Cannot redirect standard error\n");
					exit(1);
				}
				args[i] = args[i++] = NULL;
				i++;
			} else if(args[i] == "|&") {
				if(pipe(file_pipe) == -1) {
					perror("Pipe failed\n");
					exit(1);
				}
				pid1 = fork();
				if(pid1 == -1) {
					perror("Cannot duplicate process\n");
					exit(1);
				} else if(pid1 == 0) {
					dup2(file_pipe[1], 1);
					dup2(file_pipe[1], 2);
					close(file_pipe[0]);
				} else {
					dup2(file_pipe[0], 0);
					dup2(file_pipe[0], 2);
					close(file_pipe[1]);
					while((pid = wait(&status)) != -1);
				}
				args[i++] = NULL; 
				i++;
			} else if(*args[i] == ';') {
				int returnStatus = 0;
				char **c_args;
				for(int k = 0; k < i; k++) {
					c_args[k] = args[k];
				}
				char **temp;
				temp[0] = args[i++];
				pid1 = fork();
				*args = NULL;
				if(pid1 == -1) {
					perror("Cannot duplicate process\n");
					exit(1);
				} else if(pid1 == 0) {
					child_proc(c_args);
				} else {
					while((wpid = wait(&returnStatus)) > 0);
					child_proc(temp);
				}
			} else if(!strcmp(args[i], "cd")) {
				if(args[i+1] == NULL || args[i+1] == ";") {
					if(chdir(init_dir) == -1) {
						perror("Cannot set working directory\n");
						exit(1);
					}
					args[i] = NULL;
				} else {
					if(chdir(args[i+1]) == -1) {
						perror("Cannot set working directory\n");
						exit(1);
					}
					args[i] = args[i++] = NULL;
				}
			} else if(!strcmp(args[i], "exit")) {
				exit(0);
			}
		}
	}
	if(execvp(*args, args) == -1) {
		perror(args[0]);
		exit(1);
	} else if(*args == NULL);
	return 0;
}

int main() {
    char **args;
	if(getcwd(init_dir, sizeof(init_dir)) == NULL) {
			perror("Cannot get current working directory\n");
		}

    while (1) {
		printf ("Command ('exit' to quit): ");
		args = get_args();
		pid = fork();
		if(pid == -1) {
			perror("Cannot duplicate process\n");
			exit(1);
		} else if(pid == 0) {
			child_proc(args);
		} else {
			while(pid = wait(&status) != -1);
			if(args[0] == NULL) {
				exit(0);
			} else if(!strcmp(args[0], "exit")) {
				write(1, "Exiting...\n", strlen("Exiting...\n"));
				exit(0);
			} else if(!strcmp(args[0], "cd")) {
				if(args[1] == NULL || args[1] == ";") {
					if(chdir(init_dir) == -1) {
						perror("Cannot set working directory\n");
						exit(1);
					}
				} else {
					if(chdir(args[1]) == -1) {
						perror("Cannot set working directory\n");
						exit(1);
					}
				}
			}
		}
    }
}
