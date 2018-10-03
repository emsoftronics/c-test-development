#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>

//char *socket_path = "./socket";
char *socket_path = "\0hidden";

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  char buf[100];
  int fd,rc;
  int x;
  int done = 0;

  struct timeval timeout1;

  if (argc > 1) socket_path=argv[1];

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (*socket_path == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path+1, socket_path+1, sizeof(addr.sun_path)-2);
  } else {
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
  }

/*
   FD_ZERO(&writeFDs);
   FD_SET(fd, &writeFDs);

   timeout1.tv_sec = 0;
   timeout1.tv_usec = 500000;
  */ 
  do {
	if (!done) {
x = fcntl(fd,F_GETFL, 0);
   fcntl(fd,F_SETFL, x | O_NONBLOCK);
	 done = 1;
	}
  connect(fd, (struct sockaddr*)&addr, sizeof(addr));
  printf("ErronNo(%d): %s\n", errno, strerror(errno));
  perror("connect");
  usleep(100);
  if((errno != 0) && (errno != 106)) continue;
	x = fcntl(fd,F_GETFL, 0);
   	fcntl(fd,F_SETFL, (x ^ O_NONBLOCK) | O_SYNC);
	 done = 0;
/*
 while(1) {
   if(select(fd+1, (fd_set *)NULL, &writeFDs, (fd_set *)NULL, (struct timeval *)(&timeout1))  > 0 )   
        {printf("Connected"); break;}
   else 
       printf("Not Connected -%d", errno);
	}

   printf("hello\n");
*/
/*
  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(-1);
  }
*/
  while( (rc=read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
    printf("trying to write ..\n");
    if (send(fd, buf, rc, MSG_NOSIGNAL) != rc) {
	break;
    }
    printf("Hello\n");
  }
//    shutdown(fd, O_RDWR);
	printf("continue...");
 }while(1);
  return 0;
}

