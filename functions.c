#include "functions.h"

void cd(char* change)
{
	char dir[512] = "\0"; //set directory to be nothing initially
	char dir_name[512]; //variable to show directory name
	char* check;

	//use string tokens to get the directory to cd to
	while(change != NULL)
	{
		strcat(dir, change);//concatenate the string with each character
		change = strtok(NULL, " \n"); //get next character while no space
	}
      	
	char* new_dir = dir+2; //remove the "cd" part from the directory name

	check = strchr(new_dir, ';');

	if(check != NULL)
	{
		*check = '\0';
	}

	//if no argument was passed, set the new directory to user's home path
	if(strcmp(new_dir, "")==0)
	{
		new_dir = getenv("HOME");
	}

	//invoke chdir command and error check
	if(chdir(new_dir) != 0)
	{
		perror(new_dir);
	}
}

void path(char* pathname, char* prompt) {
        char dir[512] = "\0";
        char dir_name[512];
        char filepath[512];
        getcwd(filepath, sizeof(filepath));
        char* check;

        while(pathname != NULL)
        {
                strcat(dir, pathname);
                pathname = strtok(NULL, " \n");
        }

        char* new_dir = dir+4;
        char* modifier = dir+6;

        if(strcmp(new_dir, "") == 0)
        {
                printf("%s> %s\n", prompt, filepath);
        }
        else if(strchr(new_dir, '+') != NULL)
        {
                strcat(filepath, modifier);
                printf("%s> %s\n", prompt, filepath);
        }
        else if(strchr(new_dir, '-') != NULL)
        {
                printf("%s> %s\n", prompt, filepath);
        }

        return;
}

void execFlags(char** flags){
	
	pid_t pid = fork(); //fork the processes
	if(pid == -1) {
		perror("fork error"); //if there was a fork error
	}//error handling
	else if(pid == 0) {
		execvp(flags[0], flags); //execute the command with listed flags
		printf("command %s not found\n", flags[0]); //print an error if it does not execute
		exit(1); //exit the child process
	}//child
	else wait( (int *)0 ); //wait for the child process to complete

}

void pipecmd(char* line){
	int status;
	int i;
	char* passedargs[3];
	char* flags1[512];
	char* flags2[512];
	char* flags3[512];
	int count = 0;
	pid_t pid1, pid2, pid3;
	
	//get the number of pipes in passed argument to know how many pipes to open or if too many pipes given
	for (i = 0; i < strlen(line); i++)
    	{
        	if (line[i] == '|')
        	{
            	count++;
        	}
    	}
	
	//if too many pipes given, exit this command
	if(count > 2)
	{
		printf("Too many pipes provided. Please only provide up to 2 pipes\n");
		return;
	}
	
	//delimit entire line by the | character to get each individual command
	i=0;
	line = strtok(line, "|");
	while(line != NULL)
	{
		passedargs[i] = line;
		line = strtok(NULL, "|");
		i++;
	}
	
	//delimit the first index of the char* array for the first command
	i=0;
	line = strtok(passedargs[0], " ");
	while(line != NULL)
	{
		flags1[i] = line;
		line = strtok(NULL, " ");
		i++;
	}
	
	//delimit the second index of the char* array for the second command
	i=0;
	line = strtok(passedargs[1], " ");
	while(line != NULL)
	{
		flags2[i] = line;
		line = strtok(NULL, " ");
		i++;
	}
	
	//if there are two pipes provided
	if(count == 2)
	{
		//delimit the third index of the char* array for the third command
		i=0;
		line = strtok(passedargs[2], " ");
		while(line != NULL)
		{
			flags3[i] = line;
			line = strtok(NULL, " ");
			i++;
		}
	}

	int fd1[2], fd2[2];

    // make first pipe
	if(pipe(fd1) == -1)
	{
		perror("pipe");
	}

    // make second pipe if needed
	if(count == 2)
	{
		if(pipe(fd2) == -1)
		{
			perror("pipe");
		}
	}
		
	// fork the first child
	if ((pid1 = fork()) == 0)
	{
		// duplicate write end of first pipe to stdout
		dup2(fd1[1], 1);

		// close both ends of first pipe
      	close(fd1[0]);
		close(fd1[1]);
		// close both ends of second pipe if needed
		if(count == 2)
		{
			close(fd2[0]);
			close(fd2[1]);
		}
      			
	    execvp(*flags1, flags1); //execute first command
 	}
	else if(pid1 < 0) //error check first fork
	{
		perror("fork");
	}
  	else // parent (assume no error)
    {
        // fork second child
        if ((pid2 = fork()) == 0)
		{
	  		// duplicate read end of first pipe to stdin
	  		dup2(fd1[0], 0);	  		

	  		// close both ends of first pipe
	  		close(fd1[0]);
			close(fd1[1]);
			
			//duplicate write end and close second pipe if needed
			if(count == 2)
			{
				dup2(fd2[1], 1); 
				close(fd2[0]);
				close(fd2[1]);
			}

	  		execvp(*flags2, flags2); //execute second command
		}
		else if(pid2 < 0) //error check second fork
		{
			perror("fork");
		}
        else // parent (assume no error)
		{
			if(count == 2)
			{
				// fork third child if needed
				if ((pid3 = fork()) == 0)
				{
					// duplicate read end of second pipe to stdin but keep stdout
					dup2(fd2[0], 0);

					// close both ends of all pipes
					close(fd1[0]);
					close(fd1[1]);
					close(fd2[0]);
					close(fd2[1]);
		
					execvp(*flags3, flags3); //execute third command
				}
				else if(pid3 < 0) //error check third fork
				{
					perror("fork");
				}
			}
		}
    }
      
  	// close first pipe
  	close(fd1[0]);
  	close(fd1[1]);
	//close second pipe if needed
	if(count == 2)
	{
		close(fd2[0]);
		close(fd2[1]);
	}

	//wait for all children
  	for (i = 0; i < count + 1; i++)
	{
    	wait(&status);
	}
}

