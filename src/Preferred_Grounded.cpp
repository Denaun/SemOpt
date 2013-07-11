/**
 * @file	Preferred_grounded.cpp
 * @brief	Implementation of the function `grounded`
 * @author	Maurizio Zucchelli
 * @version	1
 * @date	2013-06-25
 */

#include "Preferred.h"

#ifndef NDEBUG
#include <cassert>
#endif

/**
 * @brief	Function to calculate the grounded extensions.
 * @details	Calculates the set containing every argument which, if used, ensures to win
 * 			the argument and the set of nodes which can be used on both sides
 *
 * @param[in]	theC	The subset of arguments to consider.
 * @param[out]	e		The extension
 * @param[out]	I		The set of the indolent arguments
 */
void Preferred::Grounded( const SymbolicArgumentsSet* theC, SymbolicArgumentsSet* e, SymbolicArgumentsSet* I )
{
	if ( debug )
		cerr << "Entering Grounded\n";

#ifndef NDEBUG
	assert( e->isEmpty() );
	assert( I->isEmpty() );
#endif

	// I <- A
	*I = SymbolicArgumentsSet( *this -> af -> get_arguments() );

	// Copy of C to work on
	SymbolicArgumentsSet C = *theC;

	while ( true )
	{
		if ( debug )
			cerr << "\tNew cycle\n";

		SymbolicArgumentsSet N = SymbolicArgumentsSet();

		// Search for a set of all non-attacked arguments
		for ( SymbolicArgumentsSet::iterator it = C.begin(); it != C.end(); ++it )
		{
			// For every element of C, check if it is attacked
			// if not, add it to N and, later, remove it from C and I
			bool attacked = false;
			SymbolicArgumentsSet attackers = SymbolicArgumentsSet( *af->getArgumentByName( *it )->get_attackers() );

			for ( SymbolicArgumentsSet::iterator jt = attackers.begin(); jt != attackers.end(); ++jt )
			{
				attacked = I->exists( *jt );
				if ( attacked )
					break;
			}

			if ( !attacked )
			{
				N.add( *it );

				if ( debug )
					cerr << "\t\tAdded " << *it << endl;
			}
		}

		// If no nodes have been found, we have finished.
		if ( N.isEmpty() )
		{
			if ( debug )
			{
				cerr << "\t\tNo grounded extension found.\n";
				cerr << "\t\tFinal I: " << *I << endl;
			}

			return;
		}

		// Extend the extension and filter the indolents and the considered set
		*e = N.merge( e );
		C = C.minus( &N );
		*I = I->minus( &N );

		for ( SymbolicArgumentsSet::iterator it = N.begin(); it != N.end(); ++it )
		{
			SymbolicArgumentsSet attacks = SymbolicArgumentsSet( *af->getArgumentByName( *it )->get_attacks() ); 
			C = C.minus( &attacks );
			*I = I->minus( &attacks );
		}
	}

}
