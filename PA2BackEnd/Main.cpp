#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <set>
#include <sstream>

using namespace std;

vector<string> tokenize(string input) {
	stringstream ss(input);
	istream_iterator<std::string> begin(ss);
	istream_iterator<std::string> end;
	vector<std::string> vstrings(begin, end);
	return vstrings;
}


int main(int argc, char* argv[])
{
	if (argc < 2) {
		cout << "usage: pa2BackEnd.exe inputfile.txt" << endl;
		exit(EXIT_FAILURE);
	}

	//read input 
	ifstream inputFile(argv[1]);
	map<size_t, bool> values;
	map<size_t, pair<string, size_t>> key;

	if (inputFile.is_open())
	{
		bool atKeys = false;
		string line;
		while (getline(inputFile, line))
		{
			if (line == "Keys:") {
				//skip
				atKeys = true;
			}
			else {
				auto parts = tokenize(line);
				if (atKeys) {
					key[atoi(parts[0].c_str())] = make_pair(parts[1], atoi(parts[2].c_str()));
				}
				else {
					if (parts[1] == "T") {
						values[atoi(parts[0].c_str())] = true;
					}
					else {
						values[atoi(parts[0].c_str())] = false;
					}
				}
			}
		}
	}
	else {
		cout << "could not open input file" << endl;
		exit(EXIT_FAILURE);
	}
	inputFile.close();

	//find path
	map<size_t, string> solution;
	for (auto curr : key) {
		if (values[curr.first]) {
			solution[curr.second.second] = curr.second.first;
		}
	}

	//write output
	ofstream output;
	output.open("beoutput.txt");
	if (!output.is_open()) {
		cout << "Couldn't open output file" << endl;
		exit(EXIT_FAILURE);
	}

	for (auto step : solution) {
		output << "Step " << to_string(step.first) << ": Node " << step.second << endl;
	}
	output.close();
}