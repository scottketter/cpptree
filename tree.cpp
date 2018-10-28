#include "tree.h"
#include <iostream>
#include <string>

// name space
using namespace std;

int main(){

	// compile: g++ -std=c++11 -static-libstdc++ -c "%f"
	// build: g++ -std=c++11 -static-libstdc++ -o "%e" "%f"

	Tree tree("TestTree");

	// build
	tree.nodeNew(0, 1); // Index=1
	tree.nodeNew(1, 11); // 2
	tree.nodeNew(0, 2); // 3
	tree.nodeNew(3, 21); // 4
	tree.nodeNew(0, 3); // 5
	tree.nodeNew(5, 31); // 6
	tree.nodeNew(1, 12); // 7
	tree.nodeNew(3, 22); // 8
	tree.nodeNew(5, 32); // 9
	tree.nodeNew(1, 13); // 10
	tree.nodeNew(3, 23); // 11
	tree.nodeNew(5, 33); // 12
	tree.nodeNew(0, 4); // 13
	tree.nodeNew(2, 111); // 14
	tree.Print("tree_build.txt");

	// delete test
	tree.nodeDelete(13);
	tree.nodeDelete(8);
	tree.nodeDeleteAll(1);
	tree.nodeDeleteAll(6);
	tree.Print("tree_delete.txt");

	// replace test
	tree.nodeNew(9, 321); // Index=6
	tree.nodeNew(9, 322); // 1
	tree.nodeNew(11, 231); // 2
	tree.nodeNew(0, 4); // 14
	tree.nodeNew(14, 41); // 7
	tree.nodeNew(14, 42); // 10
	tree.nodeNew(14, 43); // 8
	tree.nodeNew(14, 44); // 14
	tree.nodeNew(8, 431); // 15
	tree.Print("tree_replace.txt");

	// move up
	tree.nodeMoveUp(2, 4);
	tree.nodeMoveUp(11, 4);
	tree.nodeMoveUp(9, 3);
	tree.nodeMoveUp(8, 14);
	tree.Print("tree_moveup.txt");

	// move down
	tree.nodeMoveDown(8, 10);
	tree.nodeMoveDown(9, 12);
	tree.nodeMoveDown(11, 4);
	tree.Print("tree_movedown.txt");

	// move
	tree.nodeMove(2, 11);
	tree.nodeMove(12, 5);
	tree.nodeMove(14, 3);
	tree.nodeMove(8, 0);
	tree.Print("tree_move.txt");

	// copy
	tree.nodeCopy(8, 14);
	tree.nodeCopy(14, 0);
	tree.nodeCopy(3, 15);
	tree.nodeCopy(23, 0);
	tree.Print("tree_copy.txt");

	// save and load
	tree.Save("tree_save_1.dat", 0);
	tree.Clear();
	tree.Load("tree_save_1.dat");
	tree.Save("tree_save_2.dat", 11);
	tree.Print("tree_saveload.txt");

	// import
	tree.Import("tree_save_2.dat", 10);
	tree.Print("tree_import.txt");

	tree.Clear();
	return 0;

}
