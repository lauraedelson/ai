#include "Node.h"

Node::Node(string inName, set<string> inTreasures, set<string> inTolls, vector<string> inNeighbors):name(inName), treasures (inTreasures), tolls(inTolls), neighbors(inNeighbors){}

Node::Node() {}

Node::~Node()
{
}

const set<string>& Node::getTreasures()
{
	return treasures;
}

const set<string>& Node::getTolls()
{
	return tolls;
}

const string Node::getName()
{
	return name;
}

const vector<string>& Node::getNeighbors()
{
	return neighbors;
}
