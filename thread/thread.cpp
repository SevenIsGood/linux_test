#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#include <stdio.h>
#include <stdlib.h>

char vec[100];

void GetPolicy() {
    int policy = sched_getscheduler(getpid());

  switch(policy) {
    case SCHED_OTHER: printf("SCHED_OTHER\n"); break;
    case SCHED_RR:   printf("SCHED_RR\n"); break;
    case SCHED_FIFO:  printf("SCHED_FIFO\n"); break;
    default:   printf("Unknown...\n");
  }
}

void *run(void *arg) {
    GetPolicy();


  printf("Valid priority range for SCHED_OTHER: %d - %d\n",
         sched_get_priority_min(SCHED_OTHER),
         sched_get_priority_max(SCHED_OTHER));

  printf("Valid priority range for SCHED_FIFO: %d - %d\n",
         sched_get_priority_min(SCHED_FIFO),
         sched_get_priority_max(SCHED_FIFO));

  printf("Valid priority range for SCHED_RR: %d - %d\n",
         sched_get_priority_min(SCHED_RR),
         sched_get_priority_max(SCHED_RR));

   struct timespec ts;
   int ret;

   /* real apps must check return values */
   ret = sched_rr_get_interval(0, &ts);

   printf("Timeslice: %lu.%lu\n", ts.tv_sec, ts.tv_nsec);

/*
    sched_param sp;
    sp.sched_priority = 90;
    ret = sched_setscheduler(0, SCHED_RR, &sp);
    if (ret == -1) {
        perror("sched_setscheduler");
        return NULL;
    }
    */

   /* real apps must check return values */
   ret = sched_rr_get_interval(0, &ts);

   printf("Timeslice: %lu.%lu\n", ts.tv_sec, ts.tv_nsec);


    GetPolicy();


    unsigned int i = 0;

    while(1) {
        for (int i = 0; i < 100000000; ++i) 
        {
         vec[i%100] = i;        
         i++;
        }

        sched_yield();
    }
}

void checkRet(int ret) {
    if (ret) {
        printf("ret error: %d, exit...\n", ret);
        exit(0);
    }
}

int main(int argc, char **argv) 
{
#if 0
   struct sched_param param;
   int pid_num = 0;

   param.sched_priority = 80;
   sched_setscheduler(pid_num, SCHED_RR, &param);
#endif

    pthread_attr_t attr;
    pid_t pid;
    pthread_t tid;

    int ret;
    pthread_attr_init(&attr);
    
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    checkRet(ret);


    sched_param sp;
    sp.sched_priority = 99;
    ret =    pthread_attr_setschedparam(&attr, &sp);
    checkRet(ret);

    ret =    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    checkRet(ret);

    pthread_create(&tid, &attr, run, NULL);

    pthread_join(tid, NULL);

    return 0;
}