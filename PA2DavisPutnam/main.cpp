#include <fstream>
#include <string>
#include <iostream>
#include <iterator>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>:q!


using namespace std;

//given a space delimited string, return a vector of the parts
vector<string> tokenize(string input) {
	stringstream ss(input);
	istream_iterator<string> begin(ss);
	istream_iterator<string> end;
	vector<string> vstrings(begin, end);
	return vstrings;
}

//typedefs to make everything more readable
typedef pair<string, bool> literal;
typedef vector<literal> clause;

//return a set of clauses with the curr literal applied to the set of clauses passed in 
vector<clause> propagate(vector<clause> clauses, literal currLiteral) {
	vector<int> toRemove;
	for (size_t i = 0; i < clauses.size(); i++) {
		clause currClause = clauses[i];
		vector<int> toEdit;
		for (size_t j = 0; j < clauses[i].size(); j++) {
			if (clauses[i][j] == currLiteral) {
				toRemove.push_back(i);
			}
			literal opLiteral = make_pair(currLiteral.first, !currLiteral.second);
			if (clauses[i][j] == opLiteral) {
				toEdit.push_back(j);
			}
		}
		for (int j = toEdit.size() - 1; j >= 0; j--) {
			clauses[i].erase(clauses[i].begin() + toEdit[j]);
		}
	}
	for (int i = toRemove.size() - 1; i >= 0; i--) {
		clauses.erase(clauses.begin() + toRemove[i]);
	}
	return clauses;
}
map<string, bool> dp1(vector<clause> clauses, map<string, bool> atoms) {
	bool easyFound = false;
	do {
		easyFound = false;
		if (clauses.empty()) {
			return atoms;
		}

		vector<clause> candidates;
		for (vector<clause>::iterator currClause = clauses.begin(); currClause != clauses.end();  currClause++) {
			//if any clause is empty, we failed
			if (currClause->empty()) {
				return map<string, bool>();
			}

			//look for a forced assignment
			if (currClause->size() == 1) {
				candidates.push_back(*currClause);

			}
		}
 
		//do any forced assignments
		for (vector<clause>::iterator candidate = candidates.begin(); candidate != candidates.end(); candidate++) {
			easyFound = true;
			clause currCandidate = *candidate;
			if (atoms.find(currCandidate[0].first) != atoms.end()) {
				cout << "ERROR!" << endl;
				exit(EXIT_FAILURE);
			}

			atoms[currCandidate[0].first] = currCandidate[0].second;
			clauses = propagate(clauses, currCandidate[0]);
			if (clauses.empty())
				return atoms;

			for (vector<clause>::iterator currClause = clauses.begin(); currClause != clauses.end();  currClause++) {
				//if any clause is empty, we failed
				if (currClause->empty()) {
					return map<string, bool>();
				}
			}
		}


		//look for a pure literal
		set<literal> literals;
		set<literal> nope;
		for (vector<clause>::iterator currClause = clauses.begin(); currClause != clauses.end();  currClause++) {
			for (vector<literal>::iterator currLiteral = currClause->begin(); currLiteral != currClause->end(); currLiteral++) {
				set<literal>::iterator set_it = literals.find(make_pair(currLiteral->first, !currLiteral->second));
				if (set_it != literals.end()) {
					nope.insert(*currLiteral);
					nope.insert(make_pair(set_it->first, set_it->second));
					literals.erase(set_it);
				}
				else {
					set<literal>::iterator nope_it = nope.find(*currLiteral);
					if (nope_it == nope.end()) {
						literals.insert(*currLiteral);
					}
				}
			}
		}
		for (set<literal>::iterator currLiteral = literals.begin(); currLiteral != literals.end(); currLiteral++) {
			easyFound = true;
			if (atoms.find(currLiteral->first) != atoms.end()) {
				cout << "ERROR!" << endl;
				exit(EXIT_FAILURE);
			}

			atoms[currLiteral->first] = currLiteral->second;
			clauses = propagate(clauses, *currLiteral); 
			if (clauses.empty())
				return atoms;

			for (vector<clause>::iterator currClause = clauses.begin(); currClause != clauses.end();  currClause++) {
				//if any clause is empty, we failed
				if (currClause->empty()) {
					return map<string, bool>();
				}
			}
		}

	} while (easyFound);
	
	//now we try a guess
	literal guess;
	if (!clauses.empty() && !clauses[0].empty()) {
		guess = clauses[0][0];
		map<string, bool> guessAtoms = atoms;
		guessAtoms[guess.first] = guess.second;
		vector<clause> guessClauses = propagate(clauses, guess);
		map<string, bool> result = dp1(guessClauses, guessAtoms);
		if (result.empty()) {
			literal newGuess = make_pair(guess.first, !guess.second);
			map<string, bool> newGuessAtoms = atoms;
			newGuessAtoms[newGuess.first] = newGuess.second;
			vector<clause> newGuessClauses = propagate(clauses, newGuess);
			return dp1(newGuessClauses, newGuessAtoms);
		}
		else {
			return result;
		}
	}

	if (clauses.empty()) {
		return atoms;
	}

	return map<string, bool>();
}

map<string, bool>  dp(vector<clause> clauses) {
	return dp1(clauses, map<string, bool>());
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		cout << "usage: pa2DavisPutnam.exe inputfile.txt" << endl;
		exit(EXIT_FAILURE);
	}

	//read input 
	ifstream inputFile(argv[1]);
	vector<clause> clauses;
	vector<string> keys;
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
			else if (atKeys) {
				keys.push_back(line);
			}
			else {
				vector<string> parts = tokenize(line);
				clause newClause;
				for (vector<string>::iterator part = parts.begin(); part != parts.end(); part++) {
					string symbol;
					bool sign = true;
					if (part->at(0) == '-') {
						sign = false;
						symbol = part->substr(1);
					}
					else {
						symbol = *part;
					}
					literal newLit = make_pair(symbol, sign);
					newClause.push_back(newLit);
				}
				clauses.push_back(newClause);
			}
		}
	}
	else {
		cout << "ERROR: couldn't open input file." << endl;
		exit(EXIT_FAILURE);
	}

	ofstream output;
	output.open("dpoutput.txt");
	if (!output.is_open()) {
		cout << "Couldn't open output file" << endl;
		exit(EXIT_FAILURE);
	}

	map<string, bool> atoms = dp(clauses);
	if (!atoms.empty()) {
		for (map<string, bool>::iterator x = atoms.begin(); x != atoms.end(); x++) {
			output << x->first << " ";
			if (x->second)
				output << "T" << endl;
			else
				output << "F" << endl;
		}
	}
	else {
		output << "0" << endl;
	}
	output << "Keys:" << endl;
	for (vector<string>::iterator key = keys.begin(); key != keys.end(); key++) {
		output << *key << endl;
	}
	output.close();
}