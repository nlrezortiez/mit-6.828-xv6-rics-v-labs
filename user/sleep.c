#include "user/user.h"

int main(int argc, char* argv[]) {

  if (argc < 2) {
    printf("Usage: sleep <sec>\n");
    exit(1);
  }

  int ticks = atoi(argv[argc - 1]);
  
  if (ticks < 0) {
    printf("sec must be >= 0");
  } else if (ticks == 0) {
    exit(0);
  }

  sleep(ticks);

  exit(0);
}
