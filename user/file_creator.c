#include "kernel/types.h"

#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "user/user.h"

int stoi(const char *number, int *res) {
  *res = 0;
  for (const char *dig = number;*dig; ++dig) {
    if (*dig < '0' || *dig > '9')
      return -1;
    int digit = *dig - '0';
    *res *= 10;
    *res += digit;
  }
  return 0;
}

int create_empty_file(const char *file, int bytes) {
  printf("%d", bytes);
  int fd = open(file, O_WRONLY | O_CREATE | O_TRUNC);
  if (fd < 0)
    return -1;
  static char empty[BSIZE];
  memset(empty, 0, BSIZE);
  while (bytes > 0) {
    int to_write = bytes < BSIZE ? bytes : BSIZE;
    bytes -= to_write;
    if (write(fd, empty, to_write) != to_write) {
      close(fd);
      return -1;
    }
  }
  close(fd);
  return 0;
}

int main(int argc, const char **argv) {
  const char *file;
  int bytes;

  if (argc != 4){
    printf("incorrect number of argument for file creator\n");
    printf("expected: file_creator file_name number 1 or 0 (1 - bytes, 0 - blocks)\n");
    exit(-1);
  }
  
  file = argv[1];
  
  if (stoi(argv[2], &bytes)){
    printf("incorrect number of argument for file creator\n");
    exit(-1);
  }
  
  //argv[3] == 1 bytes
  //argv[3] == 0 blocks
  if (strcmp(argv[3], "1") ){
    if (bytes >= (1u << 31) / BSIZE){
      printf("cant create such big files\n");
      exit(-1);
    }
    bytes *= BSIZE;
  }


  int err = create_empty_file(file, bytes);
  if (err)
    printf("nullfile: failed\n");
  printf("Ok");
  exit(err);
}