#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "tfs.h"
#include "io.h"
#define WHITESPACE " \t\r\n"

int main() {
  int done = 0;
  char * tfs_file = NULL;

  do {
    // prompt is current open file, or none
    char * prompt = tfs_file ? tfs_file : "(no file)";

    // print the prompt, then get a line of input from the user
    prints(prompt);
    prints("> ");
    char line[IN_SIZE];
    inputs(line, IN_SIZE);

    // flag denoting we changed the open TFS, and need to save it to the file
    // after this command has been processed
    int altered = 0;

    char * command = strtok(line, WHITESPACE); // user's command
    char * arg1 = strtok(NULL, WHITESPACE); // first argument
    char * arg2 = strtok(NULL, WHITESPACE);
    // TODO: some commands need one more argument

    if (!strcmp("exit", command)) {
      done = 1;
    } else if (!strcmp("display", command)) {
      // most commands don't make sense if there's no open file
      if (!tfs_file) {
        prints("No file open.");
        println();
        continue;
      }
      display(); // display the TFS as columns of hex digits
    } else if (!strcmp("create", command)) {
      if (!arg1) {
        prints("Need a filepath.");
        println();
        continue;
      }
      int temp = open(arg1, O_RDONLY);
      if (temp != -1) {
        prints("File already exists: ");
        prints(arg1);
        println();
        close(temp); // close inadvertently opened file
        continue;
      }
      free(tfs_file); // free previous string (if any)
      * tfs_file = strdup(arg1); // save path for prompt and writing
      init(); // initialize new TFS
      altered = 1; // flag for saving to file
    } else if (!strcmp("open", command)) {
      if (!arg1) {
        prints("Need a filepath.");
        println();
        continue;
      }
      int temp = open(arg1, O_RDONLY);
      if (temp == -1) {
        prints("Error opening file: ");
        prints(arg1);
        println();
        continue;
      }
      // load TFS from new file (previous file already saved)
      read(temp, (char * ) tfs, 256);
      close(temp);
      free(tfs_file); // free previous string
      * tfs_file = strdup(arg1); // save path for prompt and writing
    }
    // TODO: more commands
    else if (!strcmp("mkdir", command)) {
      int dir;
      if (dir == -1) {
        prints("Directory already exists or isn't a valid name :");
        prints(arg1);
        println();
        continue;
      }

      mk_dir(arg1);
      altered = 1;

    } else if (!strcmp("ls", command)) {
      char blockPointer = Block_Pointer(arg1);
      if(blockPointer == -1) {
        continue;
      }

      for(int a = 1; a <= 10 && tfs[blockPointer][a] != 0; a++) {
        printc(tfs[blockPointer][a]);
        printc(' ');
      }
      printc('\n');

    } else if (!strcmp("import", command)) {
      char buffer[256];
      
      int temp = open(arg1, O_RDONLY);
      if (temp == -1) {
        prints("Error opening file: ");
        prints(arg1);
        println();
        continue;
      }

      int temp1 = open(arg2, O_RDONLY);
      if (temp1 == -1) {
        prints("Error opening file: ");
        prints(arg2);
        println();
        continue;
      }

      if (!arg1) {
        prints("Need a filepath.");
        println();
        continue;
      }
      
      if(!arg2) {
        prints("Need a filepath.");
        println();
        continue;
      }

      if (tfs[0][0] == 0) //Tfs[0][0] tells which block is open, but is 0 when TFS is full. 
      {
        prints("There is no more space in the TFS disk");
        continue;
      }
      int free_Block = tfs[0][0]; //pointer to the open block 
      char * LP = strrchr(arg1, '/'); 
      // command from arg1 (arg1 is pathname in Linux file system)
      char * tp = strrchr(arg2, '/');
      // command from arg2 (arg2 is absolute pathname in TFS)

      if (LP == NULL) {
        prints("Invalid path.\n");
        continue;
      }

      if(tp == NULL) {
        prints("Invalid path.\n");
        continue;
      }
      int file_In_w = open(arg1, O_RDONLY);
      int _read = read(*arg1, buffer, 256);
      struct stat buf;
      fstat(file_In_w, &buf);
      off_t size = buf.st_size;
      if(size == 0) {
        prints("File size is 0.\n");
        continue;
      }

      *(LP++) = '\0';
      if (strlen(tp) != 1) {
        prints("Invalid path.\n");
        continue;
      }
      
      *(LP++) = '\0';
      if (strlen(tp) != 1) {
        prints("Invalid path.\n");
        continue;
      }

      int parentBlock = Block_Pointer(LP); 
      //get the parent dir and put it int parent Block
      if (parentBlock == -1) {
        prints("Path does not exist.\n");
        continue;
      }
      
      for (int a = 1; a < 11; a++) {
        if (tfs[parentBlock][a] == * (LP)) {
        prints("File already exists\n");
        continue;
        }
      }

      int b = -1;
      for (int a = 1; a < 11; a++) {
        if (tfs[parentBlock][a] == 0) { 
          //0 denotes that spot is empty so TFS is not full
          //b is the offset for the dir name in the block
          b = a; 
          break;
        }
      }

      if (b == -1) {
        prints("not enough space in the dir to create a new one");
        return -1;
      }

      if(size < 16) {
        for(int i = 1; i < 16; i++) {
          if(tfs[parentBlock][i] == 0) {
            tfs[parentBlock][i] = size;
          }
        }
      }
      else {
        tfs[parentBlock][b] = * (LP); 
        //this puts the name of the dir into the empty Byte
        // int offset = ((b - 1) / 2) + 1; 
        //b-1 to remove the first meta bits (shifts from 1-10 to 0-9), 
        //then divide in half from 0-9 to 0-4 to get the index of the 4 bit numbers at the end of the block then add 11 to reach those bytes at the end of the block 

        int low_nibble = (b - 1) % 2; 
        int index = 0;
        //returns 1 or 0 (the remainder) if we need to access the low bits 
        if (low_nibble)
        //if is low is one, then you still need to get the high bits from the offset
        {
          int highBits = getHigherBits(tfs[parentBlock][index]);
          tfs[parentBlock][index] = (highBits << 4) | free_Block; 
          //gets high bits in the block pointer to combine the two bytes from the open block in the free-space list and puts them in the offset of the parent block 
        } else { 
          //else take low bits
          int lowBits = getLowerBits(tfs[parentBlock][index]);
          tfs[parentBlock][index] = lowBits | (free_Block << 4);
        }
        if (tfs[free_Block][0] == 0xFF) 
        //if at the last block
        {
          tfs[0][0] = 0; 
          index++;
          //0 denotes there are no more open blocks

        } else {
          tfs[0][0] = tfs[free_Block][0]; 
          //save the next open block and put it in tfs[0][0] 
        }
      }

        tfs[free_Block][0] = (parentBlock << 4) | free_Block;
        altered = 1;
    }
    else if (!strcmp("export", command)) {
      if (!arg1) {
        prints("Need a filepath.");
        println();
        continue;
      }
      char * tokens[16];
      // TFS block is 16
      char * token = strtok(arg1, "/");
      int num_of_tokens = 0;
      int currentBlock = 0;
      // Filling array with tokens
      for (int num = 0; num < 16 && token != NULL; num++) {
        tokens[num] = token;
        num_of_tokens++;
        token = strtok(NULL, "/");
      }
      if (token != NULL) {
        return -1;
      }

      for (int n = 0; n < num_of_tokens; n++) {
        //Each directory should be uppercase
        if (strlen( * tokens) > 1) {
          return -1;
        } else if (strlen( * tokens) == 0) {
          continue;
        }

        char directory = tokens[n][0];
        // Checking for Directory in block
        if (directory < 'A' || directory > 'Z') {
          prints("Incorrect");
          continue;
        }
        int i;
        int childBlock = -1;
        for (i = 1; i <= 10; i++) {
          if (tfs[currentBlock][i] == directory) {
            //remove the first metabits to shift from 0 to 9
            //divide it by 2 to get a four bit number
            //Add 11 at the end to reach those bytes at the end of the block
            int offset = ((i - 1) / 2) + 11;
            //Returns 1 or 0 to access the lower bits
            int low = (i - 1) % 2;
            //if low is = 1
            if (low) {
              childBlock = getLowerBits(tfs[currentBlock][offset]);

            }
            else {
              childBlock = getHigherBits(tfs[currentBlock][offset]);
            }
            break;
          }
        }
        //If childblock is -1, that means it didn't find the directory, so directory doesn't
        if (childBlock == -1) {
          prints("Directory does not exist");
          return -1;
        }
        currentBlock = childBlock;
        printc(tfs[currentBlock][i]);
      }
    } 
     else if (!strcmp("rm", command)) {
      if (!arg1) {
        prints("Need a filepath.");
        println();
        continue;
      }

    } else {
      prints("Command not recognized: ");
      prints(command);
      println();
    }

    if (altered) {
      // save changes to file
      // TODO: your code here
      // load TFS from new file (previous file already saved)
      int temp = open(arg1, O_RDONLY); 
      read(temp, (char * ) tfs, 256);
      close(temp);
      free(tfs_file); // free previous string
      * tfs_file = strdup(arg1); // save path for prompt and writing
    }

  } while (!done);

  return 0;
}
