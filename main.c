/**
 * @file main.c
 *
 * @brief YASH shell.
 *
 * @author:	Jose Carlos Martinez Garcia-Vaso <carlosgvaso@gmail.com>
 */

#include "main.h"


/**
 * @brief Shell initialization tasks
 */
void initShell() {
	// Use shell history
	//using_history();

	// Set up parent process signals
	signal(SIGTTOU, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	//signal(SIGCHLD, SIG_IGN);

	// TODO: Other init tasks
}


/**
 * @brief Check for input that should be ignored.
 *
 * This function checks for empty input strings and strings consisting of
 * whitespace characters only, and ignores such input.
 *
 * @param input_str	Inupt string to check
 * @return	1 if the input should be ignored, 0 otherwise
 */
bool ignoreInput(char* input_str) {
	// Check for empty command string
	if (!strcmp(input_str, EMPTY_STR)) {
		return (true);
	}

	// Check for a command string containing only whitespace
	uint8_t whitespace = true;
	for (size_t i=0; i<strlen(input_str); i++) {
		if (!isspace(input_str[i])) {
			whitespace = false;
		}
	}

	if (whitespace) {
		return (true);
	}

	return (false);
}


/**
 * @brief Remove job from job_arr
 *
 * @param	job_idx	Job index in the job_arr
 */
void removeJob(int job_idx) {
	// Clear job entries
	job_arr[job_idx].jobno = 0;
	job_arr[job_idx].gpid = 0;
	strcpy(job_arr[job_idx].status, "\0");

	// Decrease last job number if necessary
	if (job_idx == last_job) {
		// Find the next job
		bool next_job_found = false;
		while (!next_job_found) {
			last_job--;
			if (job_arr[last_job].jobno > 0) {
				next_job_found = true;
			}
		}
	}
}


/**
 * @brief Print job information
 *
 * @param	job_idx	Job array index
 */
void printJob(int job_idx) {
	// Print the job number
	printf("[%d]", job_arr[job_idx].jobno);

	// Print current job indicator
	if (last_job == job_idx) {
		printf("+");
	} else {
		printf("-");
	}

	// Print job status
	printf(" %s", job_arr[job_idx].status);

	// Print job command string
	printf("\t");
	for (int j=0; j<job_arr[job_idx].cmd_tok_len; j++) {
		printf("%s ", job_arr[job_idx].cmd_tok[j]);
	}
	printf("\n");
}


/**
 * @brief Send command to the background.
 *
 * TODO: Implement bg
 */
void bgExec() {

}


/**
 * @brief Send command to the foreground.
 *
 * TODO: Implement fg
 */
void fgExec() {

}


/**
 * @brief Display jobs table.
 *
 * TODO: Implement jobs
 */
void jobsExec() {
	// Update the jobs table
	maintainJobsTable();

	// Check we at least have one job in the list
	if (last_job <= EMPTY_ARRAY) {
		printf("No jobs in job table\n");
		return;
	}

	// Iterate over all the jobs in the array
	for (int i=0; i<=last_job; i++) {
		// Only print active jobs
		if (!strcmp(job_arr[i].status, JOB_STATUS_RUNNING) ||
				!strcmp(job_arr[i].status, JOB_STATUS_STOPPED)) {
			// Print the job info
			printJob(i);
		}
	}
}


/**
 * @brief Check if input is shell command, and run it.
 *
 * @param	input	Raw input string
 * @return	True if shell command ran, false if it is not a shell command
 */
bool runShellCmd(char* input) {
	if (!strcmp(input, CMD_BG)) {
		bgExec();
		return true;
	} else if (!strcmp(input, CMD_FG)) {
		fgExec();
		return true;
	} else if (!strcmp(input, CMD_JOBS)) {
		jobsExec();
		return true;
	}
	return false;
}


/**
 * @brief Split a command string into string tokens.
 *
 * This function assumes the `cmd.cmd_str` is not an empty string.
 *
 * This function uses strtok() to split a command string into string tokens.
 * Tokens are considered to be contiguous characters separated by whitespace.
 *
 * @param	cmd	Command struct
 *
 * @sa strtok(), Cmd
 */
void tokenizeString(struct Job* cmd) {
	const char CMD_TOKEN_DELIM = ' ';	// From requirements
	size_t len = 0;

	// Remove final newline char and replace with NULL char
	len = strlen(cmd->cmd_str);
	if (cmd->cmd_str[len-1] == '\n') {
		cmd->cmd_str[len-1] = '\0';
	}

	// Break down the command into tokens using strtok
	cmd->cmd_tok[0] = strtok(cmd->cmd_str, &CMD_TOKEN_DELIM);
	uint32_t count = 1;	// Start at one because we already run strtok once

	while ((cmd->cmd_tok[count] = strtok(NULL, &CMD_TOKEN_DELIM))) {
		count++;
	}
	cmd->cmd_tok_len = count;
}


/**
 * @brief Parse a command.
 *
 * This function assumes the `cmd.cmd_str` is not an empty string.
 *
 * This function takes a raw command string, and parses it to load it into a
 * `Job` struct as per the requirements.
 *
 * TODO: Might need check for multiple redirections of the same type to raise an error.
 *
 * @param	cmd_str		Raw command string
 * @param	job_arr	Jobs list of parsed commands
 * @param	last_job	Last job added to the job_arr
 *
 * @sa	Cmd
 */
void parseJob(char* cmd_str, struct Job job_arr[], int* last_job) {
	const char I_REDIR_OPT[2] = "<\0";
	const char O_REDIR_OPT[2] = ">\0";
	const char E_REDIR_OPT[3] = "2>\0";
	const char BG_OPT[2] = "&\0";
	const char PIPE_OPT[2] = "|\0";
	const char SYNTAX_ERR_1[MAX_ERROR_LEN] = "syntax error: command should not"
			" start with \0";
	const char SYNTAX_ERR_2[MAX_ERROR_LEN] = "syntax error: near token \0";
	const char SYNTAX_ERR_3[MAX_ERROR_LEN] = "syntax error: command should not"
			" end with \0";
	const char SYNTAX_ERR_4[MAX_ERROR_LEN] = "syntax error: & should be the last"
			" token of the command\0";

	// Save and tokenize command string
	strcpy(job_arr[*last_job].cmd_str, cmd_str);
	tokenizeString(&job_arr[*last_job]);

	/*
	 * Iterate over all tokens to look for arguments, redirection directives,
	 * pipes and background directives
	 */
	int cmd_count = 0;	// Command array counter
	for (uint32_t i=0; i<job_arr[*last_job].cmd_tok_len; i++) {
		if (!strcmp(I_REDIR_OPT, job_arr[*last_job].cmd_tok[i])) {	// Check for input redir
			// Check if redirection token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_1);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else if (i >= job_arr[*last_job].cmd_tok_len-1) {	// Check it is not the last token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_3);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else if (!strcmp(I_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(BG_OPT, job_arr[*last_job].cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_2);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else {	// Correct syntax
				i++;	// Move ahead one iter to get the redir argument
				if (!job_arr[*last_job].pipe) {
					strcpy(job_arr[*last_job].in1, job_arr[*last_job].cmd_tok[i]);
				} else {
					strcpy(job_arr[*last_job].in2, job_arr[*last_job].cmd_tok[i]);
				}
			}
		} else if (!strcmp(O_REDIR_OPT,job_arr[*last_job].cmd_tok[i])) {	// Output redir
			// Check if redirection token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_1);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else if (i >= job_arr[*last_job].cmd_tok_len-1) {	// Check it is not the last token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_3);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else if (!strcmp(I_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(BG_OPT, job_arr[*last_job].cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_2);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else {	// Correct syntax
				i++;	// Move ahead one iter to get the redir argument
				if (!job_arr[*last_job].pipe) {
					strcpy(job_arr[*last_job].out1, job_arr[*last_job].cmd_tok[i]);
				} else {
					strcpy(job_arr[*last_job].out2, job_arr[*last_job].cmd_tok[i]);
				}
			}
		} else if (!strcmp(E_REDIR_OPT, job_arr[*last_job].cmd_tok[i])) {	// Error redir
			// Check if redirection token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_1);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else if (i >= job_arr[*last_job].cmd_tok_len-1) {	// Check it is not the last token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_3);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else if (!strcmp(I_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(BG_OPT, job_arr[*last_job].cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_2);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else {	// Correct syntax
				i++;	// Move ahead one iter to get the redir argument
				if (!job_arr[*last_job].pipe) {
					strcpy(job_arr[*last_job].err1, job_arr[*last_job].cmd_tok[i]);
				} else {
					strcpy(job_arr[*last_job].err2, job_arr[*last_job].cmd_tok[i]);
				}
			}
		} else if (!strcmp(PIPE_OPT, job_arr[*last_job].cmd_tok[i])) {	// Pipe command
			// Check if pipe token has the correct syntax
			if (i <= 0 || cmd_count <= 0) {	// Check it is not the first token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_1);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else if (i >= job_arr[*last_job].cmd_tok_len-1) {	// Check it is not the last token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_3);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else if (!strcmp(I_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(O_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(E_REDIR_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(PIPE_OPT, job_arr[*last_job].cmd_tok[i+1]) ||
					!strcmp(BG_OPT, job_arr[*last_job].cmd_tok[i+1])) {	// Check there is an argument after this token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_2);
				strcat(job_arr[*last_job].err_msg, job_arr[*last_job].cmd_tok[i]);
				return;
			} else {	// Correct syntax
				job_arr[*last_job].pipe = true;
				cmd_count = 0;	// Start argument count for cmd2
			}
		} else if (!strcmp(BG_OPT, job_arr[*last_job].cmd_tok[i])) {	// Background command
			// Check if background token has the correct syntax
			if (i != job_arr[*last_job].cmd_tok_len-1) {	// Check if it is not the last token
				strcpy(job_arr[*last_job].err_msg, SYNTAX_ERR_4);
				return;
			} else {
				job_arr[*last_job].bg = true;
			}
		} else {	// Command argument
			if (!job_arr[*last_job].pipe) {
				job_arr[*last_job].cmd1[cmd_count] = job_arr[*last_job].cmd_tok[i];
				cmd_count++;
			} else {
				job_arr[*last_job].cmd2[cmd_count] = job_arr[*last_job].cmd_tok[i];
				cmd_count++;
			}
		}
	}
}


