/*
In this example, two children processes are created.
The first one executes the command "ls -l". Its output
is redirected to a pipe. The second one executes
"grep pipe". Its input is redirected from the
pipe the first process' output is written to. So,
the overall command lists all the *pipe* files.
The father process waits for the children to finish,
before continuing execution. This is an example of
how the "|" operator should work.

Compilation: gcc pipe-redirection.c -o pipe-redirection -Wall
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

pid_t run_cmd(int fd[2], int fddup, char *arg1, char *arg2)
{
	char *argv[5];
	pid_t pid1 = fork();
	if (pid1 < 0)
	{
		perror("First fork() failed!");
		return -1;
	}
	if (pid1 == 0)
	{
		/* Set the process output to the input of the pipe. */
		if (fddup == 0 || fddup == 1)
		{
			close(fddup);
			dup(fd[fddup]);
		}
		else if (fddup == 2)
		{
			close(0);
			dup(fd[0]);
			close(1);
			dup(fd[1]);
		}
		close(fd[0]);
		close(fd[1]);
		argv[0] = (char *)malloc(5 * sizeof(char));
		argv[1] = (char *)malloc(5 * sizeof(char));
		strcpy(argv[0], arg1);
		strcpy(argv[1], arg2);
		argv[2] = NULL;
		// fprintf(stderr, "************* Running %s %s \n", arg1, arg2);
		execvp(argv[0], argv);
		perror("execvp() failed");
		return -1;
	}
	return pid1;
}

int main()
{
	int fd[2];
	pid_t pid1, pid2, pid3;
	char *argv[5];

	pipe(fd);

	pid1 = run_cmd(fd, 1, "ls", "-la");

	pid2 = run_cmd(fd, 0, "grep", "drw");

	
	close(fd[0]);
	close(fd[1]);
	/* Wait for the children to finish, then exit. */
	if (pid1 > 0)
		waitpid(pid1, NULL, 0);
	if (pid2 > 0)
		waitpid(pid2, NULL, 0);
	// if (pid3 > 0)
	// 	waitpid(pid3, NULL, 0);
	// printf("************* Father exitting... *************\n");
	return 0;
}