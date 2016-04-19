#include "file_system.h"
#include "io.h"
#include "ioport.h"
#include<stddef.h>

struct iNode* root;

void printTree(struct iNode* curNode, int level) {
	for (int i = 0; i < level; i++) {
		printf("|");
	}
	printf("%s\n", curNode->name);
	struct iList* curListNode = curNode->files;
	while (curListNode != NULL) {
		printTree(curListNode->curFile, level + 1);
		curListNode = nextDir(curListNode);
	}
}

void testOutLoadedTree() {
	printTree(root, 0);	
}

void testContainLoadFiles() {
	struct iNode* curFile = takeFileByPath("src/test/a.txt");
	char buf[100];
	if (curFile == NULL) {
		return;
	}
	printf("curFile.size = %d\n", curFile->size);
	int count = read(curFile, 0, curFile->size, buf);
	printf("%d\n", count);
	for (int i = 0; i < count; i++) {
		printf("%c", buf[i]);
	}		
}

void testRoot() {
	readDir(root);
	struct iNode* test = takeFileByPath("test");
	readDir(test);
}

void testCreate() {
	create("file", root);
	mkdir("directory", root);
	create("aaaa", root);
	mkdir("bbbb", root);
	readDir(root);
}

void testReadWrite() {
	struct iNode* file = create("file", root);
	char* buf = "cat";
	write(file, 0, 3, buf);
	char result[4];
	read(file, 0, 3, result);
	for (int i = 0; i < 3; i++) {
		printc(result[i]);
	}
	printc('\n');
}

void testFileByPath() {
	struct iNode* inDir1 = mkdir("dir1", root);
	struct iNode* inDir2 = mkdir("dir2", inDir1);
	char* path = "dir1/dir2";
	struct iNode* result = takeFileByPath(path);
	if (result == inDir2) {
		prints("Ok\n");
	} else {
		prints("No\n");
	}
}
