/*
Fadel Alshammasi
11/10/19

This is a Unix shell written in C that has many functionalities such as internal commands, piping, executing multiple commands, running in background...etc

*/

#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include <sys/stat.h>
#include <fcntl.h>
  
  
extern char **environ;
int runInBackGround=0;
char *allCmd[] = {
  "quit",
  "cd",
  "help",
  "clr",
  "pause",
  "environ",
  "dir",
  "time"
};

/*
This function reads an input from standard in
@param no paramter
@return a char pointer (string)
*/
char *readIn(void)
{
  int c;
  int pos = 0;
  char *buffer = malloc(BUFSIZ * sizeof(char));
  

  if (!buffer) {
    fprintf(stderr, "Error\n");
    exit(1);
  }

  while (1) {
    c = getchar();
    if (c == EOF || c == '\n') {
      buffer[pos] = '\0';
      return buffer;
    } else {
      buffer[pos] = c;
    }
    pos++;
  }
}

/*
A function to check if there's something running in the backround and if a existed/killed signal is trigreed
@param no paramater
@return doesn't return anything(void)
*/
void checkBacground(){
	pid_t child_pid;
	int status;
	int i=0;
	for(i=0; i<10; i++){
		child_pid = waitpid(-1, &status, WNOHANG);
		if(child_pid>0){
			if(WIFEXITED(status) != 0){
				printf("%d Child was exited\n",child_pid);
			}else if(WIFSIGNALED(status)!=0){
				printf("%d Child was killed\n",child_pid);
			}
		}
	}
}

/*
A function to execute the commands with the use of forking and also doesn't wait for the process to finish if it's running in the background
@param string of commands 
doesn't return anything (void)
*/
void execute(char** arr) { 
  
 pid_t child_pid, parent_pid, wait_id;
    parent_pid=getpid();
    
  if ((child_pid=fork()) < 0){
    perror("error forking the child process");
  }
 else if(child_pid==0){
    if(execvp(arr[0], arr) < 0) {
      perror (" ");
  }
  exit (0);
}
  else{
      checkBacground();
    if (runInBackGround==0){
        checkBacground();
        waitpid(child_pid,NULL,0);
    }
    else if(runInBackGround==1){
       checkBacground();
    }
    
 }
} 
  

/*
A function to execute the commands that include pipe (|)
@param command after parsing the |, and another param for parsed commands(invidually)
Doesn't return anything (void)
*/
void executePipe(char** str, char** parsedpipe) { 

    pid_t child1_id;
    pid_t child2_id; 
    int pipe1[2];  

  
    if (pipe(pipe1) < 0) { 
        printf("\n Error in piping"); 
        return; 
    } 
     
    if ((child1_id = fork()) < 0) { 
        printf("\n error"); 
        return; 
    } 
  
    if (child1_id == 0) { 
    
        dup2(pipe1[1], STDOUT_FILENO); 
        close(pipe1[0]);
        close(pipe1[1]); 
  
        if (execvp(str[0], str) < 0) { 
            printf("\n Error "); 
            exit(EXIT_SUCCESS); 
        } 
    } else { 
      
        if ((child2_id=fork()) < 0) { 
            printf("\n Error in forking"); 
            return; 
        } 
  
        if (child2_id == 0) { 
            
            dup2(pipe1[0], STDIN_FILENO); 
            close(pipe1[1]);
            close(pipe1[0]); 
            if (execvp(parsedpipe[0], parsedpipe) < 0) { 
                printf("\n Error"); 
                 exit(EXIT_SUCCESS); 
            } 
        } else { 
            waitpid(child2_id,NULL,0); 
            waitpid(child1_id,NULL,0); 
        } 
    } 
} 
  
/*
A function that provides the functionaites of the internal commands(cd,ls,...etc)
@param the parsed input command
@return 1 on sucess and 0 on faliure
*/
int suportCmd(char** parsed) { 
    
    int NoOfOwnCmds = 8;
    int i;
    int tracker=0;
  
    for (i = 0; i < NoOfOwnCmds; i++) { 
        if (strcmp(parsed[0], allCmd[i]) == 0) { 
            tracker= i + 1; 
            break; 
        } 
    } 
  
    if (tracker ==1){
        exit(0); 
    }
    if (tracker ==2){
        chdir(parsed[1]); 

         char cwd[1024]; 
       const char *tmp= getcwd(cwd, sizeof(cwd)); 
        setenv("PWD",tmp,1);     //change the PWD enviroment
        return 1; 
    }
   if (tracker ==3){
        system("man ls"); 
        return 1; 
   }
    if (tracker ==4){ 
        system("clear"); 
        return 1;
    }
    if (tracker ==5){
        int c=getchar();
		while(c != '\n'){
			c=getchar();
	}
    return 1;
}
    if(tracker ==6){
        int i=0;
        while(environ[i]){
            printf("%s\n", environ[i++]);
        }
        return 1;
    }
    if (tracker==7){
     system("ls");
     return 1;
}

if(tracker==8){
    system("time");
    return 1;
}
    return 0;
 }
  

  /*
    A function to parse the commands that have ("|") for piping
    @param the original string with the pipe, and an array of string for the two or more commands after strepping out the pipe
    @return 1 on sucess and 0 on faliure
  */
