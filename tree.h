#ifndef VER_TREE
#define VER_TREE	2.0;

#include "cex.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>

// name space
using namespace std;

// node class for each node in a tree structure
class Node{

	public:

	// each node must handle these variables
	bool Free = true; // the node is marked as free to reuse in the deadpool
	long long Index = -1; // the resolved node index number
	long long Prev = -1; // the index number of the previous sibling for this node
	long long Next = -1; // the index number of the next sibling for this node
	long long Parent = -1; // the index number of the parent for this node
	long long Id = 0; // externally assigned node ID
	long long Level = 0; // the branch level for this node

	// each parent node must handle these variables
	long long Children = 0; // the number of children underneath this node
	long long Child = -1; // the index number of the first child
	long long Last = -1; // the index number of the last child

	// constructor
	Node(bool free, long long index, long long prev, long long next, long long parent, long long id, long long level, long long children, long long child, long long last){
		Free = free;
		Index = index;
		Prev = prev;
		Next = Next;
		Parent = parent;
		Id = id;
		Level = level;
		Children = children;
		Child = child;
		Last = last;
	};

};

// tree class
class Tree{

	public:

	bool Initialized = false; // the tree was initialized
	long long maxLevel = 100000; // the maximum tree level
	long long freeIndex = -1; // the next available free-node index number in the deadpool to reuse
	long long freeCount = 0; // the number of nodes that are free to reuse in the deadpool
	string Name = "Tree"; // the name of the tree
	vector<Node> Nodes; // vector node array

	// constructor
	Tree(string name){
		Init(name);
	}

	// destructor
	~Tree(){
		try{Clear();}catch(...){return;}
	}

	// add a new node as the last child under the parent node
	long long nodeNew(long long parent, long long id){

		long long Index = -1;

		// handled as the root node
		if(parent < 0){

			// validate the tree
			if((long long)Nodes.size() > 0) throw cExBadValue(__FILE__, __FUNCTION__, __LINE__, "parent", parent);

			// create the new node
			try{Nodes.push_back(Node(false, 0, 0, 0, -1, id, 0, 0, -1, -1));}catch(...){throw cExBadAlloc(__FILE__, __FUNCTION__, __LINE__, "Nodes.size", (long long)Nodes.size());}

			return 0;

		}

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// parent preconditions
			if(parent < 0 || parent >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "parent", parent, 0, (long long)Nodes.size() - 1);
			if(Nodes[parent].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "parent", parent, "Free");

		}

		// create a new node if there are no free nodes in the deadpool
		if(freeCount < 1){
			Index = (long long)Nodes.size();
			try{Nodes.push_back(Node(false, Index, -1, -1, -1, id, -1, 0, -1, -1));}catch(...){throw cExBadAlloc(__FILE__, __FUNCTION__, __LINE__, "Nodes.size", (long long)Nodes.size());}
		}

		// reuse a free node from the deadpool
		else{

			// adjust the deadpool
			if(freeIndex < 1 || freeIndex >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "freeIndex", freeIndex, 1, (long long)Nodes.size() - 1);
			Index = freeIndex;
			freeCount--;
			freeIndex = -1;
			if(freeCount > 0){
				freeIndex = Nodes[Index].Prev;
				if(freeIndex < 1 || freeIndex >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "freeIndex", freeIndex, 1, (long long)Nodes.size() - 1);
			}

			// default the node values
			Nodes[Index].Index = Index;
			Nodes[Index].Prev = -1;
			Nodes[Index].Next = -1;
			Nodes[Index].Id = id;
			Nodes[Index].Children = 0;
			Nodes[Index].Child = -1;
			Nodes[Index].Last = -1;

		}

		// place node as the last child node under the parent
		Nodes[Index].Parent = parent;
		Nodes[Index].Level = Nodes[parent].Level + 1; if(Nodes[Index].Level > maxLevel) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Nodes[Index].Level", Nodes[Index].Level, 0, maxLevel);

		// add the first child
		if(Nodes[parent].Children < 1){
			Nodes[parent].Child = Index;
		}

		// add the last child
		else{

			// check if the reference is valid
			Nodes[Index].Prev = Nodes[parent].Last; if(Nodes[Index].Prev < 0 || Nodes[Index].Prev >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Nodes[Index].Prev", Nodes[Index].Prev, 0, (long long)Nodes.size() - 1);

			// adjust the prev node values to reflect the new node
			Nodes[Nodes[Index].Prev].Next = Index;

		}

		// adjust the parent node values to reflect the new node
		Nodes[parent].Children++;
		Nodes[parent].Last = Nodes[Index].Index;

		// finalize
		Nodes[Index].Free = false;
		return Index;

	}

