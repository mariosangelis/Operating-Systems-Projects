#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc,char *argv[]) {
	
	unlink("./mountdir/file4.txt");

	return(0);
}
