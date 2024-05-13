#include <user/user.h>

typedef int pid_t; 

int main() {
  
  int fds[2] = {};
  pipe(fds);

  
  pid_t pid = fork();
  unsigned char readvar = 0;

  if (pid  == -1) {
    printf("fork() error\n aborting");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    read(fds[0], &readvar, sizeof(unsigned char));
    close(fds[0]);
    printf("%d: recieved ping\n", getpid());
    
    write(fds[1], &readvar, sizeof(unsigned char));
    close(fds[1]);
    exit(EXIT_SUCCESS);
  } else {
    const unsigned char byte = 0x2B;
    write(fds[1], &byte, sizeof(unsigned char));
    close(fds[1]);
    
    read(fds[0], &readvar, sizeof(unsigned char));
    close(fds[0]);
    printf("%d: recieved pong\n", getpid(), readvar);
  }

  exit(0);
}
