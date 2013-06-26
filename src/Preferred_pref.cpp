/**
 * @file	Preferred_pref.cpp
 * @class	Preferred
 * @brief	Implementation of the recursive function `pref`
 * @author	Maurizio Zucchelli
 * @version	1
 * @date	2013-06-25
 */

#include "Preferred.h"

/**
 * @brief	Main resolutive function.
 * @details	Fills the labels of the class with the preferred extensions found.
 * @todo	Everything
 *
 * @param[in]	theAF	The Argument Framework to be considered
 * @param[in]	theC	The subset of arguments to consider
 */
void Preferred::pref( AF* theAF, SetArguments* theC )
{
	if ( debug )
		cerr << "Entering pref\n";

	this->cleanlabs();
	this->af = theAF;
	this->C = theC;

	SetArguments e = SetArguments(),
				 I = SetArguments();
	Grounded( &e, &I );

	// Convert Grounded's output into a Labelling
	Labelling first = Labelling();
	for ( SetArgumentsIterator it = e.begin(); it != e.end(); ++it )
		first.add_label( *it, Labelling::lab_in );
	// TODO: Label I as undec and everything else as out?

	if ( debug )
		cerr << "\tFirst Labelling: " << *(first.inargs()) << endl;

	// Add the Labelling to the solutions
	this->labellings.push_back( first );

	if ( I.empty() )
	{
		if ( debug )
			cerr << "\tNo unlabelled arguments left. Returning.\n";

		return;
	}

	// Generate G restricted to I
	AF* restricted = new AF();
	this->af->restrictTo( &I, restricted );
	this->af = restricted;
	/*
	SetArguments* A = this->af->get_arguments();
	//delete A;
	*A = I;

	// Erase suspended arcs
	SetArguments* temp;
	for ( SetArgumentsIterator it = A->begin(); it != A->end(); ++it )
	{
		temp = new SetArguments();
		SetArguments* attacks = (*it)->get_attacks();
		attacks->intersect( &I, temp );
		//delete attacks;
		*attacks = *temp;

		temp = new SetArguments();
		SetArguments* attackers = (*it)->get_attackers();
		attackers->intersect( &I, temp );
		//delete attackers;
		*attackers = *temp;
	}
	*/

	if ( debug )
		cerr << "\tNew A: " << *(this->af->get_arguments()) << endl;

	// Calculate the Strongly Connected Components
	list<SetArguments*> S = SCCSSEQ();

	if ( debug )
		for ( list<SetArguments*>::iterator it = S.begin(); it != S.end(); ++it )
			cerr << "\tSCC: " << **it << endl;

	for ( list<SetArguments*>::iterator aSCC = S.begin(); aSCC != S.end(); ++aSCC )
	{
		Preferred p = Preferred();

		for ( vector<Labelling>::iterator aLabelling = this->labellings.begin();
				aLabelling != this->labellings.end(); ++aLabelling )
		{
			SetArguments O = SetArguments();
			I = SetArguments();	// I already exists..

			boundcond( *aSCC, (*aLabelling).inargs(), &O, &I );

			if ( O.empty() )
			{
				if ( debug )
					cerr << "\tCalling prefSAT.\n";

				AF restricted = AF();
				this->af->restrictTo( *aSCC, &restricted );
				// Should be G restricted to S[ i ] and I intersect C
				// And done iff I != Ø (doesn't prefSAT return Ø otherwise..?)
				p.prefSAT( &restricted, &I );
			}
			else
			{
				if ( debug )
					cerr << "\tCalling pref.\n";

				SetArguments restriction = SetArguments();
				(*aSCC)->setminus( &O, &restriction );
				AF restricted = AF();
				this->af->restrictTo( &restriction, &restricted );
				// Should be G restricted to S[ i ] \ O and I intersect C
				p.pref( &restricted, &I );
			}

			// Create the new Labellings
			for ( vector<Labelling>::iterator EStar = p.begin();
					EStar != p.end(); ++EStar )
				(*aLabelling)->clone( *Estar );
		}
	}
}

