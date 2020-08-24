#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

static int fd;


/*int pthread_create (pthread_t  *  thread, const pthread_attr_t * attr,
                                        void * (*start)(void *), void * arg)
{
    fd = open(".ddtrace", O_WRONLY | O_SYNC);
    char * error ;
    int (*pthread_create_orig)(pthread_t *, const pthread_attr_t *,
                                        void *(*) (void *), void *);
                            
    pthread_create_orig = dlsym(RTLD_NEXT, "pthread_create") ;
    if ((error = dlerror()) != 0x0)
        exit(1) ;

    int ptr = pthread_create_orig(thread, attr, start, arg);

                                                                
    char buf[50] ;
    snprintf(buf, 50, "thread created id : %d\n",(int)thread) ;
    fputs(buf, stderr) ;
    
    int i = 0 ;
    i = strlen(buf);
    buf[i - 1] = 0x0 ;
    for (int i = 0 ; i < 128 ; ) {
        i += write(fd, buf + i, 128) ;
    }
                                                                            
    return ptr ;
}*/

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
        fd = open(".ddtrace", O_WRONLY | O_SYNC);
        char *error;
        int (*real_pthread_mutex_lock)(pthread_mutex_t *mutex);
        real_pthread_mutex_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
                    
        if( (error = dlerror()) != NULL)
        {
            fputs(error, stderr);
            exit(1);
        }
        pid_t x = syscall(__NR_gettid);
                            
        char buf[50] ;
        snprintf(buf, 50, "thread : %d mutex %d locked\n",x,(int)mutex) ;
        fputs(buf, stderr) ;
    
        int i = 0 ;
        i = strlen(buf);
        buf[i - 1] = 0x0 ;
        for (int i = 0 ; i < 128 ; ) {
            i += write(fd, buf + i, 128) ;
        }
          
                                        
        return real_pthread_mutex_lock(mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
        fd = open(".ddtrace", O_WRONLY | O_SYNC);
        char *error;
        int (*real_pthread_mutex_unlock)(pthread_mutex_t *mutex);
        real_pthread_mutex_unlock = dlsym(RTLD_NEXT, "pthread_mutex_unlock");
                    
        if( (error = dlerror()) != NULL)
        {
            fputs(error, stderr);
            exit(1);
        }
        pid_t x = syscall(__NR_gettid);

        char buf[50] ;
        snprintf(buf, 50, "thread : %d mutex %d unlocked\n",x,(int)mutex) ;
        fputs(buf, stderr) ;
    
        int i = 0 ;
        i = strlen(buf);
        buf[i - 1] = 0x0 ;
        for (int i = 0 ; i < 128 ; ) {
            i += write(fd, buf + i, 128) ;
        }
          

        return real_pthread_mutex_unlock(mutex);
}




  
