#include "file_system.h"
#include "kmem_cache.h"
#include "io.h"
#include "ioport.h"
#include "memory.h"
#include "initramfs.h"
#include "locks.h"

#pragma GCC diagnostic ignored "-Wstack-usage="

struct lock_descriptor file_system_lock;

int maxID  = 0;
struct iNode* root;

extern phys_t cpio_begin;
extern phys_t cpio_end;

struct iNode* init() {
	root = kmem_alloc(sizeof(struct iNode));
	root->id = maxID++;
	root->name = "root";
	root->isDirectory = 1;
	root->files = NULL;
	root->isOpened = 0;
	root->memory = NULL;
	extract();
	return root;
}

uint32_t str16_to_int(char* s, size_t len) {
    uint32_t x = 0;
    for (size_t i = 0; i < len; ++i) {
        x = x * 16 ;
        if (s[i] >= '0' && s[i] <= '9') {
            x += s[i] - '0';
        } else if (s[i] >= 'a' && s[i] <= 'z'){
            x += s[i] - 'a' + 10;
        } else {
            x += s[i] - 'A' + 10;
        }
    }

    return x;
}

void connect(char* full_path, char* first_string, char* second_string, uint32_t size_second_string) {
	while (first_string[0] != 0) {
		full_path[0] = first_string[0];
		full_path++;
		first_string++;
	}
	for (uint32_t i = 0; i < size_second_string; i++) {
		full_path[i] = second_string[i];
	}
	full_path[size_second_string] = 0;
}

void find_parent(char* file_name, char* path_to_parent, char* full_path) {
	uint32_t i = 0;
	uint32_t last_dir = 0;
	uint32_t pos_file_name = 0;
	while (!(full_path[i] == 0)) {
		if (full_path[i] == '/') {
			last_dir = i;
			pos_file_name = 0;
		} else {
			file_name[pos_file_name++] = full_path[i];
		}
		path_to_parent[i] = full_path[i];
		i++;
	}
	path_to_parent[last_dir] = 0;
	file_name[pos_file_name] = 0;
}

int equals_size(char* str1, char* str2, uint32_t size) {
	for (uint32_t i = 0; i < size; i++) {
		if (str1[i] != str2[i]) {
			return 0;
		}
	}
	return 1;
}

uint32_t size(char* s) {
	uint32_t result = 0;
	while (s[result] != 0) {
		result++;
	}
	return result;
}

void extract() {
	phys_t cur_descriptor = VA(cpio_begin);
	int offset = 0;
	while (VA(cpio_end) - cur_descriptor >= sizeof(struct cpio_header)) 
	{
		struct cpio_header* header = (struct cpio_header*) cur_descriptor;
		uint32_t name_size = str16_to_int((char*) header->namesize, 8);
		char* name = (char*)(header + 1);
		if (equals_size(name, "TRAILER!!!", 10)) {
			break;
		}
		char full_path[name_size + 10];
		for (uint32_t i = 0; i < name_size; i++) {
			full_path[i] = name[i];
		}
		full_path[name_size] = 0;
		char path_to_parent[name_size + 10];
		char* file_name = (char*) kmem_alloc(name_size);
		find_parent(file_name, path_to_parent, full_path);
		
		struct iNode* directory = takeFileByPath(path_to_parent);

		offset += sizeof(struct cpio_header) + name_size;
		MY_ALIGN(offset);
		
		uint32_t size_of_data = str16_to_int(header->filesize, 8);
		
		if (S_ISDIR(str16_to_int(header->mode, 8))) {
			mkdir(file_name, directory);
			offset += size_of_data;
			MY_ALIGN(offset);
			cur_descriptor = VA(cpio_begin) + offset;
		} else {
			if (S_ISREG(str16_to_int(header->mode, 8))) {
				struct iNode* file = create(file_name, directory);
				uint8_t* buf = (uint8_t*) (VA(cpio_begin) + offset);
				write(file, 0, size_of_data, (char*)buf);
			} 	
			offset += size_of_data;
			MY_ALIGN(offset);
			cur_descriptor = VA(cpio_begin) + offset;
		} 
	}
}

