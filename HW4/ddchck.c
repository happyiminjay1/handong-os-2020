#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct Thread{
    int thread_used;
    int thread_id;
    int hold_mutex[10];
    int hold_mutex_num;
}Thread;

int mutex[10];
int edge[10][10];
Thread threads[10];
int thread_num = 0;
int mutex_num = 0;

int FindThreadExist(int k)
{
    int check = 0;
    int index;
    if(thread_num>0)
    {
        for(int i = 0; i<thread_num; i++)
        {
            if(threads[i].thread_id==k)
            {
                check = 1;
                index = i;
                break;
            }
        }
    }
    if(check == 0)
    {
        threads[thread_num].thread_id=k;
        thread_num++;
    }
    if(check == 0) return thread_num-1;
    else return index;
}

int FindMutexExist(int k)
{
    int check = 0;
    int index;
    if(mutex_num>0)
    {
        for(int i = 0; i<mutex_num; i++)
        {
            if(mutex[i]==k)
            {
                check = 1;
                index = i;
                break;
            }
        }
    }
    if(check == 0)
    {
        mutex[mutex_num]=k;
        mutex_num++;
    }
    if(check == 0) return mutex_num-1;
    else return index;
}

void ADD(int i, int j)
{
    edge[i][j] = 1;
    printf("ADD(%d %d)\n",i,j);
}

void FREE(int i, int j)
{
    edge[i][j] = 0;
}

int 
main ()
{
    for(int i = 0; i<10; i++)
    {
        threads[i].thread_used = 0;
        threads[i].thread_id = 0;
        threads[i].hold_mutex_num = 0;
    }
    
	int fd = open(".ddtrace", O_RDONLY | O_SYNC) ;

	while (1) {
        int thread_id, mutex_id;
        char context[100];
		char s[128] ;
		int len ;
		if ((len = read(fd, s, 128)) == -1)
			break ;
		if (len > 0)
        {
            printf("%s\n", s) ;
            sscanf(s,"thread : %d mutex %d %s\n",&thread_id,&mutex_id,context);
            printf("%d %d %s\n",thread_id,mutex_id,context);
            
            if(!strcmp(context,"locked")){
                    int index = FindThreadExist(thread_id);
                    int mutex_id_index = FindMutexExist(mutex_id);
                    if(threads[index].hold_mutex_num>0)
                    {
                        for(int i = 0; i<threads[index].hold_mutex_num; i++)
                        {
                            ADD(threads[index].hold_mutex[i],mutex_id_index);
                        }
                    }
                    threads[index].hold_mutex[threads[index].hold_mutex_num] = mutex_id_index;
                    ++threads[index].hold_mutex_num;
            }
            else{
                int mutex_id_index = FindMutexExist(mutex_id);
                int index = FindThreadExist(thread_id);
                for(int i = 0; i<10 ; i++)
                {
                    FREE(i,mutex_id_index);
                }
                printf("FREE(*, %d)\n",mutex_id_index);
                int temp;
                for(int i = 0; i<10; i++)
                {
                    if(threads[index].hold_mutex[i] == mutex_id_index)
                    {
                        temp = i;
                        break;
                    }
                }
                for(int i = temp; i<9 ;i++)
                {
                    threads[index].hold_mutex[i] = threads[index].hold_mutex[i+1];
                }
                threads[index].hold_mutex_num--;
                
            }
            for(int i = 0; i<10; i++)
            {
                for(int j = 0; j<10; j++)
                {
                    printf("%d ",edge[i][j]);
                }
                printf("\n");
            }
            
        }
    
	}
	close(fd) ;
	return 0 ;
}
