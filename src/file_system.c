#include "file_system.h"
#include "kmem_cache.h"
#include "io.h"
#include "ioport.h"
#include "memory.h"

int maxID  = 0;
struct iNode* root;

struct iNode* init() {
	root = kmem_alloc(sizeof(struct iNode));
	root->id = maxID++;
	root->name = "root";
	root->isDirectory = 1;
	root->files = NULL;
	root->isOpened = 0;
	root->memory = NULL;
	return root;
}

void readDir(struct iNode* curDirectory) {
	if (curDirectory->isDirectory == 0) {
		prints("This isn't directory\n");
		return;
	}
	struct iList* curFileNode = curDirectory->files;
	while (curFileNode != NULL) {
		prints(curFileNode->curFile->name);
		printf(" %d", curFileNode->curFile->isDirectory);
		prints("\n");
		curFileNode = curFileNode->nextFile;
	}
}

struct iNode* create(char* name, struct iNode* curDirectory) {
	if (curDirectory->isDirectory == 0) {
		prints("This isn't directory\n");
		return NULL;
	}
	struct iList* firstFile = curDirectory->files;
	struct iNode* newFile = (struct iNode*) kmem_alloc(sizeof(struct iNode));
	newFile->id = maxID++;
	newFile->name = name;
	newFile->isDirectory = 0;
	newFile->files = NULL;
	newFile->isOpened = 0;
	newFile->memory = (struct MemoryList*) kmem_alloc(sizeof(struct MemoryList));
	newFile->memory->nextMemList = NULL;
	newFile->memory->curMemory = kmem_alloc(PAGE_SIZE);
	struct iList* newNode = (struct iList*) kmem_alloc(sizeof(struct iList));
	newNode->nextFile = firstFile;
	newNode->curFile = newFile;
	curDirectory->files = newNode;
	return newFile;
}

struct iNode* mkdir(char* name, struct iNode* curDirectory) {
	if (curDirectory->isDirectory == 0) {
		prints("This isn't directory\n");
		return NULL;
	}
	struct iList* firstFile = curDirectory->files;
	struct iNode* newDirectory = (struct iNode*) kmem_alloc(sizeof(struct iNode));
	newDirectory->id = maxID++;
	newDirectory->name = name;
	newDirectory->isDirectory = 1;
	newDirectory->files = NULL;
	newDirectory->isOpened = 0;
	newDirectory->memory = NULL;
	struct iList* newNode = (struct iList*) kmem_alloc(sizeof(struct iList));
	newNode->nextFile = firstFile;
	newNode->curFile = newDirectory;
	curDirectory->files = newNode;
	
	return newDirectory;
}

int equals(char* s1, char* s2) {
	int ptr = 0;
	while (s1[ptr] != 0 && s2[ptr] != 0) {
		if (s1[ptr] != s2[ptr]) {
			return 0;
		}
		ptr++;
	}
	if (s1[ptr] == s2[ptr]) {
		return 1;
	}
	return 0;
}

struct iNode* takeFile(char* name, struct iNode* curDirectory) {
	if (curDirectory->isDirectory == 0) {
		prints("This isn't directory\n");
		return NULL;
	}
	struct iList* curNode = curDirectory->files;
	while (curNode != NULL) {
		if (equals(curNode->curFile->name, name)) {
			break;
		}
		curNode = curNode->nextFile;
	}	
	if (curNode == NULL) {
		prints("Can't find file\n");
		return NULL;
	}
	return curNode->curFile;
}

struct iNode* takeFileByPath(char* path) {
	struct iNode* curFile = root;
	int size = 0;
	while (path[size] != 0) {
		size++;
	}
	char* nextFile = (char*)kmem_alloc(size + 1);
	int ptrPath = 0;
	int ptrNextFile = 0;
	while (path[ptrPath] != 0) {
		if (path[ptrPath] != '/') {
			nextFile[ptrNextFile++] = path[ptrPath++];
		} else {
			nextFile[ptrNextFile++] = 0;
			curFile = takeFile(nextFile, curFile);
			ptrNextFile = 0;
			ptrPath++;	
		}
	}
	nextFile[ptrNextFile++] = 0;
	curFile = takeFile(nextFile, curFile);
	return curFile;
}

struct iNode* open(char* name, struct iNode* curDirectory) {
	if (curDirectory->isDirectory == 0) {
		prints("This isn't directory\n");
		return NULL;
	}
	struct iList* curNode = curDirectory->files;
	while (curNode != NULL) {
		if (equals(curNode->curFile->name, name) && curNode->curFile->isDirectory == 0) {
			break;
		}
		curNode = curNode->nextFile;
	}	
	struct iNode* result;
	if (curNode == NULL) {
		result = create(name, curDirectory);
	} else {
		result = curNode->curFile;
	}
	result->isOpened = 1;
	return result;
} 

void write(struct iNode* file, int offset, int len, char* buf) {
	if (file->isDirectory == 1) {
		prints("This file is directory");
		return;
	}
	struct MemoryList* curNode = file->memory;
	if (curNode == NULL) {
		prints("You do something wrong!");
		return;
	}
	while (offset - PAGE_SIZE >= 0) {
		if (curNode->nextMemList == NULL) {
			curNode->nextMemList = (struct MemoryList*) kmem_alloc(sizeof(struct MemoryList));
			curNode->nextMemList->nextMemList = NULL;
			curNode->nextMemList->curMemory = kmem_alloc(PAGE_SIZE);
		}
		curNode = curNode->nextMemList;
		offset -= PAGE_SIZE;	
	}
	char* writePointer = (char*)curNode->curMemory;	
	int curMemoryOffset = offset;
	for (int i = 0; i < len; i++) {
		writePointer[curMemoryOffset++] = buf[i];
		if (curMemoryOffset == PAGE_SIZE) {
			if (curNode->nextMemList == NULL) {
				curNode->nextMemList = (struct MemoryList*) kmem_alloc(sizeof(struct MemoryList));
				curNode->nextMemList->nextMemList = NULL;
				curNode->nextMemList->curMemory = kmem_alloc(PAGE_SIZE);
			}
			curNode = curNode->nextMemList;
			writePointer = curNode->curMemory;
			curMemoryOffset = 0;
		}
	}
}

char* read(struct iNode* file, int offset, int len) {
	if (file->isDirectory == 1) {
		prints("This file is directory");
		return NULL;
	}
	struct MemoryList* curNode = file->memory;
	if (curNode == NULL) {
		prints("You do something wrong!");
		return NULL;
	}
	while (offset - PAGE_SIZE >= 0) {
		if (curNode->nextMemList == NULL) {
			return NULL;	
		}
		curNode = curNode->nextMemList;
		offset -= PAGE_SIZE;	
	}
	char* readPointer = (char*)curNode->curMemory;	
	int curMemoryOffset = offset;
	char* result = (char*) kmem_alloc(len);
	for (int i = 0; i < len; i++) {
		result[i] = readPointer[curMemoryOffset++];
		if (curMemoryOffset == PAGE_SIZE) {
			if (curNode->nextMemList == NULL) {
				kmem_free((void*) result);
				return NULL;
			}
			curNode = curNode->nextMemList;
			readPointer = curNode->curMemory;
			curMemoryOffset = 0;
		}
	}
	return result;
}

