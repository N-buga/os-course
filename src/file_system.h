#include <sys/types.h>
#include <stdint.h>

struct iNode {
	pid_t id;
	char* name;
	int isDirectory;
	struct iList* files;
	int isOpened;
	uint32_t size;
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
void write(struct iNode* file, uint32_t offset, uint32_t len, char* buf);
int read(struct iNode* file, uint32_t offset, uint32_t len, char* buf);
struct iNode* takeFileByPath(char* path);
void extract();
struct iList* nextDir(struct iList* curFile);
