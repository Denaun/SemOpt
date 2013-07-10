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
void Preferred::pref( AF* theAF, SetArguments* theC )
{
	SymbolicArgumentsSet symC = SymbolicArgumentsSet( *theC );
	pref( theAF, &symC );
}

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

	vector<SymbolicArgumentsSet> tempLabellings = vector<SymbolicArgumentsSet>();

	if ( stages )
	{
		cerr << "AF initialized. Showing structure" << endl
			 << this->af->toString() << endl;
	}

	// Flag indicating whether theAF == theC,
	// 	useful for an optimization avoiding the call of boundcond.
	bool AFequalsC = SymbolicArgumentsSet( *theAF->get_arguments() ) == *theC;

	SymbolicArgumentsSet e = SymbolicArgumentsSet(),
						 I = SymbolicArgumentsSet();

	// Avoid calling Grounded if useless
	if ( theC -> size() <= 1 && AFequalsC )
	{
		if ( debug )
			cerr << "\tNo need to call Grounded.\n";

		// Empty: no extensions => empty Labelling
		// Singlet: single-element Labelling
		e = *theC;
	}
	else
		Grounded( theC, &e, &I );

	if ( stages )
		cerr << "\te: " << e << endl << "\tI: " << I << endl; 

	if ( I.isEmpty() )
	{
		if ( debug )
			cerr << "\tNo unlabelled arguments left. Returning.\n";

		// Assign the temporary Labelling
		this->labellings.push_back( e.toLabelling( this->af ) );

		return;
	}
	else
	{
		// Add the Labelling to the solutions
		tempLabellings.push_back( e );
	}

	// Generate G restricted to I
	AF* restricted = new AF();
	this->af->restrictTo( &I, restricted );
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
		if ( debug )
			cerr << "\tWorking on " << *(*aSCC)->argumentList << endl;

		vector<SymbolicArgumentsSet> newLabellings = vector<SymbolicArgumentsSet>();

		// O is preserved throught the iterations because,
		// 	if the actual SCC has not a father in the previous one,
		// 	then O remains always the same
		// This is checked right before the reiteration of the previous cycle
		// (for the first SCC the condition is always true)
		SymbolicArgumentsSet O = SymbolicArgumentsSet();
		// ( I already exists... )
		I.clear();

		for ( vector<SymbolicArgumentsSet>::iterator aLabelling = tempLabellings.begin();
				aLabelling != tempLabellings.end(); ++aLabelling )
		{
			Preferred p = Preferred();

			if ( debug )
				cerr << "\t\tGoing to call boundcond.\n";

			// Determine if the call of boundcond can be avoided
			// If the current SCC has no fathers, O = empty and I = SCC
			
			if( AFequalsC && ( *aSCC ) -> fathers.size() == 0 )
			{
				if ( debug )
					cerr << "\t\t\tO emptied and I filled.\n";

				O.clear();
				I = *(*aSCC)->argumentList;
			}
			else
			
			{
				// Reset I
				I.clear();

				
				if( preserveO && !O.isEmpty() )
				{
					if ( debug )
						cerr << "\t\t\tO (should be) preserved.\n";

					// Lo stesso insieme O per ogni e..?
					// evito.
					O.clear();
				}
				else
				
				{
					if ( debug )
						cerr << "\t\t\tO emptied.\n";

					O.clear();
				}

				boundcond( *aSCC, &(*aLabelling), &O, &I );
			}

			if ( stages )
				cerr << "\t\tO: " << O << endl << "\t\tI: " << I << endl;

			if ( O.isEmpty() )
			{
				if ( debug )
					cerr << "\t\tGoing to call prefSAT.\n";

				// Avoid calling prefSAT if the two parameters are the same
				// and the size is 2 or less:
				// 	0:	out = { {} }
				//	1:	out = { {singlet} }
				//	2:	out = { {singlet}, {singlet} }
				
				if ( ( *aSCC ) -> argumentList -> size() <= 2 && *( *aSCC ) -> argumentList == I )
				{
					if ( debug )
						cerr << "\t\t\tNo need to call prefSAT.\n";

					if ( I.isEmpty() )
					{
						// Empty: no extensions => empty Labelling
						p.labellings.push_back( Labelling() );
					}
					else
					{
						// Singlet: still have to iterate over I
						// Two element: Mutually exclusive arguments
						SetArguments ISet = I.toSetArguments( this->af );
						for ( SetArgumentsIterator it = ISet.begin();
								it != ISet.end(); ++it )
						{
							Labelling* temp = new Labelling();
							temp->add_label( *it, Labelling::lab_in );

							p.labellings.push_back( *temp );
						}
					}
				}
				else
				
				{
					AF restricted = AF();
					this->af->restrictTo( ( *aSCC ) -> argumentList, &restricted );

					// Nobody needs to adapt in the symbolic world 'coz everyone is perfect as it is :D

					if( debug )
							cerr << "\t\t\tCalling prefSAT.\n";

					SetArguments ISet = I.intersect( theC ).toSetArguments( &restricted );
					p.prefSAT( &restricted, &ISet );
				}
			}
			else
			{
				if ( debug )
					cerr << "\t\tGoing to call pref.\n";

				SymbolicArgumentsSet restriction = SymbolicArgumentsSet();
				restriction = ( *aSCC ) -> argumentList -> minus( &O );

				
				if ( restriction.size() <= 1 && restriction == I )
				{
					if ( debug )
						cerr << "\t\t\tNo need to call pref.\n";

					if ( I.isEmpty() )
					{
						// Empty: no extensions => empty Labelling
						p.labellings.push_back( Labelling() );
					}
					else
					{
						// Singlet: still have to iterate over I
						SetArguments ISet = I.toSetArguments( this->af );

						Labelling* temp = new Labelling();
						temp->add_label(
								this->af->getArgumentByName( *I.begin() ),
								Labelling::lab_in );

						p.labellings.push_back( *temp );
					}
				}
				else
				
				{
					if ( debug )
						cerr << "\t\t\tCalling pref.\n";

					AF restricted = AF();
					this->af->restrictTo( &restriction, &restricted );

					SymbolicArgumentsSet ISet = I.intersect( theC );
					p.pref( &restricted, &ISet );
				}
			}

			// prefSAT doesn't put newline at the end of its output...
			if ( debug )
				cerr << endl;

			// Create the new Labellings
			// by merging the current Labelling with every Labelling found
			for ( Preferred::iterator EStar = p.begin(); EStar != p.end(); ++EStar )
			{
				SymbolicArgumentsSet found = SymbolicArgumentsSet( *EStar );

				if ( debug )
					cerr << "\t\t\tFound " << found << endl;

				SymbolicArgumentsSet created = (*aLabelling).merge( &found );

				if ( debug )
					cerr << "\t\t\t\tCreated: " << created << endl;

				// Add it to the labellings found
				newLabellings.push_back( created );
			}
		}
		
		// The generated Labellings are the new Labellings
		tempLabellings.assign( newLabellings.begin(), newLabellings.end() );

		
		// Check if the actual SCC is a father of the next one
		preserveO = false;

		list< SCC* >::iterator next = aSCC;
		if( ++next == S.end() )
			break;

		for( list< SCC* >::iterator fathers = ( *( next ) ) -> fathers.begin();
				fathers != ( *next ) -> fathers.end(); ++fathers )
			if( *fathers == *aSCC )
			{
				preserveO = true;
				break;
			}
		
	}

	if ( stages )
	{
		stringstream s;
		copy( tempLabellings.begin(), tempLabellings.end(), ostream_iterator<SymbolicArgumentsSet>( s, "\n\t" ) );

		cerr << "Actual labellings:\n\t" << s.str() << endl;
	}

	// Finally, assign the temporary labellings using the original AF
	for ( vector<SymbolicArgumentsSet>::iterator it = tempLabellings.begin();
			it != tempLabellings.end(); ++it )
		this->labellings.push_back( (*it).toLabelling( theAF ) );
}

