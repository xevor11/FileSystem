#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "io.h"

void printc(char c) {
  // your code here
  write(STDOUT_FILENO, &c, 1);
}

void prints(char* s) {
  write(STDOUT_FILENO, s, strlen(s));
}

void println() {
  printc('\n');
}

void inputs(char dst[], int size) {
  // reads up to "size" characters from standard input into dst array if standard input contains a newline, replaces it with the null character regardless, dst must be null-terminated after this is called
  // your code here
    read(STDIN_FILENO, dst, size);
    for(int i = 0; i < size; i++) {
      if(dst[i] == '\n') {
        dst[i] = '\0';
      }
    }
}

/*
int get_File_Size(char* c) {
  struct stat st;
  stat(c, &st);
  int size = st.st_size;
  return size;
}
*/

/*
char buffer[256];
  int dir = read(*pathname, buffer, sizeof(buffer));

  if(!dir) {
    printc(0);
  }

   while (dir != '0')
    {
        write(dir, buffer, 256);
    }
*/
//