	// delete a node and all nodes underneath it (recursively)
	void nodeDeleteAll(long long index){

		long long Index = -1;
		long long Prev = -1;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// index preconditions
			if(index < 1 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 1, (long long)Nodes.size());
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");

		}

		// delete every child node underneath this node (recursively)
		Index = -1; Prev = -1;
		do{
			Index = nodePrev(index, Index);
			if(Index < 0) continue;
			if(Prev > 0) nodeDelete(Prev);
			Prev = Index;
		}while(Index > -1);
		if(Prev > 0) nodeDelete(Prev);

		// delete this node
		nodeDelete(index);

	}

	// delete a leaf node
	void nodeDelete(long long index){

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// index preconditions
			if(index < 1 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 1, (long long)Nodes.size());
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");
			if(Nodes[index].Children > 0) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Nodes[index].Children", Nodes[index].Children, "Not Empty");
			if(Nodes[index].Parent < 0 || Nodes[index].Parent >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Nodes[index].Parent", Nodes[index].Parent, 0, (long long)Nodes.size());

			// deadpool preconditions
			if(freeCount > 0 && freeIndex < 1) throw cExBadValue(__FILE__, __FUNCTION__, __LINE__, "freeIndex", freeIndex);
			if(freeIndex >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "freeIndex", freeIndex, 1, (long long)Nodes.size());

		}

		// break the node associations
		nodeRemove(index);

		// add the node to the deadpool
		if(freeCount < 1 || freeIndex < 1){
			Nodes[index].Prev = -1;
			freeCount = 1;
		}
		else{
			Nodes[freeIndex].Next = index;
			Nodes[index].Prev = freeIndex;
			freeCount++;
		}
		freeIndex = index;
		Nodes[index].Next = -1;
		Nodes[index].Free = true;

	}

	// break the associations of a node
	void nodeRemove(long long index){

		long long Index = -1;
		long long Parent = -1;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// index preconditions
			if(index < 1 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 1, (long long)Nodes.size());
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");

			// index parent preconditions
			Parent = Nodes[index].Parent; if(Parent < 0 || Parent >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Parent", Parent, 0, (long long)Nodes.size());
			if(Nodes[Parent].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Parent", Parent, "Free");

		}

		// disassociate the node if it is an only child
		if(Nodes[Parent].Children == 1){

			// disassociate the node from the parent
			Nodes[Parent].Children = 0;
			Nodes[Parent].Child = -1;
			Nodes[Parent].Last = -1;

		}

		// disassociate the node if it was the last child
		else if(index == Nodes[Parent].Last){

			// disassociate the node from the previous node
			Index = Nodes[index].Prev; if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
			Nodes[Index].Next = -1;

			// disassociate the node from the parent
			Nodes[Parent].Children--;
			Nodes[Parent].Last = Nodes[index].Prev;

		}

		// disassociate the node if it was the first child
		else if(index == Nodes[Parent].Child){

			// disassociate the node from the next node
			Index = Nodes[index].Next; if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
			Nodes[Index].Prev = -1;

			// disassociate the node from the parent
			Nodes[Parent].Children--;
			Nodes[Parent].Child = Nodes[index].Next;

		}

		// disassociate the node if it is a child
		else if(index != Nodes[Parent].Child && index != Nodes[Parent].Last){

			// disassociate the node from the previous node
			Index = Nodes[index].Prev; if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
			Nodes[Index].Next = Nodes[index].Next;

			// disassociate the node from the next node
			Index = Nodes[index].Next; if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
			Nodes[Index].Prev = Nodes[index].Prev;

			// disassociate the node from the parent
			Nodes[Parent].Children--;

		}

	}

	// get the next node recursively
	long long nodeNext(long long index, long long prev){

		long long Index = -1;
		long long Parent = -1;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// general preconditions
			if((long long)Nodes.size() < 2) return -1;

			// index preconditions
			if(index < 0 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 0, (long long)Nodes.size());
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");
			if(Nodes[index].Children < 1) return -1;

			// prev preconditions
			if(prev > 0) Index = prev; else Index = index;
			if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");

			// index parent preconditions
			Parent = Nodes[Index].Parent; if(Parent < 0) Parent = 0;
			if(Parent >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Parent", Parent, 0, (long long)Nodes.size());
			if(Nodes[Parent].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Parent", Parent, "Free");
			if(Nodes[Parent].Last < 0 || Nodes[Parent].Last >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Nodes[Parent].Last", Nodes[Parent].Last, 0, (long long)Nodes.size());

		}

		// get the first child of the current node
		if(Nodes[Index].Children > 0) return Nodes[Index].Child;

		// finish the process
		if(Index == index) return -1;

		// get the next node
		if(Index != Nodes[Parent].Last) return Nodes[Index].Next;

		// walk back to the next (non-last) node
		while(Index == Nodes[Parent].Last && Index > 0 && Index < (long long)Nodes.size() && Parent > 0){
			Index = Parent;
			if(Index == index) return -1;
			Parent = Nodes[Index].Parent;
		}

		// get the next node
		return Nodes[Index].Next;

	}

	// recursively return the leaf nodes from the last leaf node to the index node
	long long nodePrev(long long index, long long prev){

		long long Index = -1;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// general preconditions
			if((long long)Nodes.size() < 2) return -1;

			// index preconditions
			if(index < 0 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 0, (long long)Nodes.size());
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");
			if(Nodes[index].Children < 1) return -1;

			// prev preconditions
			if(prev > 0) Index = prev; else Index = index;
			if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());

		}

		// handle subsequent calls
		if(Index != index){

			// get the current node
			Index = prev; if(Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");

			// get the previous node
			if(Nodes[Index].Prev > -1){
				Index = Nodes[Index].Prev; if(Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
				if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
			}

			// get the parents previous node
			else{
				Index = Nodes[Index].Parent; if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
				if(Index == index) return -1;
				if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
				return Index;
			}

		}

		// get the last leaf node
		while(Nodes[Index].Children > 0){
			Index = Nodes[Index].Last; if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
		}

		// return the index or finalize
		if(Index == index) return -1; else return Index;

	}

	// offset the level of the index node and every node underneath
	void nodeLevelSet(long long index, long long level){

		long long Index = -1;
		long long Offset = -1;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// index preconditions
			if(index < 1 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 1, (long long)Nodes.size());
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");

			// level preconditions
			if(level < 1) throw cExBadValue(__FILE__, __FUNCTION__, __LINE__, "level", level);

		}

		// offset the level of the index node and every node underneath
		Offset = level - Nodes[index].Level;
		Index = index;
		do{
			if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");
			Nodes[Index].Level += Offset;
			if(Index == index) Index = -1;
			Index = nodeNext(index, Index);
		} while(Index > 0 && Index < (long long)Nodes.size());

	}

	// move a node to the previous node before the target
	void nodeMoveUp(long long index, long long target){

		long long Index = -1;
		long long Parent = -1;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// index preconditions
			if(index < 1 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 1, (long long)Nodes.size());
			if(index == target) throw cExBadValue(__FILE__, __FUNCTION__, __LINE__, "index", index);
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");

			// target preconditions
			if(target < 1 || target > (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "target", target, 1, (long long)Nodes.size());
			if(Nodes[target].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "target", target, "Free");

			// target parent preconditions
			Parent = Nodes[target].Parent; if(Parent < 0 || Parent >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Parent", Parent, 0, (long long)Nodes.size());
			if(Nodes[Parent].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Parent", Parent, "Free");

		}

		// break the node associations
		nodeRemove(index);

		// set the new associations
		Nodes[Parent].Children++;
		Nodes[index].Parent = Parent;
		Nodes[index].Next = target;
		Nodes[index].Prev = -1;
		if(target == Nodes[Parent].Child) Nodes[Parent].Child = index; else Nodes[index].Prev = Nodes[target].Prev;
		Index = Nodes[target].Prev;
		Nodes[target].Prev = index;
		if(Index > 0){
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
			Nodes[Index].Next = index;
		}

		// offset the node levels
		nodeLevelSet(index, Nodes[Parent].Level + 1);

	}

	// move a node to the next node after the target
	void nodeMoveDown(long long index, long long target){

		long long Parent = -1;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// index preconditions
			if(index < 1 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 1, (long long)Nodes.size());
			if(index == target) throw cExBadValue(__FILE__, __FUNCTION__, __LINE__, "index", index);
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");

			// target preconditions
			if(target < 1 || target > (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "target", target, 1, (long long)Nodes.size());
			if(Nodes[target].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "target", target, "Free");

			// target parent preconditions
			Parent = Nodes[target].Parent; if(Parent < 0 || Parent >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Parent", Parent, 0, (long long)Nodes.size());
			if(Nodes[Parent].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Parent", Parent, "Free");

		}

		// break the node associations
		nodeRemove(index);

		// set the new associations
		Nodes[Parent].Children++;
		Nodes[index].Parent = Parent;
		Nodes[index].Prev = target;
		Nodes[index].Next = -1;
		if(target == Nodes[Parent].Last) Nodes[Parent].Last = index; else Nodes[index].Next = Nodes[target].Next;
		Nodes[target].Next = index;

		// offset the node levels
		nodeLevelSet(index, Nodes[Parent].Level + 1);

	}

	// move a node to the last child of the target
	void nodeMove(long long index, long long target){

		long long Index = -1;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// index preconditions
			if(index < 1 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 1, (long long)Nodes.size());
			if(index == target) throw cExBadValue(__FILE__, __FUNCTION__, __LINE__, "index", index);
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");

			// target preconditions
			if(target < 0 || target > (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "target", target, 0, (long long)Nodes.size());
			if(Nodes[target].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "target", target, "Free");

		}

		// check if the target is somewhere underneath the index
		do{
			Index = nodeNext(index, Index);
			if(Index == index) throw cExBadValue(__FILE__, __FUNCTION__, __LINE__, "index", index);
		} while(Index > 0 && Index < (long long)Nodes.size());

		// break the node associations
		nodeRemove(index);

		// set the new associations
		Nodes[target].Children++;
		Nodes[index].Parent = target;
		Nodes[index].Next = -1;
		Nodes[index].Prev = -1;
		if(Nodes[target].Children == 1){
			Nodes[target].Last = index;
			Nodes[target].Child = index;
		}
		else{
			Index = Nodes[target].Last; if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
			Nodes[target].Last = index;
			Nodes[Index].Next = index;
			Nodes[index].Prev = Index;
		}

		// offset the node levels
		nodeLevelSet(index, Nodes[target].Level + 1);

	}

	// copy the entire node structure as the last child under the target node
	void nodeCopy(long long index, long long target){

		long long Index = -1;
		long long Parent = -1;
		long long Level = -1;
		long long pLevel = -1;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// index preconditions
			if(index < 1 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 1, (long long)Nodes.size());
			if(index == target) throw cExBadValue(__FILE__, __FUNCTION__, __LINE__, "index", index);
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");

			// target preconditions
			if(target < 0 || target > (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "target", target, 0, (long long)Nodes.size());
			if(Nodes[target].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "target", target, "Free");

		}

		// check if the target is somewhere underneath the index
		do{
			Index = nodeNext(index, Index);
			if(Index == index) throw cExBadValue(__FILE__, __FUNCTION__, __LINE__, "index", index);
		} while(Index > 0 && Index < (long long)Nodes.size());

		// copy each node to the target
		Index = index;
		Level = Nodes[index].Level;
		Parent = target;
		do{

			// get the current node
			if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");

			// move up levels
			if(Nodes[Index].Level < Level){
				for(pLevel = Level; pLevel > Nodes[Index].Level; pLevel--){
					Parent = Nodes[Parent].Parent;
					if(Nodes[Parent].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Parent", Parent, "Free");
				}
				Level = Nodes[Index].Level;
			}

			// move down one level
			if(Nodes[Index].Level > Level){
				Parent = Nodes[Parent].Last;
				if(Nodes[Parent].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Parent", Parent, "Free");
				Level = Nodes[Index].Level;
			}

			// copy the node
			nodeNew(Parent, Nodes[Index].Id);

			// signal the processing of the next nodes
			if(Index == index) Index = -1;

			// get the next node
			if(Index != index) Index = nodeNext(index, Index);

		} while(Index > 0 && Index < (long long)Nodes.size());

	}

	// save the entire tree structure under the index to a file
	void Save(string filename, long long index){

		long long Level = 0;
		long long Index = -1;
		ofstream File;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// filename preconditions
			if(filename == "") throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "filename", 0, "Empty");

			// index preconditions
			if(index < 0 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 0, (long long)Nodes.size());
			if(Nodes[index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "index", index, "Free");

		}

		// open the file
		try{File.open(filename);}catch(...){throw cExFileOpen(__FILE__, __FUNCTION__, __LINE__, filename, 0);}

		// save the tree
		try{

			// save the tree name
			File << Name << endl;

			// save every node
			Index = index;
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
			Level = Nodes[Index].Level;
			do{

				// get the current node
				if(Index < 0 || Index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "Index", Index, 0, (long long)Nodes.size());
				if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");

				// write the node
				File << (Nodes[Index].Level - Level) << " " << Nodes[Index].Id << endl;

				// get the next node
				if(Index == index) Index = -1;

				// get the next node
				if(Index != index) Index = nodeNext(index, Index);

			} while(Index > 0 && Index < (long long)Nodes.size());

		}catch(...){
			try{File.close();}catch(...){}
			throw;
		}

		// close the file
		try{File.close();}catch(...){throw cExFileClose(__FILE__, __FUNCTION__, __LINE__, filename, 0);}

	}

	// load the entire tree structure from a file
	void Load(string filename){

		long long Id = 0;
		long long Level = 0;
		long long Index = -1;
		istringstream sLine;
		string Line = "";
		ifstream File;

		// preconditions
		if(Initialized == true){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Initialized");}else{

			// filename preconditions
			if(filename == "") throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "filename", 0, "Empty");

		}

		// open the file
		try{File.open(filename);}catch(...){throw cExFileOpen(__FILE__, __FUNCTION__, __LINE__, filename, 0);}

		// load the tree
		try{

			// load the name
			getline(File, Line);
			Name = Line;

			// load the root node
			getline(File, Line);
			sLine.str(Line);
		    Level = 0; Id = 0;
		    if(!(sLine >> Level >> Id)) throw cExFileRead(__FILE__, __FUNCTION__, __LINE__, filename, 0);
			nodeNew(-1, Id);
			Initialized = true;
			Index = 0;

			// process each line
			while(getline(File, Line)){

				// get the next line
				sLine.clear();
				sLine.str(Line);

			    // get the values
			    Level = 0; Id = 0;
			    if(!(sLine >> Level >> Id)) throw cExFileRead(__FILE__, __FUNCTION__, __LINE__, filename, 0);

				// move down
				if(Level > Nodes[Index].Level + 1 && Nodes[Index].Last > 0){
					Index = Nodes[Index].Last;
				}

				// move up
				else if(Level <= Nodes[Index].Level){
					while(Level <= Nodes[Index].Level) Index = Nodes[Index].Parent;
				}

				// create the new node
				nodeNew(Index, Id);

			}

		}catch(...){
			try{File.close();}catch(...){}
			throw;
		}

		// close the file
		try{File.close();}catch(...){throw cExFileClose(__FILE__, __FUNCTION__, __LINE__, filename, 0);}

	}

	// load a tree structure under a specified index
	void Import(string filename, long long index){

		long long Id = 0;
		long long Level = 0;
		long long Index = -1;
		long long pLevel = 0;
		string Line = "";
		istringstream sLine;
		ifstream File;

		// preconditions
		if(Initialized == false){throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized, "Not Initialized");}else{

			// filename preconditions
			if(filename == "") throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "filename", 0, "Empty");

			// index preconditions
			Index = index; if(index < 0 || index >= (long long)Nodes.size()) throw cExBadRange(__FILE__, __FUNCTION__, __LINE__, "index", index, 0, (long long)Nodes.size());
			if(Nodes[Index].Free == true) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Index", Index, "Free");
			pLevel = Nodes[Index].Level;

		}

		// open the file
		try{File.open(filename);}catch(...){throw cExFileOpen(__FILE__, __FUNCTION__, __LINE__, filename, 0);}

		// load the tree
		try{

			// ignore the name
			getline(File, Line);

			// process each line
			while(getline(File, Line)){

				// get the next line
				sLine.clear();
				sLine.str(Line);

			    // get the values
			    Level = 0; Id = 0;
			    if(!(sLine >> Level >> Id)) throw cExFileRead(__FILE__, __FUNCTION__, __LINE__, filename, 0);
				Level += pLevel + 1;

				// move down
				if(Level > Nodes[Index].Level + 1 && Nodes[Index].Last > -1){
					Index = Nodes[Index].Last;
				}

				// move up
				else if(Level <= Nodes[Index].Level){
					while(Level <= Nodes[Index].Level) Index = Nodes[Index].Parent;
				}

				// create the new node
				nodeNew(Index, Id);

			}

		}catch(...){
			try{File.close();}catch(...){}
			throw;
		}

		// close the file
		try{File.close();}catch(...){throw cExFileClose(__FILE__, __FUNCTION__, __LINE__, filename, 0);}

	}

	// clear the tree
	void Clear(){

		// preconditions
		if(Initialized == false) return;

		// set the default values
		freeIndex = -1;
		freeCount = 0;
		Name = "";

		// delete the vector array
		try{Nodes.clear();}catch(...){return;}

		// finalize
		Initialized = false;

	}

	// initialize the tree
	void Init(string name){

		// preconditions
		if(Initialized == true) throw cExAlreadyInit(__FILE__, __FUNCTION__, __LINE__, "Tree", Initialized);

		// set the name
		Name = name;

		// set the default values
		freeIndex = -1;
		freeCount = 0;

		// create the root node
		nodeNew(-1, 0);

		// finalize
		Initialized = true;

	}

	// print the tree
	void Print(string filename){

		int Width = 12;
		long long Count = 0;
		long long Index = -1;
		ofstream File;

		// preconditions
		if(filename == "") return;

		// open the file
		try{File.open(filename);}catch(...){return;}

		// write the data
		try{

			// write the header information
			File << "TREE" << endl;
			File << endl << "       Name: " << Name;
			File << endl << " Node Count: " << (long long)Nodes.size();
			File << endl << " Free Index: " << freeIndex;
			File << endl << " Free Count: " << freeCount;
			File << endl << "Initialized: " << Initialized;
			File << endl;
			File << endl << "NODE STRUCTURE";
			File << endl;
			if(Initialized == false) throw cExBadState(__FILE__, __FUNCTION__, __LINE__, "Initialized", Initialized, "Not Initialized");

			// start with the root node
			File << endl << "0:" << Nodes[0].Id;
			Index = nodeNext(0, -1);
			Count = 1;

			// recursively print each node at the correct indentation level for the structure
			while(Index > 0 && Count < (long long)Nodes.size() && Index < (long long)Nodes.size()){

				// get the node
				if(Nodes[Index].Level < 0 || Nodes[Index].Level > maxLevel) break;

				// print the node
				File << endl << string(Nodes[Index].Level, '-');
				if(Nodes[Index].Free == true) File << "*";
				File << Index << ":" << Nodes[Index].Id;
				Count++;

				// get the next node
				try{Index = nodeNext(0, Index);}catch(...){break;}

			}

			// print the raw node list
			File << endl;
			File << endl << "NODE LIST";
			File << endl;
			File << right << setw(Width) << "index" << " |" << setw(Width) << "Id" << " |" << setw(Width) << "Index" << " |" << setw(Width) << "Level" << " |" << setw(Width) << "Free" << " |" << setw(Width) << "Prev" << " |" << setw(Width) << "Next" << " |" << setw(Width) << "Parent" << " |" << setw(Width) << "Child" << " |" << setw(Width) << "Last" << " |" << setw(Width) << "Children";
			for(Index = 0; Index < (long long)Nodes.size(); Index++){
				File << endl << string(Width, '-') << "-+" << string(Width, '-') << "-+" << string(Width, '-') << "-+" << string(Width, '-') << "-+" << string(Width, '-') << "-+" << string(Width, '-') << "-+" << string(Width, '-') << "-+" << string(Width, '-') << "-+" << string(Width, '-') << "-+" << string(Width, '-') << "-+" << string(Width, '-');
				File << endl << right << setw(Width) << Index << " |" << setw(Width) << Nodes[Index].Id << " |" << setw(Width) << Nodes[Index].Index << " |" << setw(Width) << Nodes[Index].Level << " |" << setw(Width) << Nodes[Index].Free << " |" << setw(Width) << Nodes[Index].Prev << " |" << setw(Width) << Nodes[Index].Next << " |" << setw(Width) << Nodes[Index].Parent << " |" << setw(Width) << Nodes[Index].Child << " |" << setw(Width) << Nodes[Index].Last << " |" << setw(Width) << Nodes[Index].Children;
			}

			// print the deadpool list
			File << endl;
			File << endl << "DEADPOOL LIST";
			File << endl;
			Index = freeIndex;
			while(Index > 0){
				File << endl << Index << ":" << Nodes[Index].Id;
				Index = Nodes[Index].Prev;
			}

		}catch(...){
			try{File.close();}catch(...){return;}
			return;
		}

		// close the file
		try{File.close();}catch(...){return;}

	}

};

#endif