/**
 * @brief Redirect input, output and error of simple commands without pipes, or
 * left child of a pipe.
 *
 * @param	cmd	Command to set the redirection
 */
void redirectSimple(struct Job* cmd) {
	const char REDIR_ERR_1[MAX_ERROR_LEN] = "open errno ";
	const char REDIR_ERR_2[MAX_ERROR_LEN] = ": could not open file: ";
	extern errno;
	char errno_str[sizeof(int)*8+1];

	// Input redirection
	if (strcmp(cmd->in1, EMPTY_STR)) {
		//
		int i1fd = open(cmd->in1, O_RDONLY,
						S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (i1fd == SYSCALL_RETURN_ERR) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->in1);
			return;
		}

		dup2(i1fd, STDIN_FILENO);
		close(i1fd);
	}

	// Output redirection
	if (strcmp(cmd->out1, EMPTY_STR)) {
		// Create output file
		int o1fd = open(cmd->out1, O_WRONLY|O_CREAT|O_TRUNC,
				S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (o1fd == SYSCALL_RETURN_ERR) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->out1);
			return;
		}

		dup2(o1fd, STDOUT_FILENO);
		close(o1fd);
	}

	// Error redirection
	if (strcmp(cmd->err1, EMPTY_STR)) {
		//
		int e1fd = open(cmd->err1, O_WRONLY|O_CREAT|O_TRUNC,
						S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (e1fd == SYSCALL_RETURN_ERR) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->err1);
			return;
		}

		dup2(e1fd, STDERR_FILENO);
		close(e1fd);
	}
}


