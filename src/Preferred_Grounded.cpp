/**
 * @file	Preferred_grounded.cpp
 * @class	Preferrred
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
 * @brief	Grounded Function
 * @details	Calculates the set containing every argument which, if used, ensures to win
 * 			the argument and the set of nodes which can be used on both sides
 *
 * @param[in]	theAF	The Argumentation Framework to be considered
 * @param[in]	theC	The set of arguments to consider
 * @param[out]	e		The extension
 * @param[out]	I		The set of the indolent arguments
 */
void Preferred::Grounded( AF* theAF, SetArguments* theC, SetArguments* e, SetArguments* I )
{
#ifndef NDEBUG
	assert( e->empty() );
	assert( I->empty() );
#endif

	// I <- A
	theAF->get_arguments()->clone( I );

	// Copy of C to work on
	SetArguments C = SetArguments();
	theC->clone( &C );

	while ( true )
	{
		SetArguments N = SetArguments();

		// Search for a set of all non-attacked arguments
		for ( SetArgumentsIterator it = C.begin(); it != C.end(); ++it )
		{
			// For every element of C, check if it is attacked
			// if not, add it to N and, later, remove it from C and I
			bool attacked;
			SetArguments* attackers = (*it)->get_attackers();
			for ( SetArgumentsIterator jt = attackers->begin(); jt != attackers->end(); ++jt )
			{
				attacked = I->exists( (*jt) );
				if ( attacked )
					break;
			}

			if ( !attacked )
				N.add_Argument( (*it) );
		}

		// If no nodes have been found, we have finished.
		if ( N.empty() )
			return;

		// Extend the extension and filter the indolents and the considered set
		N.clone( e );
		C.setminus( &N, &C );
		I->setminus( &N, I );
		for ( SetArgumentsIterator it = N.begin(); it != N.end(); ++it )
		{
			C.setminus( (*it)->get_attackers(), &C );
			I->setminus( (*it)->get_attackers(), I );
		}
	}

}