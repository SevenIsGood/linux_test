#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <vector>

int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cout << "usage: memalloc G\r\n\tG: gigabyte\r\nexample: memalloc 0.3" << std::endl;
        return 0;
    }

    double G = atof(argv[1]);

    const int MaxNewSize = G * 1000 * 1000 * 1000;
    const int NewSizePer = 412;

    const int AnotherSize = 13;

    long long total = 0;

    std::vector<char *> cs;
    while (total < MaxNewSize)
    {
//        char *p = new char[NewSizePer];
        char *p = (char *)malloc(NewSizePer);

        if (p)
        {
            memset(p, 0, NewSizePer);
            total += NewSizePer;
            cs.push_back(p);
        }
#if 1
        p = (char *)malloc(AnotherSize);
        if (p) {
            memset(p, 0, AnotherSize);
            total += AnotherSize;
        }
#endif
        std::cout << "new " << NewSizePer << " data: " << (p ? "ok" : "failed") << ", total " << (total / 1024 / 1024) << " MB" << std::endl;

        if (!p)
            break;
    }

    const int SleepTime = 30;
    std::cout << "ready to sleep " << SleepTime << " seconds" << std::endl;
    sleep(SleepTime);

    std::cout << "ready to free:" << std::endl;
    while (!cs.empty())
    {
        char *p = cs.back();
        cs.pop_back();
        total -= NewSizePer;
        free(p);

        std::cout << "free node, left " << (total / 1024 / 1024) << "MB" << std::endl;
    }

    while(1) {
        std::cout << "sleep forever" << std::endl;
        sleep(100);
    }

    return 0;
}