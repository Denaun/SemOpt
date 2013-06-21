/**
 * @file 		SemOpt.cpp
 * @brief 		Main file
 * @author 		Federico Cerutti <f.cerutti@abdn.ac.uk> and
 * 				Mauro Vallati <m.vallati@hud.ac.uk>
 * @copyright	GNU Public License v2
 */

#include "semopt.h"

/**
 * @brief Configuration variables
 */
bool debug = false;
bool externalsat = true;
string satsolver;
int labellingconditions = 0;
bool manualopt = false;
string inputfile;
string semantics;

time_t start;

#ifndef UNIT_TEST

int main(int argc, char *argv[])
{
	//debug = true; // more verbose
	inputfile = string("test-input/scc-test.dl");
	AF framework = AF();
	if (!framework.readFile(inputfile))
	{
		return -1;
	}

	Preferred p = Preferred();

	SetArguments C_set1 = SetArguments();
	C_set1.add_Argument(framework.getArgumentByName("a1"));
	C_set1.add_Argument(framework.getArgumentByName("a6"));

	p.prefSAT(&framework, &C_set1);

	for (Preferred::iterator it = p.begin(); it != p.end(); it++)
	{
		cout << "First extension " << endl;
		for (SetArgumentsIterator itarg = (*it).inargs()->begin();
				itarg != (*it).inargs()->end(); itarg++)
		{
			cout << (*itarg)->getName() << endl;
		}
		cout << "Or even " << endl;
		cout << *((*it).inargs()) << endl;
	}

	SetArguments C_set2 = SetArguments();
	C_set2.add_Argument(framework.getArgumentByName("a2"));
	C_set2.add_Argument(framework.getArgumentByName("a3"));
	C_set2.add_Argument(framework.getArgumentByName("a4"));

	p.prefSAT(&framework, &C_set2);

	for (Preferred::iterator it = p.begin(); it != p.end(); it++)
	{
		cout << "Second extension " << endl;
		cout << *((*it).inargs()) << endl;
	}

	return 0;
}
#endif
