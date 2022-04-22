#include <sys/fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>



int main(int argc, char **argv) {
  const char *netPath = "/sys/class/net";
   DIR *dir = opendir(netPath);

   dirent *dirp;
   
   while((dirp = readdir(dir)) != NULL) {
       if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
           continue;
       }

       printf("netcard: %s\n", dirp->d_name);

       if(strstr(dirp->d_name, "enp")  != NULL)
           printf("netcard: physics card %s\n", dirp->d_name);
   }

   closedir(dir);

	return 0;
}
