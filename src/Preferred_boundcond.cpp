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
void Preferred::boundcond( SCC* aSCC, SymbolicArgumentsSet* e,
						   SymbolicArgumentsSet* O, SymbolicArgumentsSet* I )
{
	if ( debug )
		cerr << "Entering boundcond\n";

#ifndef NDEBUG
	assert( I->isEmpty() );
#endif

	// Assume that, if O is not empty, it has already been determined: skip it.
	if ( O->isEmpty() )
	{
		/* From the description:
		 * O è il sottoinsieme dei nodi di S[ i ] che sono attaccati da nodi in e
		 */

		// It's easier to determine the inverse of O, which is I before filtering
		SymbolicArgumentsSet attacks = e->getAttacks( this->af );
		*I = aSCC->argumentList->minus( &attacks );

		// Determine S[ i ] \ I = O
		*O = aSCC->argumentList->minus( I );
	}
	else
	{
		// Determine I as S[ i ] \ O
		*I = aSCC->argumentList->minus( O );
	}

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
	SymbolicArgumentsSet external = SymbolicArgumentsSet( *this->af->get_arguments() )
										.minus( aSCC->argumentList );

	// Keep the nodes of I not attacked by externals and filter the remaining
	SymbolicArgumentsSet toBeRemoved = *I;

	SymbolicArgumentsSet attacks = external.getAttacks( this->af );
	*I = I->minus( &attacks );
	
	// Nodes of I attacked by externals
	// => they can be kept iff they satisfy the second condition
	toBeRemoved = toBeRemoved.minus( I );

	SymbolicArgumentsSet toBeKept = SymbolicArgumentsSet();

	if ( debug )
		cerr << "\t\tNodes not satisfying the first condition: " << toBeRemoved << endl;

	// Remove from the set the nodes attacked by nodes satisfying both the conditions:
	// 	- node in G \ ( S[ i ] U e )
	// 	- node attacked by e
	//external.setminus( e, &external );
	for ( SymbolicArgumentsSet::iterator it = toBeRemoved.begin(); it != toBeRemoved.end(); ++it )
	{
		SymbolicArgumentsSet attackers = SymbolicArgumentsSet();
		// Find the external attackers (could be done the opposite way, maybe faster)
		for ( SymbolicArgumentsSet::iterator jt = external.begin(); jt != external.end(); ++jt )
			if ( SymbolicArgumentsSet( *this->af->getArgumentByName( *jt )->get_attacks() ).exists( *it ) )
				attackers.add( *jt );

		// Check that no attackers are in e
		size_t nAttackers = attackers.size();
		attackers = attackers.minus( e );

		if ( attackers.size() < nAttackers )
		{
			if ( debug )
				cerr << "\t\t" << *it << " attacked by e.\n";

			continue;
		}

		// Check that every attacker is itself attacked by e
		bool safe = true;
		for ( SymbolicArgumentsSet::iterator jt = attackers.begin(); jt != attackers.end(); ++jt )
		{
			bool attacked = false;
			for ( SymbolicArgumentsSet::iterator kt = e->begin(); kt != e->end(); ++kt )
			{
				attacked = SymbolicArgumentsSet( *this->af->getArgumentByName( *kt )->get_attacks() ).exists( *jt );
				if ( attacked )
					break;
			}

			if ( !attacked )
			{
				safe = false;
				break;
			}
		}

		if ( safe )
		{
			if ( debug )
				cerr << "\t\t" << *it << "'s attackers are attacked by e.\n";

			toBeKept.add( *it );
		}
		else if ( debug )
			cerr << "\t\tNot every attacker of " << *it << " is attacked by e.\n";
	}

	// Re-add the nodes to I
	*I = toBeKept.merge( I );

	if ( debug )
		cerr << "\tDetermined I: " << *I << endl;
}

