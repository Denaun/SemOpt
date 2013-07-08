/**
 * @file	Preferred_pref.cpp
 * @class	Preferred
 * @brief	Implementation of the recursive function `pref`
 * @author	Maurizio Zucchelli
 * @author	Mattia Rizzini
 * @version	1.1
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
void Preferred::pref( AF* theAF, SymbolicArgumentsSet* theC )
{
	if ( debug )
		cerr << "Entering pref\n";

	this->cleanlabs();
	this->af = theAF;
	this->C = theC;

	if ( stages )
	{
		cerr << "AF initialized. Showing structure" << endl
			 << this->af->toString() << endl;
	}

	// Flag indicating whether theAF == theC,
	// 	useful for an optimization avoiding the call of boundcond.
	bool AFequalsC = *( theAF->get_arguments() ) == theC -> toSetArguments();

	SymbolicArgumentsSet e = SymbolicArgumentsSet(),
				 I = SymbolicArgumentsSet();

	// Avoid calling Grounded if useless
	if ( theC -> size() <= 1 && AFequalsC )
	{
		if ( debug )
			cerr << "\tNo need to call Grounded.\n";

		// Empty: no extensions => null iteration
		// Singlet: still have to iterate over I
		for ( SymbolicArgumentsSet::iterator it = theC -> begin(); it != theC -> end(); ++it )
			e.add( *it );
	}
	else
		Grounded( &e, &I );

	if ( stages )
		cerr << "\te: " << e << endl << "\tI: " << I << endl; 

	// Convert Grounded's output into a Labelling
	Labelling first = e.toLabelling();
	// TODO: Label I as undec and everything else as out?

	if ( stages )
		cerr << "\tFirst Labelling: " << *(first.inargs()) << endl;

	// Add the Labelling to the solutions
	this->labellings.push_back( first );

	if ( I.isEmpty() )
	{
		if ( debug )
			cerr << "\tNo unlabelled arguments left. Returning.\n";

		return;
	}

	// Generate G restricted to I
	AF* restricted = new AF();
	SetArguments ISet = I.toSetArguments();
	this->af->restrictTo( &ISet, restricted );
	this->af = restricted;

	if ( stages )
		cerr << "\tNew AF ( restricted ):\n" << this->af->toString() << endl;

	// Structure used in SCCSSEQ
	this->initDFSAF();

	if ( debug )
		cerr << "\tDFSAF structure initialized\n";

	// Calculate the Strongly Connected Components
	list< SCC* > S = SCCSSEQ();

	// Print SCC and parenthood
	if ( stages )
		for ( list< SCC* >::iterator it = S.begin(); it != S.end(); ++it )
		{
			cerr << "\tSCC: " << *( *it ) -> argumentList << endl;

			for( list< SCC* >::iterator father = ( *it ) -> fathers.begin();
					father != ( *it ) -> fathers.end(); father++ )
				cerr << "\t\tFather: " << *( *father ) -> argumentList << endl;
		}

	// Indicates to preserve O or not inside the inner cycle
	// (no boundcond recalculation after the first time, conditions described below)
	bool preserveO = true;

	for ( list< SCC* >::iterator aSCC = S.begin(); aSCC != S.end(); ++aSCC )
	{
		vector<Labelling> newLabellings = vector<Labelling>();

		// O is preserved throught the iterations because,
		// 	if the actual SCC has not a father in the previous one,
		// 	then O remains always the same
		// This is checked right before the reiteration of the previous cycle
		// (for the first SCC the condition is always true)
		SymbolicArgumentsSet O = SymbolicArgumentsSet();
		// ( I already exists... )
		I = SymbolicArgumentsSet();

		for ( vector<Labelling>::iterator aLabelling = this->labellings.begin();
				aLabelling != this->labellings.end(); ++aLabelling )
		{
			Preferred p = Preferred();

			// Determine if the call of boundcond can be avoided
			// If the current SCC has no fathers, O = empty and I = SCC
			if( preserveO && !O.isEmpty() )
			{
				if ( debug )
					cerr << "\tO and I preserved.\n";
			}
			else
			{
				// Reset the sets
				O = SymbolicArgumentsSet();
				I = SymbolicArgumentsSet();

				if( AFequalsC && ( *aSCC ) -> fathers.size() == 0 )
				{
					if ( debug )
						cerr << "\tO emptied and I filled.\n";

					I = ( *aSCC ) -> argumentList;
				}
				else
				{
					//if ( debug )
					//	cerr << "\tCalling boundcond.\n";

					SymbolicArgumentsSet thisLabelling = (*aLabelling).inargs();
					boundcond( *aSCC, &thisLabelling, &O, &I );
				}
			}

			if ( stages )
				cerr << "\tO: " << O << endl << "\tI: " << I << endl;

			if ( O.isEmpty() )
			{
				if ( debug )
					cerr << "\tGoing to call prefSAT.\n";

				// Avoid calling prefSAT if the two parameters are the same
				// and the size is 2 or less:
				// 	0:	out = { {} }
				//	1:	out = { {singlet} }
				//	2:	out = { {singlet}, {singlet} }
				if ( ( *aSCC ) -> argumentList -> size() <= 2 && *( *aSCC ) -> argumentList == I )
				{
					if ( debug )
						cerr << "\t\tNo need to call prefSAT.\n";

					// Empty: no extensions => null iteration
					// Singlet: still have to iterate over I
					// Two element: Mutually exclusive arguments
					p.labellings.push_back( I.toLabelling() );
				}
				else
				{
					AF restricted = AF();
					SetArguments setVersion = ( *aSCC ) -> argumentList -> toSetArguments();
					this->af->restrictTo( &setVersion, &restricted );

					// Nobody needs to adapt in the symbolic world 'coz everyone is perfect as it is :D

					if( debug )
							cerr << "\t\tCalling prefSAT.\n";

					SetArguments ISet = I.toSetArguments();
					p.prefSAT( &restricted, &ISet );
				}
			}
			else
			{
				if ( debug )
					cerr << "\tGoing to call pref.\n";

				SymbolicArgumentsSet restriction = SymbolicArgumentsSet();
				restriction = ( *aSCC ) -> argumentList -> minus( &O );

				if ( restriction.size() <= 1 && restriction == I )
				{
					if ( debug )
						cerr << "\t\tNo need to call pref.\n";

					// Empty: no extensions => null iteration
					// Singlet: still have to iterate over I
					p.labellings.push_back( I.toLabelling() );
				}
				else
				{
					if ( debug )
						cerr << "\t\tCalling pref.\n";

					AF restricted = AF();
					SetArguments restrictionSet = restriction.toSetArguments();
					this->af->restrictTo( &restrictionSet, &restricted );

					p.pref( &restricted, &I );
				}
			}

			// prefSAT doesn't put newline at the end of its output...
			if ( debug )
				cerr << endl;

			// Create the new Labellings
			// by merging the current Labelling with every Labelling found
			for ( Preferred::iterator EStar = p.begin(); EStar != p.end(); ++EStar )
			{
				if ( debug )
					cerr << "\t\tFound " << *((*EStar).inargs()) << endl;

				// Rebuild the Labelling using the original Arguments
				// Not doing so could cause problems to next boundconds
				(*EStar).adaptTo( theAF );

				(*aLabelling).clone( &(*EStar) );

				if ( debug )
					cerr << "\t\t\tCreated: " << *((*EStar).inargs()) << endl;

				// Add it to the labellings found
				newLabellings.push_back( *EStar );
			}
		}
		
		// The generated Labellings are the new Labellings
		this->labellings.assign( newLabellings.begin(), newLabellings.end() );

		// Check if the actual SCC is a father of the next one
		preserveO = false;

		list< SCC* >::iterator next = aSCC;
		if( ++next == S.end() )
			break;

		for( list< SCC* >::iterator fathers = ( *( next ) ) -> fathers.begin();
				fathers != ( *( next ) ) -> fathers.end(); fathers++ )
			if( *fathers == *aSCC )
			{
				preserveO = true;
				break;
			}
	}
}

