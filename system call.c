/* 

Name: Meet Vora
 
OBJECTIVE: 
The object of this assignment is to learn about system calls, creating an interface between the operating system and the user
(command interpreter) and implementing an important part of an operating system.


RUN INSTRUCTION :
   gcc -c meetacs1.c for compilation
   gcc meetacs1.o -o meetacs1 for object file
   ./meetacs1 for execution
*/

/* smallsh.h  source code unix process ppt */
//smallsh.h - - defs for smallish command processor

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

#define EOL 1			/* end of line */
#define ARG 2			/* normal arguments  */
#define AMPERSAND 3
#define SEMICOLON 4
#define MAXARG 512		/* max. no. command args */
#define MAXBUF 512		/* max length input line */


#define FOREGROUND 0
#define BACKGROUND 1


/* special character to smallsh : */
static char special [] = {' ', '\t', '&', ';', '\n', '\0'};

/* program buffers and work pointers */
static char inpbuf[MAXBUF], tokbuf[2*MAXBUF], *ptr = inpbuf, *tok = tokbuf;


/* userin source code unix process ppt */
/* The user input program that will print a prompt and wait for a line of input 
from the keyboard */ 


int userin (char *p)
{ 

  int c, count;


/* initialization for later routines */
ptr = inpbuf;

tok = tokbuf;


/* display prompt */
printf("%s" , p);
count = 0;


while(1)

   {



      if ((c = getchar()) == EOF)
           return(EOF);

     if (count < MAXBUF)
          inpbuf[count++]  =   c;

     if (c == '\n' && count < MAXBUF)
        {

            inpbuf[count] = '\0';
            return count;
        }   




      /* if line too long restart */

           if(c=='\n') 
	
  {

   printf("smallsh: input line too long \n");
   count = 0;
  
	
  printf("%s",p);
 
	
 }

       }



    }


/* inarg source code unix process ppt */	
	
int inarg(char c)
{ 

 char *wrk;

 for (wrk = special; *wrk; wrk++)
 {

    if (c == *wrk)
        return (0);
  }



return (1);



}

/* gettok source code unix process ppt */

/* get token, place into tokbuf */

int gettok(char **outptr)
{

  int type;


/* set the outptr string to tok */

*outptr = tok;

/* strip white space from the buffer containing the tokens */
while ( *ptr == ' '|| *ptr == '\t')
      ptr++;


/* set the token pointer to the first token in the buffer */
*tok++ = *ptr;


/* set the type variable depending on the token in the buffer */
switch (*ptr++)

{

  case '\n':

       type = EOL;

       break;


case '&':

     type = AMPERSAND;
    break;


case ';':



     type = SEMICOLON;
     break;



default:


     type = ARG;

    /* keep reading valid ordinary characters */
   while (inarg(*ptr))
          *tok++ = *ptr++;

}

*tok++ = '\0';

return type;

}



/* procline source code unix process ppt */

int procline(void)   /* process input line */

{  

  char *arg[MAXARG + 1];		/* pointer array for runcommand */
  int toktype;					/* type of token in command */

  int narg;						/* number of arguments so far */
  int type;						/* FOREGROUND or BACKGROUND */

  narg = 0;



for (;;)   /* loop forever */

{

    /*  take action according to token type */

   switch(toktype = gettok(&arg[narg]))

   {

     case ARG: if(narg < MAXARG)

                       narg++;

               break;

    case EOL:

    case SEMICOLON:
    
    case AMPERSAND:
         if (toktype == AMPERSAND)
              type = BACKGROUND;
         else

               type = FOREGROUND;
          if(narg != 0)
            {

             arg[narg] = NULL;
             runcommand(arg, type);
          }

          if (toktype == EOL || toktype==SEMICOLON)
              return;

         narg = 0;

         break;

      }



}



}




/* runcommand source code http://homepage.usask.ca/~xil358/mesh.c */

