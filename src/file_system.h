#include <sys/types.h>
#define NULL 0

struct iNode {
	pid_t id;
	char* name;
	int isDirectory;
	struct iList* files;
	int isOpened;
	struct MemoryList* memory;	
};

struct iList{
	struct iList* nextFile;
	struct iNode* curFile;
};

struct MemoryList {
	void* curMemory;
	struct MemoryList* nextMemList; 
};

struct iNode* init();
void readDir(struct iNode* curDirectory);
struct iNode* create(char* name, struct iNode* curDirectory);
struct iNode* mkdir(char* name, struct iNode* curDirectory);
struct iNode* open(char* name, struct iNode* curDirectory);
void write(struct iNode* file, int offset, int len, char* buf);
char* read(struct iNode* file, int offset, int len);
