// defines the "byte" type as an unsigned 8-bit integer, valued 0 to 255
typedef unsigned char byte;

// this is the TFS currently in memory, globally available to all functions
// the first index is that of the 16 blocks of the TFS
// the second index is that of the 16 bytes within a block
byte tfs[16][16];

void init();

void display();

void mk_dir(char* pathname);

int getBlock(int r, int b);

int getHigherBits(int n);

int getLowerBits(int c);

char getHex(char c);

int Block_Pointer(char *str);

// TODO: many more functions
