#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define  BUFF_SIZE   1024


int m[50][50];
int used[50] = {0};
int deliver[50] = {0};
int min_route[50] = {0};
int min = -1;
int sum;
int child_pro_num;
int cal_end_point = 0;
int cal_end_point2 = 0;
int length = 0;
pid_t child_pid[12];
int child_pid_used[12] = {0};
int minimum = 1000000000;
int min_deliver[50] = {0};
int total_num = 0;
int t_count = 0;

//*************************Pipe setting**************************//
int   pipes_child[2];
char  buff[BUFF_SIZE];

void Listen()
{
  memset( buff, 0, BUFF_SIZE);
  if(read(pipes_child[0], buff, BUFF_SIZE)>0)
  {
    int counter = 0;
    printf( "부모 프로세스: %s\n", buff);
    int num = 0, len;
    char * line = buff;
    int count = 0;
    int get_min = 0;
    int check = 0;
    while ( sscanf( line, "%d%n", &num, &len) == 1 ) {
      if(get_min==0) {
        get_min = 1;
        if(min==-1) min = num;
        else{
          if(min>num) min = num;
          else{
            check = 1;
          }
        }
      }
      else{
        if(check == 0) min_route[count] = num;
        count++;
      }
      line += len;
    }
    printf("t_count : %d\n",num);
    total_num += num;
  }
}

void handler_ctr_c (int sig)
{
		printf("Program terminating\n") ;
    printf("finding best solution...\n") ;
		for(int i = 0; i<12 ; i++)
    {
      if(child_pid_used[i]==1)
      {
        kill(child_pid[i],SIGTERM);
        sleep(1);
      }
    }
    while(1){
      int endpoint = 0;
      for(int i = 0; i<12 ;i++)
      {
        if(child_pid_used[i]==1) endpoint = 1;
      }
      if(endpoint == 0)
      {
        printf("All execution complete!\n");
        printf("Final Solution : \n");
        printf("Path - ");
        for(int i = 0; i<length; i++)
        {
          printf("%d ",min_route[i]);
        }
        printf("\n");
        printf("The length of shortest path is %d\n",min);
        printf("The length of shortest path is %d\n",min);
        if(total_num<2100000000)
        printf("Number of checked route is : %d\n",total_num);
        else{
          printf("Number of route exceed Int range\n");
        }
        break;
      }
    }
    exit(0);
}

void sigchld_handler(int sig)
{
	pid_t child ;
	int exitcode ;
	child = wait(&exitcode) ;
	printf("> child process %d is terminated with exitcode %d\n",
			child, WEXITSTATUS(exitcode)) ;
  for(int i = 0; i < child_pro_num; i++)
  {
    if(child_pid[i] == child)
    {
      Listen();
      child_pid_used[i] = 0;
    }
  }
}

void handler(int sig)
{
	while(1);
}

void handler2(int sig)
{
	printf("Program ended by Parent\n");
  printf("Last Searched Route : ");
  for(int i = 0; i < length; i++)
  {
    printf("R%d",deliver[i]);
  }
  printf("\n");
  //******************Pipe_s******************
  close(pipes_child[0]);
  int index = 0;
  index += sprintf(&buff[index], "%d ",minimum);
  for (int i=0; i<length; i++)
    index += sprintf(&buff[index], "%d ", min_deliver[i]);
  index += sprintf(&buff[index], "%d ",t_count);
  write(pipes_child[1], buff, strlen(buff));
  memset( buff, 0, BUFF_SIZE);
  close(pipes_child[1]);
  //******************Pipe_e******************

  exit(0);
}

void init(int * deliver, int start, int end)
{
  for(int i = start; i<=end; i++)
  {
    for(int j = end; j>=0; j--)
    {
      if(used[j]==0)
      {
        used[j] = 1;
        deliver[i] = j;
        break;
      }
    }
  }
}

void givenextinput1(int * deliver, int index)
{
  if(index==0){
    printf("program ended\n");
    cal_end_point = 1;
  }
  else{
      int temp;
      temp = deliver[index];
      used[temp] = 0;
      while(1)
      {
        deliver[index] = deliver[index]-1;
        if(deliver[index]<=-1){
          givenextinput1(deliver,index-1);
          deliver[index] = length-2;
        }
        if( (used[deliver[index]]==0) & (temp!=deliver[index]))
        {
            used[deliver[index]] = 1;
            break;
        }
      }
  }
}

