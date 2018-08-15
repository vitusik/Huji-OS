#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/types.h>
       #include <unistd.h>

int main(int argc, char* argv[])
{
    int fd = open("/tmp/FBR_cache0.txt",O_DIRECT | O_SYNC);
    lseek(fd,5,0);
    char test[1];
    read(fd,test,1);
    return 0;
}