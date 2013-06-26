/*
 * AF.h
 *
 *  Created on: 13/mag/2013
 *      Author: valla
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <map>
#include <sstream>
#include <fstream>

#include "Argument.h"
class SetArguments;
class SetArgumentsIterator;

using namespace std;

class AF
{
	SetArguments *arguments;
	//vector<pair<int, int> > raw_attacks;
	//map<Argument *, SetArguments *> attacks;
	//map<Argument *, SetArguments *> attackers;
public:
	AF();
	virtual ~AF();
	bool readFile(string file);
	int numArgs();
	Argument *getArgumentByName(string);
	Argument *getArgumentByNumber(int position);
	SetArguments *get_arguments() const;

	// Added method
	// Copies the Framework, restricting it to a subset of nodes and their
	// relative non-suspended arcs
	void restrictTo( SetArguments*, AF* );
	// Returns a string containing a representation of the nodes and the attacks
	string toString();

	SetArgumentsIterator begin();
	SetArgumentsIterator end();
};

#include "SetArguments.h"

#endif /* GRAPH_H_ */
