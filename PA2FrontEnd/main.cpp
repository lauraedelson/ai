#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

#include "Node.h"

using namespace std;

static string AT = "@";
static string HAS = "H";
static string AVAILABLE = "A";

struct atoms
{
	set<pair<string, int>> ats;
	vector<pair<string, int>> hass;
	vector<pair<string, int>> availables;
};

//child method for build_atoms recursion
atoms build_atoms(map<string, Node>& nodes, string node_name, size_t steps) {
	atoms currAtoms;
	Node currNode = nodes[node_name];
	currAtoms.ats.insert(make_pair(currNode.getName(), steps));

	if (steps < 1) {
		return currAtoms;
	}

	vector<string> neighbors = currNode.getNeighbors();
	for (vector<string>::iterator child = neighbors.begin(); child != neighbors.end(); child++) {
		atoms child_atoms = build_atoms(nodes, *child, steps - 1);
		currAtoms.ats.insert(child_atoms.ats.begin(), child_atoms.ats.end());
	}
	return currAtoms;
}

//given a start node and a collection of treasures, build all the atoms
atoms build_atoms(map<string, Node>& nodes, string node_name, size_t steps, vector<string>& treasures) {
	atoms currAtoms;
	for (vector<string>::iterator treasure = treasures.begin(); treasure != treasures.end(); treasure++) {
		for (size_t step = 0; step <= steps; step++) {
			currAtoms.availables.push_back(make_pair(*treasure, step));
			currAtoms.hass.push_back(make_pair(*treasure, step));
		}
	}

	Node currNode = nodes[node_name];
	currAtoms.ats.insert(make_pair(currNode.getName(), steps));

	if (steps < 1) {
		return currAtoms;
	}

	vector<string> neighbors = currNode.getNeighbors();
	for (vector<string>::iterator child = neighbors.begin(); child != neighbors.end(); child++ ) {
		atoms child_atoms = build_atoms(nodes, *child, steps - 1);
		currAtoms.ats.insert(child_atoms.ats.begin(), child_atoms.ats.end());
	}
	return currAtoms;
}