void givenextinput2(int * deliver, int index, int end)
{
  if(index==end){
    cal_end_point2 = 1;
  }
  else{
      int temp;
      temp = deliver[index];
      used[temp] = 0;
      while(1)
      {
        deliver[index] = deliver[index]-1;
        if(deliver[index]<=-1){
          givenextinput2(deliver,index-1,end);
          deliver[index] = length-2;
        }
        if( (used[deliver[index]]==0) & (temp!=deliver[index]))
        {
            used[deliver[index]] = 1;
            break;
        }
      }
  }
}

int main(int argc, char **argv) {
  signal(SIGCHLD, sigchld_handler) ;
	int i, j, t ;
  if ( -1 == pipe(pipes_child))
  {
     perror( "파이프 생성 실패");
     exit(1);
  }
  FILE * fp = fopen(argv[1], "r") ;
	char str_read[512];
  fgets(str_read,512,fp);
	printf("읽어들인 문자열 : %s \n", str_read);
	for(int i = 0; i < 512; i++)
	{
		if(str_read[i]==' ') length++;
		if(str_read[i]=='\0') break;
	}
  fclose(fp) ;

	printf("legnth : %d",length);

	fp = fopen(argv[1], "r") ;
  if(argv[1]==NULL|argv[2]==NULL)
  {
    printf("file path and number of children process required!\n");
    if(atoi(argv[2])>12 | atoi(argv[2])<1) printf("Incorrect child process");
    exit(0);
  }
  printf("argv[2] : %d\n",atoi(argv[2]));
  child_pro_num = atoi(argv[2]);
	for (i = 0 ; i < length ; i++) {
		for (j = 0 ; j < length ; j++) {
			fscanf(fp, "%d", &t) ;
			m[i][j] = t ;
		}
	}
	fclose(fp) ;

  for(int i = 0; i < length-12; i++)
  {
    deliver[i] = length-1-i;
    used[length-1-i] = 1;
  }
//*************************Pre Setting **************************//

//*************************Child and Parent Process**************************//
  pid_t term_pid ;
  int exit_code ;
  int firstTime = 0;
  int firstTime2 = 0;
  signal(SIGINT, handler_ctr_c) ;

  while(!cal_end_point)
  {
      for(int i = 0; i < child_pro_num; i++) {
        if(child_pid_used[i] == 0) {

          if(firstTime == 0) firstTime = 1;
          else{givenextinput1(deliver,length-13);} //

          child_pid_used[i] = 1;
          child_pid[i] = fork();
//************************* allocate child subtask **************************//
          if(child_pid[i]==0) {
            signal(SIGINT, handler);
            signal(SIGTERM, handler2) ;
            //signal(SIGTERM, handler);
            init(deliver,length-12,length-1);
            printf("running..\n");
            sum = 0;
            printf("%d ",deliver[0]);
            for(int i = 1; i<length-12; i++)
            {
              printf("%d ",deliver[i]);
              sum += m[deliver[i-1]][deliver[i]];
            }
            printf("sum : %d\n",sum);
            int t_sum = sum;
            while(!cal_end_point2)
            {
//************************* child subtask 12!**************************//
              t_sum = sum;
              t_count++;
              if(firstTime2==0) firstTime2 =1;
              else {givenextinput2(deliver,length-1,length-13);}
              for(int i = length-12; i<length; i++)
              {
                t_sum += m[deliver[i-1]][deliver[i]];
              }
              t_sum += m[deliver[length-1]][deliver[0]];
              if(minimum>t_sum)
              {
                minimum = t_sum;
                memcpy(min_deliver,deliver,sizeof(deliver));
              }
            }

            //******************Pipe_s******************
            close(pipes_child[0]);
    				int index = 0;
						index += sprintf(&buff[index], "%d ",minimum);
						for (int i=0; i<length; i++)
        			index += sprintf(&buff[index], "%d ", min_deliver[i]);
            index += sprintf(&buff[index], "%d ",t_count);
            write(pipes_child[1], buff, strlen(buff));
            memset( buff, 0, BUFF_SIZE);
						close(pipes_child[1]);
            //******************Pipe_e******************
            exit(0);
          }
        }
      }
  }
//************************* Parent waiting and allocate new child subtask **************************//

  while(1){
    int endpoint = 0;
    for(int i = 0; i<12 ;i++)
    {
      if(child_pid_used[i]==1) endpoint = 1;
    }
    if(endpoint == 0)
    {
      printf("All execution complete!\n");
      printf("Final Solution : \n");
      printf("Path - ");
      for(int i = 0; i<length; i++)
      {
        printf("%d ",min_route[i]);
      }
      printf("\n");
      printf("The length of shortest path is %d\n",min);
      if(total_num<2100000000)
      printf("Number of checked route is : %d\n",total_num);
      else{
        printf("Number of route exceed Int range\n");
      }
      break;
    }
  }
}
