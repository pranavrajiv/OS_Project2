#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


void error(char* out){
	printf("ERROR AT: %s\n", out);
	char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
}

/////////////////////
//BUILT IN COMMANDS//
/////////////////////
int built_in_cd(char **args);
int built_in_pwd();
int built_in_exit();

char *built_in_names[] = {
	"cd",
	"pwd",
	"exit",
};

int (*built_in_comm[]) (char**) = {
	&built_in_cd,
	&built_in_pwd,
	&built_in_exit,
};

int num_built_ins(){
	return sizeof(built_in_names)/sizeof(char*);
}

int built_in_cd(char **args){
	//printf("args: %s\n", args[0]);
	if(args[0] == NULL){
		chdir(getenv("HOME"));
	}
	else{
		char* BUFF;
		BUFF = getcwd(BUFF, 4096);
		char* path = malloc((strlen(args[0]) + strlen(BUFF) + 30)*sizeof(char));
		//printf("BUFF: %s\n ", BUFF);
       		strcpy(path, BUFF);
        	strcat(path, "/");
        	strcat(path, args[0]);
		//printf("path: %s\n ", path);
		if(chdir(path) != 0){
			error("built_in_cd");
		}
	}
	return 1;
}

int built_in_pwd(char **args){
	char* parentDir;
	char* parentPntr;
	parentPntr = getcwd(parentDir, 4096);
	if(parentPntr == NULL){
		error("built_in_pwd");
	}
	else{
		printf(parentPntr);
        char* newLine = "\n";
		printf(newLine);
	}
	return 0;
}

int built_in_exit(char **args){
	//ToDo: KILL BACKGROUND PROCCESSES
	return 0;
}

char *readIn(void){
	int pos = 0;
	int charIn;
	char *text = malloc(sizeof(char)*128);
	if(!text){
		error("built in readIn");	
	}
	fgets(text, 128, stdin);
	if(!text){
		error("second built in read in");
	}
	if(strlen(text) > 1){
		//nulls out the \n character	
		//text[strlen(text)-1]=0;
	}

	//printf("READ PRINT: %s\n", text);
	return text;
}

char **getArgs(char *line){
	
	char **arguments = malloc(sizeof(char*)*128);
	char *argument;
	int argNum = 0;
	if(!arguments){
		error("getArgs");
	}
	
	argument = strtok_r(line, " \t", &line);
	while(argument != NULL){
	//	printf("arg: %s\n", argument);
		arguments[argNum] = argument;
		argNum++;
		//printf("TOKEN PRINT: %s\n",argument);
		argument = strtok_r(NULL, " \t",&line);
	}
	arguments[argNum++] = NULL;
		
	return arguments;
}

int run(char **args){
	int status;
	pid_t pid, wpid;
	int built_itr = 0;
        int (*built_in_runner) (char**);
	while(built_itr < num_built_ins() || built_itr < 0){
	//	printf("ARGS: %s, NAMES: %s\n", args[0], built_in_names[built_itr]);
      		if(!strcmp(args[0], built_in_names[built_itr])){
	//		printf("IN THIS");
      			built_in_runner = *built_in_comm[built_itr];
 	     		return(built_in_runner(&args[1]));
                }
	        built_itr++;
	}

	pid = fork();
	//CHILD
	if(pid == 0){
		
		if(execvp(*args, args)<1){
			error("run");
		}
		exit(0);
	}
	//ERROR
	else if(pid < 0){
		error("2nd run");	
	}
	else{
		wpid = waitpid(pid, &status, WUNTRACED);
	}
}

char* reformatInput(char *argLine){
	int i = 0;
    //variable which stores if its an input or an output operator
    char flag='\0';
	size_t fileLen = sizeof(argLine);
        char *fileText = malloc(fileLen+1);

	while(i+1 < strlen(argLine)){
		//printf("ARGS OF I : %s\n", args[i]);
		if(argLine[i] == '>' || argLine[i] == '<'){
			flag=argLine[i];
            //printf("i: %s /END\n", args[i+2]);
			//stdout = fopen(args[i+2], "a");
			int temp = i-1;
			i = i+2; //to move past both > and ' '.
			int j = 0;
			printf("i: %d\n",i);
			while(i+1 < strlen(argLine)){
				printf("WHILE,IF  %c", argLine[i]);
				if((argLine[i] - 32 != 0)&&(argLine[i] - 9 != 0)){
					printf("\t %c\n\n", argLine[i]);
					fileText[j] = argLine[i];
					printf("\n\nLINE: %s\n\n",argLine);
					j++;
				}
				printf("-_-%s-_-\n", fileText);
				i++;
			}

            if(flag - 0 != 0)
            {
            
                if(flag == '>')
			    {
                   ; //fileW = fopen(argv[4],"w");
                }
                else
			    {
                    ;
                }

            }   

	        while(temp < i || argLine[i] - 9 == 0 || argLine[i] - 32 == 0){
		        argLine[i]=0;
		        i--;
	        }
			printf("FINAL LINE: %s", argLine);
			break;
		}
		i++;
	}
	return fileText;
}

int main(int argc, int argv){
	FILE* fileR = NULL;
    FILE* fileW = NULL;
    char* fileOut;
	int runNum = 1;
	int loop = 1;
	char *lineIn;
	char **args;

	do{
		printf("mysh (%d)> ",runNum);
		lineIn = readIn();
		fileOut = reformatInput(lineIn);
		if(lineIn[strlen(lineIn)-1] - 10 == 0){
			lineIn[strlen(lineIn)-1]=0;
		}
        
		if(strlen(lineIn) > 1){
        FILE *saved = stdout;
        stdout = fopen(fileOut, "a");
		args = getArgs(lineIn);
		loop = run(args);
        //fclose(stdout);
       //stdout = saved;
		}
		//fclose(stdout);
		//stdout = savedOut;
		runNum++;
	}while(loop);

	free(lineIn);
	free(args);
}