//given a space delimited string, return a vector of the parts
vector<string> tokenize(string input) {
	stringstream ss(input);
	istream_iterator<string> begin(ss);
	istream_iterator<string> end;
	vector<string> vstrings(begin, end);
	return vstrings;
}

	int main(int argc, char* argv[])
	{
		if (argc < 2) {
			cout << "usage: pa2frontend inputfile.txt" << endl;
			exit(EXIT_FAILURE);
		}

		//read input 
		ifstream inputFile(argv[1]);
		vector<string> nodeNames;
		vector<string> treasures;
		int steps;
		map<string, Node> nodes;
		if (inputFile.is_open())
		{
			string line;
			getline(inputFile, line);
			nodeNames = tokenize(line);
			getline(inputFile, line);
			treasures = tokenize(line);
			getline(inputFile, line);
			steps = atoi(line.c_str());
			while (getline(inputFile, line))
			{
				vector<string> pieces = tokenize(line);
				string name = pieces[0];
				set<string> nodeTreasures, nodeTolls;
				vector<string> connections;
				vector<string>::iterator treasures_loc = find(pieces.begin(), pieces.end(), "TREASURES");
				vector<string>::iterator tolls_loc = find(pieces.begin(), pieces.end(), "TOLLS");
				vector<string>::iterator next_loc = find(pieces.begin(), pieces.end(), "NEXT");
				treasures_loc++;
				while (treasures_loc < tolls_loc)
				{
					nodeTreasures.insert(*treasures_loc);
					treasures_loc++;
				}
				tolls_loc++;
				while (tolls_loc < next_loc)
				{
					nodeTolls.insert(*tolls_loc);
					tolls_loc++;
				}
				next_loc++;
				while (next_loc < pieces.end()) {
					connections.push_back(*next_loc);
					next_loc++;
				}

				Node newNode = Node(name, nodeTreasures, nodeTolls, connections);
				nodes.insert(pair<string, Node>(name, newNode));
			}
			inputFile.close();
		}
		else {
			cout << "Unable to open file" << endl;
			exit(EXIT_FAILURE);
		}
		
		//navigate graph to create our atoms
		vector<string> stuff;
		set<pair<string, string>> ats, hass, availables;
		//find start node
		string startNode;
		string goalNode;
		for (vector<string>::iterator name = nodeNames.begin(); name != nodeNames.end(); name++) {
			string capsName = *name;
			for (size_t i = 0; i < capsName.size(); i++) {
				capsName[i] = toupper(capsName[i]);
			}
			if (capsName == "START") {
				startNode = *name;
			}
			if (capsName == "GOAL") {
				goalNode = *name;
			}
		}
		atoms all_atoms = build_atoms(nodes, startNode, steps, treasures);
		vector<pair<string, int>> at_vec, has_vec, avail_vec;
		copy(all_atoms.ats.begin(), all_atoms.ats.end(), back_inserter(at_vec));
		has_vec = all_atoms.hass;
		avail_vec = all_atoms.availables;

		//create propositions and write to output
		set<string> prepositions;
		vector<string> keys;
		size_t i = 0;
		for (i; i < at_vec.size(); i++) {
			string curr_name = at_vec[i].first;
			int curr_step = steps - at_vec[i].second;
			// 1 - can't be in two places at once
			for (vector<string>::iterator name = nodeNames.begin(); name != nodeNames.end(); name++) {
				if (*name != curr_name) {
					vector<pair<string, int>>::iterator at_it = find(at_vec.begin() + i, at_vec.end(), make_pair(*name, steps - curr_step));
					if (at_it != at_vec.end()) {
						int statement_id = i;
						prepositions.insert("-" + to_string((long long unsigned int)statement_id) + " -" + to_string((long long unsigned int)distance(at_vec.begin(), at_it)));
					}
				}
			}

			//3 - The player must move on edges
			string statement;
			vector<string> neighbors = nodes[curr_name].getNeighbors();
			for (vector<string>::iterator neighbor_name = neighbors.begin(); neighbor_name != neighbors.end(); neighbor_name++) {
				vector<pair<string, int>>::iterator at_it = find(at_vec.begin(), at_vec.end(), make_pair(*neighbor_name, (steps - curr_step) + 1));
				if (at_it != at_vec.end()) {
					statement.append(to_string((long long unsigned int)distance(at_vec.begin(), at_it)) + " ");
				}
			}
			if (!statement.empty()) {
				statement.append("-" + to_string((long long unsigned int)i));
				prepositions.insert(statement);
			}

			//11 - player at start at time 0
			if (curr_name == startNode && curr_step == 0) {
				prepositions.insert(to_string((long long unsigned int)i));
			}

			//13 - player at goal at finish
			if (curr_name == goalNode && curr_step == steps) {
				prepositions.insert(to_string((long long unsigned int)i));
			}

			//write key
			keys.push_back(to_string((long long unsigned int)i) +" " + curr_name + " " + to_string((long long unsigned int)curr_step));

		}
		for (i; i < at_vec.size() + has_vec.size(); i++ ) {
			size_t index = i - at_vec.size();
			string curr_treasure = has_vec[index].first;
			int curr_step = has_vec[index].second;
			//2 - if a player has an item, at the time he has it, it is not available
			vector<pair<string, int>>::iterator it = find(avail_vec.begin(), avail_vec.end(), make_pair(curr_treasure, curr_step));
			if (it != avail_vec.end()) {
				prepositions.insert("-" + to_string((long long unsigned int)i) + " " + "-" + to_string((long long unsigned int)distance(avail_vec.begin(), it) + at_vec.size() + has_vec.size()));
			}

			for (map<string, Node>::iterator curr_node = nodes.begin(); curr_node != nodes.end(); curr_node++) {
				if (curr_node->second.getTolls().find(curr_treasure) != curr_node->second.getTolls().end()) {
					//6 - If node N has toll T and the player is at N at time I, then the player no longer has T at time I. 
					vector<pair<string, int>>::iterator at_now_it = find(at_vec.begin(), at_vec.end(), make_pair(curr_node->first, steps - curr_step));
					if (at_now_it != at_vec.end()) {
						prepositions.insert("-" + to_string((long long unsigned int)distance(at_vec.begin(), at_now_it)) + " -" + to_string((long long unsigned int)i));
					}

					//4 - If node N has toll T and the player is at N at time I+1, then the player must have T at time I
					vector<pair<string, int>>::iterator at_it = find(at_vec.begin(), at_vec.end(), make_pair(curr_node->first, steps - (curr_step + 1)));
					if (at_it != at_vec.end()) {
						prepositions.insert("-" + to_string((long long unsigned int)distance(at_vec.begin(), at_it)) + " " + to_string((long long unsigned int)i));
					}
				}
			}


			vector<pair<string, int>>::iterator has_it, available_it;
			has_it = find(has_vec.begin(), has_vec.end(), make_pair(curr_treasure, curr_step + 1));
			available_it = find(avail_vec.begin(), avail_vec.end(), make_pair(curr_treasure, curr_step));
			if (has_it != has_vec.end()) {

				//9 - If treasure T has been spend at time I, then the player does not have it at time I+1.
				if (available_it != avail_vec.end()) {
					prepositions.insert(to_string((long long unsigned int)distance(avail_vec.begin(), available_it) + at_vec.size() + has_vec.size()) + " " + to_string((long long unsigned int)i) + " -" + to_string((long long unsigned int)distance(has_vec.begin(), has_it) + at_vec.size()));
				}

				//10 - If the player has treasure T at time I and is at node N at time I+1, and N does not require T as a toll, then the player still has T at I+1. 
				for (map<string, Node>::iterator curr_node = nodes.begin(); curr_node != nodes.end(); curr_node++) {
					if (curr_node->second.getTreasures().find(curr_treasure) != curr_node->second.getTreasures().end()) {
						vector<pair<string, int>>::iterator at_it = find(at_vec.begin(), at_vec.end(), make_pair(curr_node->first, steps - (curr_step + 1)));
						if (at_it != at_vec.end()) {
							prepositions.insert("-" + to_string((long long unsigned int)i) + " -" + to_string((long long unsigned int)distance(at_vec.begin(), at_it)) + " " + to_string((long long unsigned int)distance(has_vec.begin(), has_it) + at_vec.size()));
						}
					}
				}
			}

		}
		for (i; i < at_vec.size() + has_vec.size() + avail_vec.size(); i++) {
			size_t index = i - (at_vec.size() + has_vec.size());
			string curr_treasure = avail_vec[index].first;
			int curr_step = avail_vec[index].second;
			
			//5 -If treasure T is initially at node N and is available at time I and the player is at N at time I+1, then at time I+1 the player has T.
			vector<pair<string, int>>::iterator has_it = find(has_vec.begin(), has_vec.end(), make_pair(curr_treasure, curr_step + 1));
			if (has_it != has_vec.end()) {
				for (map<string, Node>::iterator curr_node = nodes.begin(); curr_node != nodes.end(); curr_node++) {
					if (curr_node->second.getTreasures().find(curr_treasure) != curr_node->second.getTreasures().end()) {
						vector<pair<string, int>>::iterator at_it = find(at_vec.begin(), at_vec.end(), make_pair(curr_node->first, steps - (curr_step + 1)));
						if (at_it != at_vec.end()) {
							prepositions.insert("-" + to_string((long long unsigned int)i) + " -" + to_string((long long unsigned int)distance(at_vec.begin(), at_it )) + " " + to_string((long long unsigned int)distance(has_vec.begin(), has_it) + at_vec.size()));
						}
					}
				}
			}


			vector<pair<string, int>>::iterator available_it = find(avail_vec.begin(), avail_vec.end(), make_pair(curr_treasure, curr_step + 1));
			if (available_it != avail_vec.end()) {
				//7 - If treasure T is available at I, and the player is at node N which is not the home of T at I+1, then T is available at I+1. 
				for (map<string, Node>::iterator curr_node = nodes.begin(); curr_node != nodes.end(); curr_node++) {
					set<string> currTreasures = curr_node->second.getTreasures();
					if (currTreasures.find(curr_treasure) == currTreasures.end()) {
						vector<pair<string, int>>::iterator at_it = find(at_vec.begin(), at_vec.end(), make_pair(curr_node->first, steps - (curr_step + 1)));
						if (at_it != at_vec.end()) {
							prepositions.insert("-" + to_string((long long unsigned int)i) + " -" + to_string((long long unsigned int)distance(at_vec.begin(), at_it)) + " " + to_string((long long unsigned int)distance(avail_vec.begin(), available_it) + at_vec.size() + has_vec.size()));
						}
					}
				}
				//8 - If treasure T is not available at time I, then it is not available at time I+1
				prepositions.insert(to_string((long long unsigned int)i) + " -" + to_string((long long unsigned int)distance(avail_vec.begin(), available_it) + at_vec.size() + has_vec.size()));
			}
			
			//12 - treasures all available at time 0
			if (curr_step == 0) {
				prepositions.insert(to_string((long long unsigned int)i));
			}
		}
		//write to output
		ofstream output;
		output.open("feoutput.txt");
		if (!output.is_open()) {
			cout << "Couldn't open output file" << endl;
			exit(EXIT_FAILURE);
		}
		for (set<string>::iterator preposition = prepositions.begin(); preposition != prepositions.end(); preposition++) {
			output << *preposition << endl;
		}
		output << "Keys:" << endl;
		for (vector<string>::iterator key = keys.begin(); key != keys.end(); key++) {
			output << *key << endl;
		}
		output.close();
	}