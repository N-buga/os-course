#include "file_system.h"
#include "kmem_cache.h"
#include "io.h"
#include "ioport.h"
#include "memory.h"
#include "initramfs.h"

#pragma GCC diagnostic ignored "-Wstack-usage="

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
	printf("%d %d\n", (uint32_t) cpio_begin, (uint32_t) cpio_end);
	phys_t cur_descriptor = VA(cpio_begin);
	int offset = 0;
	while (VA(cpio_end) - cur_descriptor >= sizeof(struct cpio_header)) 
	{
		printf("cur_difference: %lu %lu, %lu\n", VA(cpio_end), cur_descriptor, VA(cpio_end) - cur_descriptor);
		struct cpio_header* header = (struct cpio_header*) cur_descriptor;
		uint32_t name_size = str16_to_int((char*) header->namesize, 8);
		printf("name_size: %u\n", name_size);
		char* name = (char*)(header + 1);
		for (uint32_t i = 0; i < name_size; i++) {
			printf("%c", name[i]);
		}
		printf("\n");
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
		
		printf("full_path: %s\n", full_path);
		printf("file_name: %s\n", file_name);
		printf("path_to_parent: %s\n", path_to_parent);
		
		printf("path_to_parent.size = %u\n", size(path_to_parent));
		struct iNode* directory = takeFileByPath(path_to_parent);

		printf("findInode: %p\n", directory);
		
		offset += sizeof(struct cpio_header) + name_size;
		MY_ALIGN(offset);
		
		if (S_ISDIR(str16_to_int(header->mode, 8))) {
			mkdir(file_name, directory);
			cur_descriptor = VA(cpio_begin) + offset;
		} else {
			uint32_t size_of_data = str16_to_int(header->filesize, 8);
			if (S_ISREG(str16_to_int(header->mode, 8))) {
				struct iNode* file = create(file_name, directory);
				char* buf = (char*) (cur_descriptor + offset);
				write(file, 0, size_of_data, buf);
			} 	
			offset += size_of_data;
			MY_ALIGN(offset);
			cur_descriptor = VA(cpio_begin) + offset;
		} 
	}
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