/**
 * @brief Redirect input, output and error of right child of a pipe.
 *
 * @param	cmd	Command to set the redirection
 */
void redirectPipe(struct Job* cmd) {
	const char REDIR_ERR_1[MAX_ERROR_LEN] = "open errno ";
	const char REDIR_ERR_2[MAX_ERROR_LEN] = ": could not open file: ";
	extern errno;
	char errno_str[sizeof(int)*8+1];

	// Input redirection
	if (cmd->pipe && strcmp(cmd->in2, EMPTY_STR)) {
		//
		int i2fd = open(cmd->in2, O_RDONLY,
						S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (i2fd == SYSCALL_RETURN_ERR) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->in2);
			return;
		}

		dup2(i2fd, STDIN_FILENO);
		close(i2fd);
	}

	// Output redirection
	if (cmd->pipe && strcmp(cmd->out2, EMPTY_STR)) {
		//
		int o2fd = open(cmd->out2, O_WRONLY|O_CREAT|O_TRUNC,
						S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (o2fd == SYSCALL_RETURN_ERR) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->out2);
			return;
		}

		dup2(o2fd, STDOUT_FILENO);
		close(o2fd);
	}

	// Error redirection
	if (cmd->pipe && strcmp(cmd->err2, EMPTY_STR)) {
		//
		int e2fd = open(cmd->err2, O_WRONLY|O_CREAT|O_TRUNC,
						S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);

		if (e2fd == SYSCALL_RETURN_ERR) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, REDIR_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, REDIR_ERR_2);
			strcat(cmd->err_msg, cmd->err2);
			return;
		}

		dup2(e2fd, STDERR_FILENO);
		close(e2fd);
	}

}