int parsePipe(char* str, char** strpiped) 
{ 
    int i; 
    for (i = 0; i < 2; i++) { 
        strpiped[i] = strsep(&str, "|"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL){ 
        return 0;
    } 
    else { 
        return 1; 
    } 
} 

/*
    A function to parse the space in commands
    @param the original string with the space, and an array of string for the two or more commands after strepping out the space
    @return nothing as it's void
  */  
void parseSpace(char* str, char** parsed) 
{ 
    int i; 
  
    for (i = 0; i < BUFSIZ; i++) { 
        parsed[i] = strsep(&str, " "); 
  
        if (parsed[i] == NULL) 
            break; 
        if (strlen(parsed[i]) == 0) 
            i--; 
    } 
} 
 
/*
    A function to help distinguish if the command contains a pipe or not 
    @param 
    @return  an int flag to help know if the command is just a normal command or if it has piping
  */
int processString(char* str, char** parsed, char** parsedpipe) { 

    int piped = 0; 
    char* fid[2]; 
  
    piped = parsePipe(str, fid); 
  
    if (piped) { 
        parseSpace(fid[0], parsed); 
        parseSpace(fid[1], parsedpipe); 
  
    } else { 
  
        parseSpace(str, parsed); 
    } 
  
    if (suportCmd(parsed)) {
        return 0; 
    }
    else{
        return piped + 1;
    } 
} 
  

 /*
    A function to parse the semi colon AND a new line to help with the batch file function 
    @param the original string with multiple commans sepreated by ; AND \n
    @return an array of commands parsed 
  */
char **parse_lineAndSem(char *line){

  int bufsize = BUFSIZ, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  token = strtok(line, ";\n");
  while (token != NULL) {
    tokens[position] = token;
    position++;

    token = strtok(NULL, ";\n");
  }
  tokens[position] = NULL;
  return tokens;
}

/*
    A function to parse the semi coloun in commands to handle multiple commands
    @param the original string with multiple commans sepreated by ;
    @return an array of commands without the semi colun 
  */
char **parse_sem(char *line){

  int bufsize = BUFSIZ, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  token = strtok(line, ";");
  while (token != NULL) {
    tokens[position] = token;
    position++;

    token = strtok(NULL, ";");
  }
  tokens[position] = NULL;
  return tokens;
}

/*
    Another helper executation function to handle the execution of multiple commands
    @param the original multiple commands seprated 
    @return void so nothing
  */
void multipleCmds(char** command) {
    char** cmd=malloc(1000 * sizeof(char *));
    
    while (*command != NULL) {
        parseSpace(*command, cmd);
        suportCmd(cmd);
        execute(cmd);
        command++;
    }
}


/*
Helper function to help reading the commands from the batch file and storing them in an array of strings so we could execute
@param batchfile: the name of the file to be read
@return void 
*/
void batchHelper(char * batchfile){ 

        FILE * file=fopen(batchfile, "r");
        fseek(file, 0, SEEK_END); 
        size_t size = ftell(file);
        fseek(file, 0, SEEK_SET); 

        char * line = malloc(size);

        fread(line, 1, size, file);
        
        char **tmp=parse_lineAndSem(line);
       
      //printf("%s\n",tmp[1]); 
       multipleCmds(tmp);    

        free (line);
        free(tmp);
        fclose(file);
        
    }



/*
Main method to run the program
@param argc: number of command line arguments, argv: stores the actual arguments
@return 0 if sucessful 
*/
int main(int argc, char **argv) { 


    if (argv[1] != NULL) {
        batchHelper(argv[1]);
    }

    else{

    char *input=malloc(BUFSIZ * sizeof(char)); 
    char *parsedArgs[BUFSIZ]; 
    char* parsedArgsWithPipe[BUFSIZ]; 
    char cwd[BUFSIZ] ; 
    int isThrePipeFlag = 0; 
    int i;


  label:
    while (1) {
        checkBacground(); 
       
    char *tmp= getcwd(cwd, sizeof(cwd));  
    const char *tmp2="$ ";
    tmp2=strcat(tmp,tmp2);
        printf("%s",tmp2);
        input=readIn(); //read input from console
        setenv("SHELL", "home/fadelsh/Fadel_Alshammasi_Project1/myshell" ,1);
         checkBacground(); 
        int len=strlen(input);
   

//check if there's an & at the end for background processing 
        if(strcmp ("&", &input[len-1])==0){
            input[len-1]=0;
             runInBackGround=1;
        }
        else
        {
            runInBackGround=0;
        }        
        checkBacground(); 

    int hasSemiCol=0;

//checking for ; to handle multiple commands

    for (i=0; i<BUFSIZ;i++){
        if(input[i]==';'){
            hasSemiCol=1;
            break;
        }
    }


int hasRedirection=0;
 for (i=0; i<BUFSIZ;i++){
        if(input[i]=='>'|| input[i]=='<'){
            hasRedirection=1;
            break;
        }
    }

if (hasRedirection==1)
{
   
   system(input);
   parseSpace(input,parsedArgs);
   suportCmd(parsedArgs);
   execute(parsedArgs);
    
}

//execute multiple commands then go back to take more commands
    if (hasSemiCol==1){
           char **tmp= parse_sem(input);
               multipleCmds(tmp);
               free(tmp);
               goto label;
         }

// if no multiple commands,
        isThrePipeFlag = processString(input, parsedArgs, parsedArgsWithPipe); 
        checkBacground(); 
         // if there's no pipe
        if (isThrePipeFlag == 1){
             checkBacground(); 
             //if there's an &
            if(runInBackGround==1){
                checkBacground(); 
                 execute(parsedArgs);
                 continue;

            } 
            else if(runInBackGround==0){
                checkBacground();
                 execute(parsedArgs); 
            }
           
    }  
    // if there's a pipe
        if (isThrePipeFlag == 2) {
             checkBacground();
            executePipe(parsedArgs, parsedArgsWithPipe); 
        }
         checkBacground(); 
   
}
  
  }
  return 0; 
}

