/*
 * =====================================================================================
 *
 *       Filename:  client.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Wednesday 19 September 2018 10:00:20  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

//char *socket_path = "./socket";
#define SOCK_NAME "\0hidden"

static int getSocketFd(char *socket_name)
{
  struct sockaddr_un addr;
  int fd;
  int err = 0;
  char *socket_path = socket_name;

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (*socket_path == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path+1, socket_path+1, sizeof(addr.sun_path)-2);
  } else {
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
  }

  err = 0;
  while (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    if(err != errno) perror("connect error");
    err = errno;
    usleep(10000);
  }
  printf("connected to server!!\n");

  return fd;
}

int main(int argc, char *argv[]) {
  char buf[100];
  int fd,rc;


  while(1) {
      fd = getSocketFd(SOCK_NAME);
      while( (rc=read(STDIN_FILENO, buf, sizeof(buf))) > 0) {

        if (send(fd, buf, rc, MSG_NOSIGNAL) != rc) {
            break;
        }
    }
    close(fd);
    printf("Disconnected with server!!\n");
  }
  return 0;
}