/**
 * @brief Set up signal handling to relay signals to children processes.
 *
 * This function is based on the UT Austin EE 382V Systems Programming class
 * examples posted by Dr. Ramesh Yerraballi.
 *
 * TODO: Add support for job control.
 *
 * @param cmd	Parsed command
 */
void waitForChildren(struct Job* cmd) {
	const char SIG_ERR_1[MAX_ERROR_LEN] = "signal errno ";
	const char SIG_ERR_2[MAX_ERROR_LEN] = ": waitpid error";
	extern errno;
	char errno_str[sizeof(int)*8+1];

	int status;
	uint8_t count = 0;
	int child_num;

	// Determine the number of child processes
	if (cmd->pipe) {
		child_num = CHILD_COUNT_PIPE;
	} else {
		child_num = CHILD_COUNT_SIMPLE;
	}

	// Wait for child to exit
	while (count < child_num) {
		/**
		 * TODO: Fix EINTR (4) error
		 *
		 * This snippet always throws the EINTR (4) error code. The
		 * documentation for waitpid (3) says EINTR means: "WNOHANG was not set
		 * and an unblocked signal or a SIGCHLD was caught; see signal(7)."
		 *
		 * TODO: Fix WCONTINUED compilation error
		 *
		 * See this for error description: https://stackoverflow.com/questions/
		 * 60101242/compiler-error-using-wcontinued-option-for-waitpid
		 */
		//if (waitpid(-1, &status, WUNTRACED|WCONTINUED) == SYSCALL_RETURN_ERR) {
		if (waitpid(job_arr[last_job].gpid, &status, WUNTRACED) == SYSCALL_RETURN_ERR) {
			sprintf(errno_str, "%d", errno);
			strcpy(cmd->err_msg, SIG_ERR_1);
			strcat(cmd->err_msg, errno_str);
			strcat(cmd->err_msg, SIG_ERR_2);
			return;
		}

		if (WIFEXITED(status)) {
			if (verbose) {
				printf("-yash: child process terminated normally\n");
			}
			count++;
		} else if (WIFSIGNALED(status)) {
			printf("\n");	// Ensure there is an space after "^C"
			if (verbose) {
				printf("-yash: child process terminated by a signal\n");
			}
			count++;
		} else if (WIFSTOPPED(status)) {
			printf("\n");	// Ensure there is an space after "^Z"
			if (verbose) {
				printf("-yash: child process stopped by a signal\n");
			}
			//
		} /*else if (WIFCONTINUED(status)) {
			//
		}*/
	}
}


