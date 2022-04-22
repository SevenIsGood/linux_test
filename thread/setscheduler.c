 #include <sched.h>
 main ()
 {
   struct sched_param param;
   int pid_num = 0;

   param.sched_priority = 80;
   sched_setscheduler(pid_num, SCHED_FIFO, &param);
   while (1) 
      ;
 }