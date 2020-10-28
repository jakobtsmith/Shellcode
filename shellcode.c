#include "functions.h"

#define MAX 512

int main(int argc, char* argv[]) {
	char cmd[MAX];//taking in commands
	char send[MAX];
	char prompt[MAX];
	
	 char history[20][MAX];
	char *flags[MAX];//parsing
	char* breaking[MAX];
	char *ptr;//for delimiting

	FILE *fp = stdin; //dynamic file pointer for interactive and batch mode

	const char delim[2] = ";";//delimiting
	const char odelim[2] = " \n";
	
	int i = 0;//counting 
	int j = 0;
	int k = 0;
	int x = 0;
	int z;
	int v = 0;	
	int checking = 0;//switches
	int excheck = 0;
	
	printf("What would you like the prompt to be: ");
	gets(prompt);
	
	if(argc == 2) //if entering batch mode
	{
		fp = fopen(argv[1], "r"); //open the provided file
		if(fp == NULL)
		{
			printf("Failed to open %s\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		
	}
	//if more than one file provided
	else if(argc > 2)
	{
		printf("Too many files passed to shell. Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	//if arguments exist call batch mode
	while(1) {

		printf("%s> ", prompt);
		
		//gets(cmd);
		
		if(fgets(cmd, MAX, fp) == NULL) //get input from user and exit if at end of file or some other error
		{
			excheck = 1;
			checking = 1;
		}

		ptr = strtok(cmd, delim); //get the first token
		if(argc == 2 && checking == 0 && excheck == 0) printf("Command entered: %s\n", ptr);
		i = 0;
		while(ptr != NULL){
			breaking[i] = ptr;
			i++;
			ptr = strtok(NULL, delim);
		}//breaks up by semi colons
	
		j = 0;
		while(j < i){
			char line[MAX] = "\0";
			strcpy(send, breaking[j]);
			ptr = strtok(send, odelim);//breaks up by spacing
		
			if(checking == 0){
				if(ispipe(ptr) == 1) checking = 1;//runs for redirection/pipe
			}
			
			strcpy(send, breaking[j]); //copy string back to original state before checking
			ptr = strtok(send, odelim);

			//check for empty strings passed to function
			int nullcount = 0;
			int didbreak = 0;
			while(ptr == NULL) //while the command is null
			{
				if(nullcount == 0) printf("Null passed. Command will not execute\n"); //print that a null was passed the first time we catch it
				if(j < i) //increment if possible
				{
					j++;
					if(breaking[j] != " \n"){ //check the next value
						if(breaking[j] != NULL) {
						strcpy(send, breaking[j]);
	                        		ptr = strtok(send, odelim);
						}
						else { didbreak = 1; break; } //break on a null
					}
					else { didbreak = 1; break; } //break on a newline
				}
				else { didbreak = 1; break; } //break if there's no more commands
				nullcount++; //increment the number of times the loop has been passed through
			}

			if(didbreak == 1) break; //break if the previous loop was broken from
			if (x == 20)
                        {
                                v = 1;
                                x = x - 20;
                        }
                        //deletes the previous commands if more than 20 have been inputed and starts over agian
                        if(v == 1)
                        {
                                strcpy(history[x],"\0");
                        }
                        //copies the coomand to the history string array
                        strcpy(history[x],ptr);
                        x++;
			if(strcmp(ptr, "cd") == 0){
				cd(ptr);
				checking = 1;
			}//runs for directory change
			
			else if(strcmp(ptr, "exit") == 0){
				excheck = 1;
				checking = 1;
			}//runs for exiting
			else if(strcmp(ptr, "path") == 0) {
				path(ptr, prompt);
				checking = 1;
			}
			else if(strcmp(ptr, "myhistory") == 0){
				//prints out the history of the commands used
				myhistory(history,x);
				x++;
				checking = 1;
			}//runs for history

			else{
				k = 0;
				while(ptr != NULL){
					flags[k] = ptr;
					k++;
					ptr = strtok(NULL, odelim);
				}//loads flag array
				flags[k] = ptr;
			}//if command is regular
		
			if(checking == 0) execFlags(flags);//executes if regular
			j++;
			checking = 0;

		}//runs through tokens

		if(excheck == 1){
			kill(getppid(), SIGINT);
			printf("Goodbye!");
			exit(0);
		}//exits program

	}//infinite while
}
