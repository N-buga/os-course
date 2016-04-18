#include "file_system.h"
#include "io.h"
#include "ioport.h"

void testRoot() {
	struct iNode* root = init();
	readDir(root);	
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
