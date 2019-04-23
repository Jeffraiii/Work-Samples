#include "Parts.h"
#include <iostream>
#include <string>

struct Node
{
	std::string mName;

	void Print()
	{
		std::cout << mName;
	}
};

struct NodePrinter
{
public:
	NodePrinter(Node* n)
	{
        mNode = n;
	}

	void Print()
	{
		mNode->Print();
	}
private:
    Node* mNode;
};

void Part1()
{
	Node* node = new Node();
	node->mName = "Sanjay";

	NodePrinter printer(node);
	printer.Print();
}
