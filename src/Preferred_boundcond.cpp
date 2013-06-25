/**
 * @file	Preferred_boundcond.cpp
 * @class	Preferred
 * @brief	Implementation of the function `boundcond`
 * @author	Maurizio Zucchelli
 * @version	1
 * @date	2013-06-25
 */

#include "Preferred.h"

#ifndef NDEBUG
#include <cassert>
#endif

/**
 * @brief Function boundcond.
 *
 * @param[in]	theAF	The Argument Framework to be considered
 * @param[in]	aSCC	The Strongly Connected Component to consider
 * @param[in]	e		The actual extension
 * @param[out]	O		First output set
 * @param[out]	I		Second output set
 */
void Preferred::boundcond( AF* theAF, SetArguments* aSCC, SetArguments* e,
						   SetArguments* O, SetArguments* I )
{
	if ( debug )
		cerr << "Entering boundcond\n";

#ifndef NDEBUG
	assert( O->empty() );
	assert( I->empty() );
#endif

	/* From the description:
	 * O è il sottoinsieme dei nodi di S[ i ] che sono attaccati da nodi in e
	 */
	
	// It's easier to determine the inverse of O, which is I before filtering
	aSCC->clone( I );
	for ( SetArgumentsIterator it = e->begin(); it != e->end(); ++it )
		I->setminus( (*it)->get_attacks(), I );

	// Determine S[ i ] \ I = O
	aSCC->setminus( I, O );

	if ( debug )
		cerr << "\tDetermined O: " << O << endl;

	/* From the description:
	 * I è il sottoinsieme dei nodi di S[ i ] \ O che
	 *  o non subiscono attacchi da nodi in G esterni a S[ i ]
	 *  o subiscono eventuali attacchi solo da parte di nodi che
	 *   (i)	sono contenuti in G ma non in S[ i ] U e
	 *   (ii)	sono attaccati da nodi in e
	 */
	// I is already S[ i ] \ O
	
	if ( debug )
		cerr << "\tDetermining I from: " << I << endl;
	
	// Nodes of G not in S[ i ]
	SetArguments external = SetArguments();
	theAF->get_arguments()->setminus( aSCC, &external );

	// Nodes of I attacked by externals
	// => they can be kept iff they satisfy the second condition
	SetArguments toBeRemoved = SetArguments();
	I->clone( &toBeRemoved );
	for ( SetArgumentsIterator it = external.begin(); it != external.end(); ++it )
		toBeRemoved.setminus( (*it)->get_attacks(), &toBeRemoved );

	if ( debug )
		cerr << "\t\tNodes not satisfying the first condition: " << toBeRemoved << endl;

	// Remove from the set the nodes satisfying both the conditions:
	// 	- node in G \ ( S[ i ] U e )
	// 	- node attacked by e
	external.setminus( e, &external );
	// The increment is done inside the for to avoid removal problems
	for ( SetArgumentsIterator it = toBeRemoved.begin(); it != toBeRemoved.end(); )
	{
		if ( external.exists( *it ) )
		{
			bool attacked = false;
			for ( SetArgumentsIterator jt = e->begin(); jt != e->end(); ++jt )
			{
				attacked = (*jt)->get_attacks()->exists( *it );
				if ( attacked )
					break;
			}
			
			if ( attacked )
			{
				Argument* previous = *it;
				++it;

				if ( debug )
					cerr << "\t\tRemoving " << previous << endl;

				toBeRemoved.remove( previous );
			}
			else
				++it;
		}
	}

	// Remove the nodes from I
	I->setminus( &toBeRemoved, I );

	if ( debug )
		cerr << "\tDetermined I: " << I << endl;
}

