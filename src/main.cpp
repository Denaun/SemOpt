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
bool stages = false;
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
	inputfile = string("test-input/scc-test.dl");

	// Arguments parsing
	int i = 0;
	while ( ++i < argc )
	{
		if ( !strcmp( argv[ i ], "-d" ) )
		{
			// More verbose
			debug = true;
			stages = true;
			
			cerr << "Debug on.\n";
		}
		else if( !strcmp( argv[ i ], "-d2" ) )
		{
			// Verbose but not that much
			stages = true;

			cerr << "Stages output on.\n";
		}
		else
		{
			// Custom input file
			inputfile = argv[ i ];

			if ( debug )
				cerr << "Input file: " << inputfile << endl;
		}
	}

	AF framework = AF();
	if (!framework.readFile(inputfile))
	{
		// No file to be read: exit.
		if ( debug )
			cerr << "File not found.\n";

		exit( EXIT_FAILURE );
	}


	// Use examples for scc-test.dl
	Preferred p = Preferred();

//	// First: a1, a6
//	SetArguments C_set1 = SetArguments();
//	C_set1.add_Argument(framework.getArgumentByName("a1"));
//	C_set1.add_Argument(framework.getArgumentByName("a6"));
//
//	p.prefSAT(&framework, &C_set1);
	p.pref( &framework, framework.get_arguments() );

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

//	// Second: a2, a3, a4
//	SetArguments C_set2 = SetArguments();
//	C_set2.add_Argument(framework.getArgumentByName("a2"));
//	C_set2.add_Argument(framework.getArgumentByName("a3"));
//	C_set2.add_Argument(framework.getArgumentByName("a4"));
//
//	p.pref( &framework, &C_set2);
//
//
//	for (Preferred::iterator it = p.begin(); it != p.end(); it++)
//	{
//		cout << "Second extension " << endl;
//		cout << *((*it).inargs()) << endl;
//	}

	return 0;
}
#endif
