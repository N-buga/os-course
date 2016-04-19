#include "file_system.h"
#include "io.h"
#include "ioport.h"

void testRoot() {
	struct iNode* root = init();
	readDir(root);	
	struct iNode* test = takeFileByPath("test");
	readDir(test);
}

void testCreate() {
	struct iNode* root = init();
	create("file", root);
	mkdir("directory", root);
	create("aaaa", root);
	mkdir("bbbb", root);
	readDir(root);
}

void testReadWrite() {
	struct iNode* root = init();
	struct iNode* file = create("file", root);
	char* buf = "cat";
	write(file, 0, 3, buf);
	char* result = read(file, 0, 3);
	for (int i = 0; i < 3; i++) {
		printc(result[i]);
	}
	printc('\n');
}

void testFileByPath() {
	struct iNode* root = init();
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
