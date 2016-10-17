#pragma once
#include <set>
#include <string>
#include <vector>

using namespace std;
class Node
{
public:
	Node(string inName, set<string> inTreasures, set<string> inTolls, vector<string> inNeighbors);
	Node();
	~Node();

	const string getName();
	const set<string>& getTreasures();
	const set<string>& getTolls();
	const vector<string>& getNeighbors();

private:
	const string name;
	const set<string> treasures;
	const set<string> tolls;
	const vector<string> neighbors;
};

