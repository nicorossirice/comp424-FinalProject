#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

static char receive[256];

int main(){
	
	int fd;
	int sz;
	char str[255];
	
	static char long_test_str[4096];
	memset(long_test_str, 0, 4096);
    memset(long_test_str, 80, 4095);
	printf("%s\n", long_test_str);

	printf("Testing... \n");
    fd = open("/dev/meschar", O_RDWR);      // Capital o, not zero
	
	if (fd < 0) { 
    	perror("r1"); 
    	exit(1); 
	} 
	
	// printf("Enter string: \n");
	// scanf("%s", str);
	
	// sz = write(fd, str, 255); 
  
	printf("Done sending :) \n");
	
	// printf("HP, I'm going to repeat back what you said (if you hit enter)\n");
	// getchar(\n);
	int ret = read(fd, receive, 256);
	printf("REPEAT OF MESSAGE: %s\n", receive);
	
	close(fd);
	
	return 0;
}