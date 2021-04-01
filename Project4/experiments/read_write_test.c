#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc,char *argv[]) {

	int fd1,ret;
	int fd2;
	char buf[8192];


	fd1=open("./mountdir/file1.txt",O_RDWR|O_CREAT,S_IRWXU);
	if(fd1==-1) {
		printf("Error with open().\n");
	}
	fd2=open("./mountdir/file2.txt",O_RDWR|O_CREAT,S_IRWXU);
	if(fd2==-1) {
		printf("Error with open().\n");
	}
	//Read file1.txt
	ret=read(fd2,buf,8192);
	printf("ret=%d\n",ret);
	write(fd1,buf,8192);
	write(fd1,buf,8192);
	close(fd1);
	close(fd2);

	return(0);
}