/**
 * @brief Execute commands.
 *
 * This function allows for both simple and piped commands (1 pipe only).
 * Moreover, the command is checked for correctness, and ignored if it does not
 * exist.
 *
 * This function is based on the UT Austin EE 382V Systems Programming class
 * examples by Dr. Ramesh Yerraballi.
 *
 * TODO: Add support for job control.
 *
 * @param	job_arr	Jobs list of parsed commands
 * @param	last_job	Last job added to the job_arr
 */
void runJob(struct Job job_arr[], int* last_job) {
	const char PIPE_ERR_1[MAX_ERROR_LEN] = "pipe errno ";
	const char PIPE_ERR_2[MAX_ERROR_LEN] = ": failed to make pipe";
	extern errno;
	char errno_str[sizeof(int)*8+1];

	pid_t c1_pid, c2_pid;
	int pfd[2];
	int stdout_fd;

	if (job_arr[*last_job].pipe) {
		stdout_fd = dup(STDOUT_FILENO);	// Save stdout

		if (pipe(pfd) == SYSCALL_RETURN_ERR) {
			sprintf(errno_str, "%d", errno);
			strcpy(job_arr[*last_job].err_msg, PIPE_ERR_1);
			strcat(job_arr[*last_job].err_msg, errno_str);
			strcat(job_arr[*last_job].err_msg, PIPE_ERR_2);
			return;
		}
	}

	c1_pid = fork();

	if (c1_pid == 0) {	// Child 1 or left child process
		// Create a new session and a new group, and become group leader
		setpgid(0, 0);

		// Set up signal handling sent to the children process group
		if (verbose) {
			printf("-yash: children process group: ignoring signal SIGTTOU, "
					"but getting all the others\n");
		}
		signal(SIGTTOU, SIG_IGN);
		signal(SIGINT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		signal(SIGCHLD, SIG_DFL);

		if (job_arr[*last_job].pipe) {
			close(pfd[0]);	// Close unused read end
			dup2(pfd[1], STDOUT_FILENO);	// Make output go to pipe
		}
		// Do additional redirection if necessary
		redirectSimple(&job_arr[*last_job]);
		if (strcmp(job_arr[*last_job].err_msg, EMPTY_STR)) {
			if (job_arr[*last_job].pipe) {
				dup2(stdout_fd, STDOUT_FILENO);	// Allow to write to stdout
			}
			printf("-yash: %s\n", job_arr[*last_job].err_msg);
			exit(EXIT_ERR_CMD);
		}

		// Execute command
		if (execvp(job_arr[*last_job].cmd1[0], job_arr[*last_job].cmd1) == SYSCALL_RETURN_ERR
				&& verbose) {
			printf("-yash: execvp() errno: %d\n", errno);
		}
		// Make sure we terminate child on execvp() error
		exit(EXIT_ERR_CMD);
	} else {	// Parent process
		if (job_arr[*last_job].pipe) {
			c2_pid = fork();

			if (c2_pid == 0) {	// Child 2 or right child process
				// Join the group created by child 1
				setpgid(0, c1_pid);

				// Set up signal handling sent to the children process group
				if (verbose) {
					printf("-yash: children process group: ignoring signal SIGTTOU, "
							"but getting all the others\n");
				}
				signal(SIGTTOU, SIG_IGN);
				signal(SIGINT, SIG_DFL);
				signal(SIGTSTP, SIG_DFL);

				close(pfd[1]);	// Close unused write end
				dup2(pfd[0], STDIN_FILENO);	// Get input from pipe

				// Do additional redirection if necessary
				redirectPipe(&job_arr[*last_job]);
				if (strcmp(job_arr[*last_job].err_msg, EMPTY_STR)) {
					dup2(stdout_fd, STDOUT_FILENO);	// Allow to write to stdout
					printf("-yash: %s\n", job_arr[*last_job].err_msg);
					exit(EXIT_ERR_CMD);
				}

				// Execute command
				if (execvp(job_arr[*last_job].cmd2[0], job_arr[*last_job].cmd2) == SYSCALL_RETURN_ERR
						&& verbose) {
					printf("-yash: execvp() errno: %d\n", errno);
				}
				// Make sure we terminate child on execvp() error
				exit(EXIT_ERR_CMD);
			}
			// Parent process. Close pipes so EOF can work
			close(pfd[0]);
			close(pfd[1]);
			close(stdout_fd);
		}

		// Parent process
		// Save job gpid
		job_arr[*last_job].gpid = c1_pid;
		if (!job_arr[*last_job].bg) {
			// Give terminal control to child
			if (verbose) {
				printf("-yash: "
						"giving terminal control to child process group\n");
			}
			tcsetpgrp(0, c1_pid);

			// Block while waiting for children
			waitForChildren(&job_arr[*last_job]);
			if (strcmp(job_arr[*last_job].err_msg, EMPTY_STR)) {
				return;
			}

			// Get back terminal control to parent
			if (verbose) {
				printf("-yash: returning terminal control to parent process\n");
			}
			tcsetpgrp(0, getpid());
			removeJob(*last_job);	// Remove job from jobs table
		}
	}
}


/**
 * @brief Handle new job.
 *
 * This function parses the raw input of the new job, adds the job to the jobs
 * table, and it executes the new job.
 *
 * @param	input	Raw input of the new job
 */
void handleNewJob(char* input) {
	// Initialize a new Job struct
	struct Job job = {
			EMPTY_STR,		// cmd_str
			{ EMPTY_STR },	// cmd_tok
			0,				// cmd_tok_size
			{ EMPTY_STR },	// cmd1
			EMPTY_STR,		// in1
			EMPTY_STR,		// out1
			EMPTY_STR,		// err1
			{ EMPTY_STR },	// cmd2
			EMPTY_STR,		// in2
			EMPTY_STR,		// out2
			EMPTY_STR,		// err2
			false,			// pipe
			false,			// bg
			EMPTY_ARRAY,	// gpid
			EMPTY_ARRAY,	// jobno
			EMPTY_STR,		// status
			EMPTY_STR		// err_msg
	};

	// Add command to the jobs array
	if (last_job < MAX_CONCURRENT_JOBS) {
		last_job++;
		job_arr[last_job] = job;
		job_arr[last_job].jobno = last_job + 1;
		strcpy(job_arr[last_job].status, JOB_STATUS_RUNNING);
	} else {
		printf("-yash: max number of concurrent jobs reached: %d",
				last_job);
		return;
	}

	// Parse job
	if (verbose) {
		printf("-yash: parsing input...\n");
	}
	parseJob(input, job_arr, &last_job);
	if (strcmp(job_arr[last_job].err_msg, EMPTY_STR)) {
		printf("-yash: %s\n", job_arr[last_job].err_msg);
		return;
	}

	// Run job
	if (verbose) {
		printf("-yash: executing command...\n");
	}
	runJob(job_arr, &last_job);
	if (last_job != EMPTY_ARRAY) {
		if (strcmp(job_arr[last_job].err_msg, EMPTY_STR)) {
			printf("-yash: %s\n", job_arr[last_job].err_msg);
			return;
		}
	}
}

/**
 * @brief Check if any background jobs finished.
 *
 * Check if any previously running job in the jobs table has finished running.
 *
 * TODO: Implement
 */
void maintainJobsTable() {
	// Check every job in the job_arr
	for (int i=0; i<=last_job; i++) {
		// Skip jobs that already finished
		if (!strcmp(job_arr[i].status, JOB_STATUS_RUNNING) ||
				!strcmp(job_arr[i].status, JOB_STATUS_STOPPED)) {
			int status;
			if (waitpid(job_arr[i].gpid, &status, WNOHANG|WUNTRACED|WCONTINUED) == SYSCALL_RETURN_ERR) {
				printf("-yash: error checking child %d status: %d\n",
						job_arr[i].gpid, errno);
				// TODO: handle error
			}
			if (WIFEXITED(status)) {
				if (verbose) {
					printf("-yash: child process terminated normally\n");
				}

				// Change status to done and, remove child from array
				strcpy(job_arr[i].status, JOB_STATUS_DONE);
				printJob(i);
				removeJob(i);

			} else if (WIFSIGNALED(status)) {
				if (verbose) {
					printf("-yash: child process terminated by a signal\n");
				}

				// Change status to done, and remove child from array
				strcpy(job_arr[i].status, JOB_STATUS_DONE);
				printJob(i);
				removeJob(i);
			} else if (WIFSTOPPED(status)) {
				if (verbose) {
					printf("-yash: child process stopped by a signal\n");
				}

				// Change status to stopped
				strcpy(job_arr[i].status, JOB_STATUS_STOPPED);
			} else if (WIFCONTINUED(status)) {
				if (verbose) {
					printf("-yash: child process continued by a signal\n");
				}

				// Change status to running
				strcpy(job_arr[i].status, JOB_STATUS_RUNNING);
			}
		}
	}
}

/**
 * @brief Send a SIGKILL to all jobs in the jobs list
 */
void killAllJobs() {
	for (int i=0; i<last_job; i++) {
			// Skip jobs that already finished
			if (!strcmp(job_arr[i].status, JOB_STATUS_RUNNING) ||
					!strcmp(job_arr[i].status, JOB_STATUS_STOPPED)) {
				kill(job_arr[i].gpid, SIGKILL);
			}
	}
}


/**
 * @brief Point of entry.
 *
 * @param argc	Number of command line arguments
 * @param argv	Array of command line arguments
 * @return	Errorcode
 */
int main(int argc, char** argv) {
	const char USAGE[MAX_ERROR_LEN] = "\nUsage:\n"
			"./yash [options]\n"
			"\n"
			"Options:\n"
			"\t-v, --verbose\tVerbose output from shell\n";
	const char ARG_ERROR[MAX_ERROR_LEN] = "-yash: unknown argument: ";
	const char V_FLAG_SHORT[3] = "-v\0";
	const char V_FLAG_LONG[10] = "--verbose\0";
	const char V_INFO[MAX_ERROR_LEN] = "-yash: verbose output set\n";
	char* in_str;

	// Read command line arguments
	verbose = false;
	if (argc > 1) {
		for (int i=1; i<argc; i++){
			if (!strcmp(V_FLAG_SHORT, argv[i])
					|| !strcmp(V_FLAG_LONG, argv[i])) {
				verbose = true;
				printf(V_INFO);
			} else {
				printf(ARG_ERROR);
				printf("%s\n", argv[i]);
				printf(USAGE);
				return (EXIT_ERR_ARG);
			}
		}
	}

	// Initialize the shell
	initShell();

	/*
	 * Use `readline()` to control when to exit from the shell. Typing
	 * [Ctrl]+[D] on an empty prompt line will exit as stated in the
	 * requirements. From `readline()` documentation:
	 *
	 * "If readline encounters an EOF while reading the line, and the line is
	 * empty at that point, then (char *)NULL is returned. Otherwise, the line
	 * is ended just as if a newline had been typed."
	 */
	while ((in_str = readline("# "))) {
		// Check input to ignore and show the prompt again
		if (verbose) {
			printf("-yash: checking if input should be ignored...\n");
		}

		// Check if input should be ignored
		if (ignoreInput(in_str)) {
			if (verbose) {
				printf("-yash: input ignored\n");
			}
		} else if (runShellCmd(in_str)) {	// Check if input is a shell command
			if (verbose) {
				printf("-yash: ran shell command\n");
			}
		} else {	// Handle new job
			if (verbose) {
				printf("-yash: new job\n");
			}
			handleNewJob(in_str);
		}

		// Check for finished jobs
		maintainJobsTable();
	}

	// Ensure a new-line on exit
	printf("\n");
	if (verbose) {
		printf("-yash: exiting...\n");
	}
	killAllJobs();

	// TODO: Ensure all child processes are dead on exit

	return (EXIT_OK);
}
