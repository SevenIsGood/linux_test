#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#define CORE_SIZE   1024 * 1024 * 500
int main()
{
#if 1
    struct rlimit rlmt;
    if (getrlimit(RLIMIT_CORE, &rlmt) == -1) {
        return -1;
    }
    printf("Before set rlimit CORE dump current is:%d, max is:%d\n", (int)rlmt.rlim_cur, (int)rlmt.rlim_max);

    rlmt.rlim_cur = (rlim_t)CORE_SIZE;
    rlmt.rlim_max  = (rlim_t)CORE_SIZE;

    if (setrlimit(RLIMIT_CORE, &rlmt) == -1) {
	printf("settlimit error: %s\n", strerror(errno));
        return -1;
    }

    if (getrlimit(RLIMIT_CORE, &rlmt) == -1) {
        return -1;
    }
    printf("After set rlimit CORE dump current is:%d, max is:%d\n", (int)rlmt.rlim_cur, (int)rlmt.rlim_max);
#endif
    /*测试非法内存，产生core文件*/
    int *ptr = NULL;
    *ptr = 10;

    return 0;
}



