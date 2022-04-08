#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "tfs.h"
#include "io.h"

// initializes the TFS in memory
void init() {

  // first block is root directory
  // - there is free space, and first free block is 01
  tfs[0][0] = 1;
  // - root dir contents and their locations are all 0
  for (int i = 1; i < 16; i++)
    tfs[0][i] = 0;

  // remaining blocks are free
  // each free block points to the next one,
  // except the last block which stores 255 in first byte
  for (int i = 1; i < 16; i++)
    tfs[i][0] = i == 15 ? 0xFF : i + 1;
    // free block data (beyond byte 0) is undefined (don't need to set)
}

// display the TFS as columns of hex digits
void display() {
  // your code here
  for(int i = 0; i < 16; i++) {
    char hex = getHex(i);
    printc('x');
    println();
    for(int j = 0; i < 16; j++) {
      printc(' ');
      //
      printc(getHex(getHigherBits(tfs[i][j])));
      printc(getHex(getLowerBits(tfs[i][j])));
    }
  }
  println();
  // println(byte&0xff);
  // for each block in the tfs
  //   for each byte in the block
  //       printc(getHex(getUpperBits(byte)))
  //       printc(getLowerbits(byte))
  // println(return byte&0xff)
}
void mk_dir(char* pathname) {
  if (!pathname) {
    prints("Need a valid path.");
    println();
  }
  if (tfs[0][0] == 0) //Tfs[0][0] tells which block is open, but is 0 when TFS is full. 
  {
    prints("There is no more space in the TFS disk");
  }
  int openBlock = tfs[0][0]; //pointer to the open block 
  char * Holder = strrchr(pathname, '/'); //get the command from arg1

  if (Holder == NULL) {
    prints("Invalid path.\n");
  }
  *(Holder++) = '\0';

  if (strlen(Holder) != 1) {
    prints("Invalid path.\n");
  }

  int parentBlock = Block_Pointer(pathname); //get the parent dir and put it int parentlLock
  if (parentBlock == -1) {
    prints("Path does not exist.\n");
  }

  for (int a = 1; a < 11; a++) {
    if (tfs[parentBlock][a] == * (Holder)) {
      prints("Directory already exists\n");
      continue;
    }
  }

  int b = -1;
  for (int a = 1; a < 11; a++) {
    if (tfs[parentBlock][a] == 0) { //0 denotes that that spot is empty so TFS is not full
      b = a; //b is the offset for the dir name in the block 
      break;
    }
  }

  if (b == -1) {
    prints("not enough space in the dir to create a new one");
  }

  tfs[parentBlock][b] = * (Holder); //this puts the name of the dir into the empty Byte

  int offset = ((b - 1) / 2) + 11; //b-1 to remove the first meta bits (shifts from 1-10 to 0-9), 
  //then divide in half from 0-9 to 0-4 to get the index of the 4 bit numbers at the end of the block then add 11 to reach those bytes at the end of the block 

  int isLow = (b - 1) % 2; //returns 1 or 0 (the remainder) if we need to access the low bits 
  if (isLow) 
  { //if is low is one, then you still need to get the high bits from the offset
    int highBits = getHigherBits(tfs[parentBlock][offset]);
    tfs[parentBlock][offset] = (highBits << 4) | openBlock; //grabs high bits in the block ponter to combine the two bytes from the open block and puts them in the offset of the parentBlock 
  } else { 
    //else you need the low bits and need to offset it by 4 to get just the low bits, otherwise the high bits are included as well which is not good 
    int lowBits = getLowerBits(tfs[parentBlock][offset]);
    tfs[parentBlock][offset] = lowBits | (openBlock << 4);
  }

  if (tfs[openBlock][0] == 0xFF) //if at the last block, there is no more opened blocks 
  {
    tfs[0][0] = 0; //0 denotes there are no more open blocks 
  } else {
    tfs[0][0] = tfs[openBlock][0]; //otherwise save the next open block and put it in tfs[0][0] 
  }
  tfs[openBlock][0] = (parentBlock << 4) | openBlock;
}

/*
void ls(char* pathname, int block_number) {
  // we assume the preflight checks are already complete
  // checking if the pathname is a correct pathname
  // The directory under consideration exists
  int index;
  int rows;
  int columns;
  for(int rows = 0; rows < 16; rows++) {
    // tfs[0][0] -> 
    for(int columns = 1; columns < 15; columns++) {
      int get_block_number = Block_Pointer(&pathname[index]);
      index++;
      while(tokenize(pathname) != NULL) {
        char value = tfs[rows][columns];
        char path_value = getHex(pathname[index]);
        if(path_value == value) {
          int get_block_number = Block_Pointer(&pathname[index + 1]);
          tfs[rows][columns] = tfs[get_block_number][columns];
        }
        else {
        continue;
        }
      }
    }
    */