/*The runcommand starts any command processes. It also responsible for changing the
working directory, catching signals and handling pipe line commands. It has an integer
parameter called where. If where is set to the value of BACKGROUND as defined in smallsh.c,
then the waitpid call is omitted and runcommand simply prints the process-id and returns.*/
int runcommand(char **cline, int where)
{

		pid_t pid;
        int status, fd1 ;

 
        int i = 0;
        int j = 0;
        int k = 0;
        int l = 0;
        int m = 0;

        /*for counting the number of pipes in pipe line command*/
        int npipe = 0;

        /*for number of pipes and pipe commands*/
        char *arg[10][MAXARG+1];

        /*used for I/O redirection*/
        char **temp;
        temp = cline; 
       
		 /*check to see if this is a pipe line*/
    	for(i=0;cline[i]!= NULL; i++) 
        {
                if(strcmp(cline[i],"|") == 0)
                break;
        }

		/*handle pipe line starts here*/
        if(cline[i] != NULL) 
        {
		 /*adding the first group of commands (i.e. before the first '|') into the 
          two dimensional arg array*/
		  
          	for(k=0;cline[k] != NULL;k++)
          	{
                	if (strcmp(cline[k], "|") != 0)

                	{
                        	arg[l][m++] = cline[k];
                	}
                	else
                	{
                        	m=0;
                        	l++;
                        	npipe++;
                	}
          	}
        	/*call the join function to handle the pipe line commands*/ 
        pipemng(arg,npipe,where);
        return(0);
        }// end  if
    
      /*if this is not a pipe line command, then check to see if this is a change
        directory command*/
    	else
    	if(strcmp(*cline,"exit")==0)
        exit(0);
    	else
        {
        	if(strcmp(*cline,"cd")==0)
        	{
            		if(cline[1] == NULL) // if nothing after cd,  goes to home directory
                 	chdir(getenv("HOME"));
	            	else
                	{
               			if (chdir(cline[1]) == -1)
                		perror("change directory error");
                	}
            		return(0);
        	}

			 /*fork a child to execute the command*/
  	        switch (pid = fork()) 
        	{
				 /*fork fails*/
                case -1: 
                        perror("small sh");
                        return (-1);
				 /*fork succeeds*/
                case 0:  
                        /*background process will ignore any of the following signals*/
                        if (where==BACKGROUND)
                        {
                         signal(SIGINT, SIG_IGN);
                         signal(SIGQUIT, SIG_IGN);
                        }
                        else
                        {
                        /*foreground process will catch any of the below signals */
                         signal(SIGINT, SIG_DFL);
                         signal(SIGQUIT, SIG_DFL);
                        }
            		for(j=0;temp[j] != NULL; j++)
                        {
                          
                          if(strcmp(temp[j], "<") == 0)  
                          { 
                           
                           temp[j] = NULL;
                           j++;
							/*open file for input with read only permission*/
                           fd1 = open(temp[j], O_RDONLY, 0644); 
                           close(0);  /*close file with status = 0 (i.e. standard input)*/
                           dup2(fd1, 0);
                          }
                          if (strcmp(temp[j], ">") == 0 ) // output redirection 
                          {
                           temp[j++] = NULL;
                           while(strcmp(temp[j], " ")==0)
                           {
                            temp[j] = NULL;
                           j++;
                           } 
                           if(strcmp(temp[j], ">") == 0)// appending command 
                           {  
                             temp[j] = NULL;
                             j++;
                             fd1 = open(temp[j], O_WRONLY|O_APPEND);
                           }
                           else fd1 = open(temp[j], O_WRONLY|O_CREAT, 0644);// redirection 
                           close(1); //close file
                           dup2(fd1, 1);
                          }
               }  /* reffrence of redirection from Stackoverflow.com */
            
                    if(strcmp(*cline,"cls")==0)
            		execvp("clear",cline);
            		else if(strcmp(*cline,"md")==0)
            		execvp("mkdir",cline);
            		if(strcmp(*cline,"del")==0)
            		execvp("rm",cline);
            		if(strcmp(*cline,"copy")==0)
            		execvp("cp",cline);
            		if(strcmp(*cline,"move")==0||strcmp(*cline,"rename")==0)
            		execvp("mv",cline);
            		if(strcmp(*cline,"type")==0)
            		execvp("cat",cline);
            		if(strcmp(*cline,"md")==0)
            		execvp("mkdir",cline);
            		if(strcmp(*cline,"rd")==0)
            		execvp("rmdir",cline);
                    if(strcmp(*cline,"more")==0)
            		execvp("more",cline);
                        // if commands are other than specified DOS command
                        execvp(*cline, cline);
                        perror(*cline);
                        exit(1);
                default:
                        signal(SIGINT, SIG_IGN);
                        signal(SIGQUIT, SIG_IGN);
        }
        
        if (where == BACKGROUND)// if type is background print id
        {
                printf("[Process id %d]\n", pid);
                return (0);
        }
        if (waitpid (pid, &status, 0) == -1)
                return (-1);
        else
                return (status);
        }
}