void redircmd(char* line){
	char *ptr;
	int i = 0;
	int j = 0;
	char *flags[512];
	const char delim[2] = " \n";
	int fd[10];
	int checking = 0;
	
	if(fork() == 0){
		ptr = strtok(line, delim);
		while( ptr != NULL ){
			
			if(checking == 0){
				flags[i] = ptr;
				i++;
				ptr = strtok(NULL, delim);
			}//breaks by spacing
		
			if(ptr != NULL){
				checking = 0;
				if(strcmp(ptr, ">") == 0){
					ptr = strtok(NULL, delim);
					fd[j] = open(ptr, O_CREAT | O_TRUNC | O_WRONLY, 0600);

					dup2(fd[j], 1);
					close(fd[j]);//performs writing
					j++;
					
					ptr = strtok(NULL, delim);//get next index
					if(ptr != NULL){
						if(strcmp(ptr, "<") == 0 || strcmp(ptr, ">") == 0) checking = 1;
					}//checks if next index is redirection
				}//checks for > redirection
			
				else if(strcmp(ptr, "<") == 0){
					ptr = strtok(NULL, delim);
					fd[j] = open(ptr, O_RDONLY);

					dup2(fd[j], 0);
					close(fd[j]);//performs reading
					j++;
					
					ptr = strtok(NULL, delim);//gets next index
					if(ptr != NULL){
						if(strcmp(ptr, "<") == 0 || strcmp(ptr, ">") == 0) checking = 1;
					}//checks if next index is redirection
				}//checks for other direction
			
			}//checks if null
		
		}//runs till null
		flags[i] = NULL;//sets last to NULL
		
		execvp(flags[0], flags);//exec
	}//child
	else{
		wait( (int *)0 );
		return;
	}//parent
}

int ispipe(char* line)
{
	char dir[512] = "\0"; //character array to hold the concatenated string to check for pipes
	int i = 0;

	//concatenate the entire line passed in to check for pipes strchr
	while(line != NULL)
	{
		strcat(dir, line);
		line = strtok(NULL, ";");
		if(i == 0) strcat(dir, " ");
		i++;
	}
	
	//puts(dir);
	//only execute pipecmd if only pipes are found
	if(strchr(dir, '|') != NULL && strchr(dir, '<') == NULL && strchr(dir, '>') == NULL)
	{	
		pipecmd(dir);
		return 1;
	}
	
	if(strchr(dir, '|') == NULL && strchr(dir, '<') != NULL || strchr(dir, '>') != NULL)
	{	
		redircmd(dir);
		return 1;
	}
	
	return 0;
}
void myhistory( char history[][512], int counter)
{
	int y = 0;
        printf("Command History: \n");
		//while loop that prints out the commands that have been used
                while (y < counter)
                {
                        printf("%s, ", history[y]);
			y++;
                }
        printf("\n");
        
}

