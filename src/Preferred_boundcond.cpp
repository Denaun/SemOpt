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
 * @param[in]	aSCC	The Strongly Connected Component to consider
 * @param[in]	e		The actual extension
 * @param[out]	O		First output set
 * @param[out]	I		Second output set
 */
void Preferred::boundcond( SetArguments* aSCC, SetArguments* e,
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
		cerr << "\tDetermined O: " << *O << endl;

	/* From the description:
	 * I è il sottoinsieme dei nodi di S[ i ] \ O che
	 *  o non subiscono attacchi da nodi in G esterni a S[ i ]
	 *  o subiscono eventuali attacchi solo da parte di nodi che
	 *   (i)	sono contenuti in G ma non in S[ i ] U e
	 *   (ii)	sono attaccati da nodi in e
	 */
	// I is already S[ i ] \ O
	
	if ( debug )
		cerr << "\tDetermining I from: " << *I << endl;
	
	// Nodes of G not in S[ i ]
	SetArguments external = SetArguments();
	this->af->get_arguments()->setminus( aSCC, &external );

	// Keep the nodes of I not attacked by externals and filter the remaining
	SetArguments toBeRemoved = SetArguments();
	I->clone( &toBeRemoved );
	for ( SetArgumentsIterator it = external.begin(); it != external.end(); ++it )
		I->setminus( (*it)->get_attacks(), I );
	
	// Nodes of I attacked by externals
	// => they can be kept iff they satisfy the second condition
	toBeRemoved.setminus( I, &toBeRemoved );
	SetArguments toBeKept = SetArguments();

	if ( debug )
		cerr << "\t\tNodes not satisfying the first condition: " << toBeRemoved << endl;

	// Remove from the set the nodes attacked by nodes satisfying both the conditions:
	// 	- node in G \ ( S[ i ] U e )
	// 	- node attacked by e
	//external.setminus( e, &external );
	for ( SetArgumentsIterator it = toBeRemoved.begin(); it != toBeRemoved.end(); ++it )
	{
		SetArguments attackers = SetArguments();
		// Find the external attackers (could be done the opposite way, maybe faster)
		for ( SetArgumentsIterator jt = external.begin(); jt != external.end(); ++jt )
			if ( (*jt)->get_attacks()->exists( *it ) )
				attackers.add_Argument( *jt );

		// Check that no attackers are in e
		int nAttackers = attackers.cardinality();
		attackers.setminus( e, &attackers );
		if ( attackers.cardinality() < nAttackers )
		{
			if ( debug )
				cerr << "\t\t" << (*it)->getName() << " attacked by e.\n";

			continue;
		}

		// Check that every attacker is itself attacked by e
		bool safe = true;
		for ( SetArgumentsIterator jt = attackers.begin(); jt != attackers.end(); ++jt )
		{
			bool attacked = false;
			for ( SetArgumentsIterator kt = e->begin(); kt != e->end(); ++kt )
			{
				attacked = (*kt)->get_attacks()->exists( *jt );
				if ( attacked )
					break;
			}

//			safe &= attacked;
//			if ( !safe )
//				break;
			if ( !attacked )
			{
				safe = false;
				break;
			}
		}

		if ( safe )
		{
			if ( debug )
				cerr << "\t\t" << (*it)->getName() << "'s attackers are attacked by e.\n";

			toBeKept.add_Argument( *it );
		}
		else if ( debug )
			cerr << "\t\tNot every attacker of " << (*it)->getName() << " is attacked by e.\n";

		/*
		if ( !attackers.empty() )
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
				if ( debug )
					cerr << "\t\tKeeping " << (*it)->getName() << endl;

				// Put the node in the set containing the nodes to be kept
				toBeKept.add_Argument( *it );
			}
			else if ( debug )
				cerr << "\t\t" << (*it)->getName() << " is not attacked.\n";

		}
		else if ( debug )
			cerr << "\t\t" << (*it)->getName() << " is not external.\n";
		*/
	}

	// Readd the nodes to I
	toBeKept.clone( I );

	if ( debug )
		cerr << "\tDetermined I: " << *I << endl;
}