void readDir(struct iNode* curDirectory) {
	if (curDirectory == NULL) {
		printf("you try to find NULL\n");
		return;
	}
	if (curDirectory->isDirectory == 0) {
		prints("This isn't directory\n");
		return;
	}
	lock(&file_system_lock);
	struct iList* curFileNode = curDirectory->files;
	unlock(&file_system_lock);
	while (curFileNode != NULL) {
		prints(curFileNode->curFile->name);
		printf(" %d", curFileNode->curFile->isDirectory);
		prints("\n");
		curFileNode = curFileNode->nextFile;
	}
}

struct iList* nextDir(struct iList* curFile) {
	if (curFile != NULL) {
		return curFile->nextFile;
	}
	return NULL;
}

struct iList* openDir(struct iNode* curDir) {
	lock(&file_system_lock);
	struct iList* result = curDir->files;
	unlock(&file_system_lock);
	return result;
}

int isDir(struct iList* curDir) {
	return curDir->curFile->isDirectory;
}

struct iNode* create(char* name, struct iNode* curDirectory) {
	if (curDirectory->isDirectory == 0) {
		prints("This isn't directory\n");
		return NULL;
	}
	lock(&file_system_lock);
	struct iList* firstFile = curDirectory->files;
	struct iNode* newFile = (struct iNode*) kmem_alloc(sizeof(struct iNode));
	newFile->id = maxID++;
	newFile->name = name;
	newFile->isDirectory = 0;
	newFile->files = NULL;
	newFile->isOpened = 0;
	newFile->memory = (struct MemoryList*) kmem_alloc(sizeof(struct MemoryList));
	newFile->size = 0;
	struct MemoryList* curMemory = newFile->memory;
	curMemory->curMemory = kmem_alloc(PAGE_SIZE);
	struct iList* newNode = (struct iList*) kmem_alloc(sizeof(struct iList));
	newNode->nextFile = firstFile;
	newNode->curFile = newFile;
	curDirectory->files = newNode;
	unlock(&file_system_lock);
	return newFile;
}

struct iNode* mkdir(char* name, struct iNode* curDirectory) {
	if (curDirectory->isDirectory == 0) {
		prints("This isn't directory\n");
		return NULL;
	}
	lock(&file_system_lock);
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
	unlock(&file_system_lock);
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
	if (curDirectory == NULL) {
		return NULL;
	}
	if (curDirectory->isDirectory == 0) {
		prints("This isn't directory\n");
		return NULL;
	}
	lock(&file_system_lock);
	struct iList* curNode = curDirectory->files;
	unlock(&file_system_lock);
	while (curNode != NULL) {
		if (equals(curNode->curFile->name, name)) {
			break;
		}
		curNode = curNode->nextFile;
	}	
	if (curNode == NULL) {
		printf("Can't find file %s\n", name);
		return NULL;
	}
	return curNode->curFile;
}

struct iNode* takeFileByPath(char* path) {
	struct iNode* curFile = root;
	if (size(path) == 0) {
		return curFile;
	}
	int size = 0;
	while (path[size] != 0) {
		size++;
	}
	char nextFile[size + 1];
	int ptrPath = 0;
	int ptrNextFile = 0;
	while (path[ptrPath] != 0) {
		if (path[ptrPath] != '/') {
			nextFile[ptrNextFile++] = path[ptrPath++];
		} else {
			nextFile[ptrNextFile++] = 0;
			curFile = takeFile(nextFile, curFile);
			if (curFile == NULL) {
				printf("Can't find path %s\n", path); 
				return NULL;
			}
			ptrNextFile = 0;
			ptrPath++;	
		}
	}
	nextFile[ptrNextFile++] = 0;
	curFile = takeFile(nextFile, curFile);
	if (curFile == NULL) {
		printf("We can't find file with name = %s\n", nextFile);
	}
	return curFile;
}