// ls:
   // tokenize pathname 
   // check each character for lowercase or '\0'
   // Look for the directory starting at root
   // Check bytes 11 - 15 of the root for the block pointer
   // Jump to that row search again
   // Once we reach last directory then print its contents



// Import :
  // we assume the preflight checks are already complete
  // checking if the pathname is a correct pathname
  // Does the file exist in TFS
  // Can the file be opened to writing in arg2

  // Check size of file
  // If file > 16 bytes
  // Keep first 8 bytes open to read from file header into TFS
  // Keep next n bytes to read and write file data
  // Take 16 bytes at a time (to accomodate in each row) 
  // close the file handle 
  // return
  // algorithm:
  // file size greater than 16 bytes, 
  // then read 8 bytes into the file head
  //     then read a maximum of 16 bytes into the new body
  //         then add a pointer to the new body block to the file head
  //           then just close the file handle

// Export:
  // we assume the preflight checks are already complete
  // checking if the pathname is a correct pathname
  // Does the file exist in TFS
  // Can the file be opened to writing in arg2

  // Check size of file
  // If file > 16 bytes
  // Keep first 8 bytes open to write file header
  // Keep next n bytes to store file data
  // Take 16 bytes at a time (to accomodate in each row) 
  // close the file handle 
  // return
//}

// Remove:
  // we assume the preflight checks are already complete
  // checking if the pathname is a correct pathname
  // Can the file be opened to writing in arg2
  // Does the file or directory exist in TFS
  // if not end
  // make sure the contents of the file or directory block is empty if not then end

  // re assign the block pointers for free-space list
  // change block number value from 'x' to '0' in the 11 to 15 bytes of the parent directory
//}

// search through a block
// look for directory
// get the block pointer using highbits and lowbits
// look for directory...
// Once you find it return contents of the dir

int Block_Pointer(char *str) {

    char *tokens[16];
    //At most our tfs block is 16
    char *token = strtok(str, "/" );
    int num_of_Tokens = 0;
    int currentBlock = 0; //Always will be the 0 block
    //Fill the array of tokens with my tokens
    for (int val = 0; val < 16 && token != NULL; val++)
    {
        tokens[val] = token;
        num_of_Tokens++;
        token = strtok(NULL, "/");
    }
    if (token != NULL)
    {
        //there's still things we need to tokenize, which means we can't access it
        return -1;
    }

    for (int num = 0; num < num_of_Tokens; num++) {
        //Each directory should be uppercase 1
        if (strlen(*tokens) > 1) {
            return -1;
        } else if (strlen(*tokens) == 0) {
            continue;
        }

        char dir = tokens[num][0];
        //Check if the directory name is correct
        if (dir < 'A' || dir > 'Z') {
            prints("Directory name is incorrect");
            return -1;
        }
        int i;
        int child_block = -1;
        for (i = 1; i <= 10; i++) {
            if (tfs[currentBlock][i] == dir) {
                //remove the first metabits to shift from 0 to 9
                //divide it by 2 to get a four bit number
                //Add 11 at the end to reach those bytes at the end of the block
                int offset = ((i - 1) / 2) + 11;
                //Returns 1 or 0 to access the lower bits
                int isLow = (i - 1) % 2;
                //if low is = 1
                if (isLow) {
                    child_block = getLowerBits(tfs[currentBlock][offset]);

                }
                //Otherwise
                else {
                    child_block = getHigherBits(tfs[currentBlock][offset]);
                }
                break;
            }

        }
        //If childblock is -1, that means it didn't find the directory, so directory doesn't
        if (child_block == -1)
        {
            prints("Directory does not exist");
            return -1;
        }
        currentBlock = child_block;
    }
    return currentBlock;
}

// not used only for reference
int getBlock(int r, int b)
{
  int x = (r+1)/2;
  char bit = tfs[b][10+x];
  char* samp = "";
  memcpy(samp, &bit, 2);
  char* str = "";
  int val = 0;

  if(r % 2 == 0) //its the denominator
  {
    memcpy(str, &bit, 1);
    val = atoi(str);
    return val;
  }
  else //its the numerator
  {
    char num[2];
    strncpy(num, &bit, sizeof(bit));

    memset(num, ' ', 1);
    str = strtok(num, " ");
    val = atoi(str);

    return val;
  }
}

int getHigherBits(int n) {
  return n >> 4;
}

 int getLowerBits(int c) {
  return c & 0xF;
}

char getHex(char c) {
  int num = 0;
  if(num == '0') {
    return num;
  }
  if(c < 10) {
    num = '0' + c;
    return num;
  }
  else {
    num = 'A' + (c - 10);
    return num;
  }
}

/*
char* tokenize(char* path) {
  const char c = '/';
  char* token;
  token = strtok(path, &c);
  while( token != NULL ) {
    token = strtok(NULL, &c);
   }
   return path;
}

*/

// TODO: many more functions, to handle other commands
