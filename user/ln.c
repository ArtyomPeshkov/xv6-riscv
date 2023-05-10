#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, const char **argv) {
  if (argc == 3) {
    if (link(argv[1], argv[2]))
      fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
    exit(0);
  } else if (argc == 4 && !strcmp(argv[1], "-s")) {
    if (symlink(argv[2], argv[3]))
      fprintf(2, "symlink %s %s: failed\n", argv[2], argv[3]);
    exit(0);
  } else {
    fprintf(2, "Usage: ln old new\n");
    exit(1);    
  }
}