struct iNode* open_from_directory(char* name, struct iNode* curDirectory) {
	if (curDirectory == NULL) {
		prints("Cur directory is NULL\n");
	}
	if (curDirectory->isDirectory == 0) {
		prints("This isn't directory\n");
		return NULL;
	}
	lock(&file_system_lock);
	struct iList* curNode = curDirectory->files;
	unlock(&file_system_lock);
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
	lock(&file_system_lock);
	if (result->isOpened == 0) {
		printf("File %s is already opened\n", name);
		unlock(&file_system_lock);
		return NULL;
	}
	unlock(&file_system_lock);
	result->isOpened = 1;
	return result;
}

int close(struct iNode* file) {
	if (file == NULL) {
		prints("Can't close NULL ptr on file\n");
		return 0;
	}
	if (file->isDirectory) {
		printf("File %s is directory\n", file->name);
		return 0;
	}
	lock(&file_system_lock);
	if (file->isOpened == 1) {
		printf("File %s is already opened\n", file->name);
		unlock(&file_system_lock);
		return 0;
	}
	file->isOpened = 0;
	unlock(&file_system_lock);
	return 1;
} 

void write(struct iNode* file, uint32_t offset, uint32_t len, char* buf) {
	if (file == NULL) {
		printf("Can't write to file NULL");
		return;
	}
	if (file->isDirectory == 1) {
		prints("This file is directory\n");
		return;
	}
	if (file->size < (uint32_t)offset + len) {
		file->size = offset + len;
	}
	struct MemoryList* curNode = file->memory;
	if (curNode == NULL) {
		prints("You do something wrong!\n");
		return;
	}
	long long signedOffset = offset;
	while (signedOffset - PAGE_SIZE >= 0) {
		if (curNode->nextMemList == NULL) {
			curNode->nextMemList = (struct MemoryList*) kmem_alloc(sizeof(struct MemoryList));
			curNode->nextMemList->nextMemList = NULL;
			curNode->nextMemList->curMemory = kmem_alloc(PAGE_SIZE);
		}
		curNode = curNode->nextMemList;
		signedOffset -= PAGE_SIZE;	
	}
	lock(&file_system_lock);
	char* writePointer = (char*)curNode->curMemory;	
	int curMemoryOffset = offset;
	for (uint32_t i = 0; i < len; i++) {
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
	unlock(&file_system_lock);
}

int read(struct iNode* file, uint32_t offset, uint32_t len, char* buf) {
	if (file == NULL) {
		prints("Can't read from file NULL");
		return 0;
	}
	if (file->isDirectory == 1) {
		prints("This file is directory");
		return 0;
	}
	struct MemoryList* curNode = file->memory;
	if (curNode == NULL) {
		prints("You do something wrong!");
		return 0;
	}
	long long signedOffset = offset;
	while (signedOffset - PAGE_SIZE >= 0) {
		printf("offset = %lld, page_size = %u\n", signedOffset, PAGE_SIZE); 
		if (curNode->nextMemList == NULL) {
			return 0;	
		}
		curNode = curNode->nextMemList;
		signedOffset -= PAGE_SIZE;	
	}
	
	char* readPointer = (char*)curNode->curMemory;	
	uint32_t curMemoryOffset = offset;
	lock(&file_system_lock);
	for (uint32_t i = 0; i < len; i++) {
		buf[i] = readPointer[curMemoryOffset++];
		if (curMemoryOffset == PAGE_SIZE) {
			if (curNode->nextMemList == NULL) {
				unlock(&file_system_lock);
				return i;
			}
			curNode = curNode->nextMemList;
			readPointer = curNode->curMemory;
			curMemoryOffset = 0;
		}
	}
	unlock(&file_system_lock);
	return len;
}