/*Reference:: code from -- http://homepage.usask.ca/~xil358/mesh.c */
/*The pipemng is a function for processing pipe*/
int pipemng(char *arg[10][MAXARG+1],int npipe,int where)
{
    pid_t pid[npipe+1];         /*pid array of fork child*/
    int i=0,j;        		    /*simply counters*/
    int p[npipe][2], status;    /*two-dimensional array for holding the file descriptors
                                of each pipe*/

    /*make pipes here*/
    for(i=0;i<npipe;i++)
    {
        /*open pipe*/
        if(pipe(p[i])==-1)
           fatal("pipe call in join");
    }
    /*create child to run command*/
    for(i=0;i<=npipe;i++)
    {
        switch(pid[i]=fork()){
        case -1:
              fatal("fork children failed");

        case 0:

              /*handle signals*/                 
              if(where==BACKGROUND)
              {
                 signal(SIGINT,SIG_IGN);
                 signal(SIGQUIT,SIG_IGN);
              }
              else
              {
                 signal(SIGINT,SIG_DFL);
                 signal(SIGQUIT,SIG_DFL);
              }

/*the first command group, make its standard output to go to the first pipe*/     
              if(i==0)
              { 
                dup2(p[i][1],1);

               for(j=0;j<npipe;j++)
                {
/*save file descriptor*/
                    close(p[j][0]);
                    close(p[j][1]);
                }

                execvp(arg[i][0], arg[i]);

/*if execvp returns, error has occured*/
                fatal(arg[i][0]);
             }
           
/*the last command group, make its standard input come from previous pipe*/
             else if (i==npipe)
             {
                dup2(p[i-1][0],0);
               for(j=0;j<npipe;j++)
                {
                    close(p[j][0]);
                    close(p[j][1]);
                }
                execvp(arg[npipe][0],arg[npipe]);
                fatal(arg[npipe][0]);
            }

/*all the other command group, make their standard input from their previous pipes
            and standard output go to the next pipe*/
            else
            {
               dup2(p[i-1][0],0);
               dup2(p[i][1],1);
               for(j=0;j<npipe;j++)
               {
                   close(p[j][0]);
                   close(p[j][1]);
               }
               execvp(arg[i][0],arg[i]);
               fatal(arg[i][0]);
            }

         }      /*end of the switch*/

     }          /*end for*/


/*parent code here*/   
    for(j=0;j<npipe;j++)
    {
       close(p[j][0]);
       close(p[j][1]);
    }

    if (where==BACKGROUND)
    {
       for(j=0;j<=npipe;j++)
       {
            if (pid[j]>0)
                printf("[Process id %d]\n",pid[j]);
            else
                 sleep(1);
       }
       return(0);
   }

/*parent wait here*/
   while(waitpid(pid[npipe],&status,WNOHANG)==0)
         sleep(1);

       return(0);

} 
/*end of the pipemng*/


/* reffrence from stackoverflow  */
int fatal(char *s)
{
        perror(s);
        exit(1);
}

/* reffrence from unix process ppt */
main()
{
    char *prompt = "Command>";
    while (userin(prompt)!=EOF)
                procline();
}
