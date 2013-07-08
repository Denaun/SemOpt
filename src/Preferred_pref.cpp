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
	bool AFequalsC = theAF->get_arguments() == theC;

	SetArguments e = SetArguments(),
				 I = SetArguments();

	// Avoid calling Grounded if useless
	if ( theC->cardinality() <= 1 && *theC == *(theAF->get_arguments()) )
	{
		if ( debug )
			cerr << "\tNo need to call Grounded.\n";

		// Empty: no extensions => null iteration
		// Singlet: still have to iterate over I
		for ( SetArgumentsIterator it = theC->begin(); it != theC->end(); ++it )
			e.add_Argument( *it );
	}
	else
		Grounded( &e, &I );

	if ( stages )
		cerr << "\te: " << e << endl << "\tI: " << I << endl; 

	// Convert Grounded's output into a Labelling
	Labelling first = Labelling();
	for ( SetArgumentsIterator it = e.begin(); it != e.end(); ++it )
		first.add_label( *it, Labelling::lab_in );
	// TODO: Label I as undec and everything else as out?

	if ( stages )
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
		SetArguments O = SetArguments();
		// ( I already exists... )
		I = SetArguments();

		for ( vector<Labelling>::iterator aLabelling = this->labellings.begin();
				aLabelling != this->labellings.end(); ++aLabelling )
		{
			Preferred p = Preferred();

			// Determine if the call of boundcond can be avoided
			// If the current SCC has no fathers, O = empty and I = SCC
			if( preserveO && !O.empty() )
			{
				if ( debug )
					cerr << "\tO and I preserved.\n";

				// TODO? Readapt I
				// Shouldn't be useful,
				// 	because I is not needed until it is adapted before pref/prefSAT
				I.adaptTo( this->af );
			}
			else
			{
				// Reset the sets
				O = SetArguments();
				I = SetArguments();

				if( AFequalsC && ( *aSCC ) -> fathers.size() == 0 )
				{
					if ( debug )
						cerr << "\tO emptied and I filled.\n";

					( *aSCC ) -> argumentList -> clone( &I );
				}
				else
				{
					//if ( debug )
					//	cerr << "\tCalling boundcond.\n";

					boundcond( *aSCC, (*aLabelling).inargs(), &O, &I );
				}
			}

			if ( stages )
				cerr << "\tO: " << O << endl << "\tI: " << I << endl;

			if ( O.empty() )
			{
				if ( debug )
					cerr << "\tGoing to call prefSAT.\n";

				// Avoid calling prefSAT if the two parameters are the same
				// and the size is 2 or less:
				// 	0:	out = { {} }
				//	1:	out = { {singlet} }
				//	2:	out = { {singlet}, {singlet} }
				if ( ( *aSCC ) -> argumentList -> cardinality() <= 2 && *( *aSCC ) -> argumentList == I )
				{
					if ( debug )
						cerr << "\t\tNo need to call prefSAT.\n";

					// Empty: no extensions => null iteration
					// Singlet: still have to iterate over I
					// Two element: Mutually exclusive arguments
					for ( SetArgumentsIterator it = I.begin();
							it != I.end(); ++it )
					{
						Labelling* temp = new Labelling();
						temp->add_label( *it, Labelling::lab_in );
						p.labellings.push_back( *temp );
					}
				}
				else
				{
					AF restricted = AF();
					this->af->restrictTo( ( *aSCC ) -> argumentList, &restricted );

					// prefSAT problem: the nodes in I are different from
					// the nodes in the restricted AF.
					// Temporary solution: rebuild I.
					I.adaptTo( &restricted );

					if( debug )
							cerr << "\t\tCalling prefSAT.\n";

					p.prefSAT( &restricted, &I );
				}
			}
			else
			{
				if ( debug )
					cerr << "\tGoing to call pref.\n";

				SetArguments restriction = SetArguments();
				( *aSCC ) -> argumentList -> setminus( &O, &restriction );

				if ( restriction.cardinality() <= 1 && restriction == I )
				{
					if ( debug )
						cerr << "\t\tNo need to call pref.\n";

					// Empty: no extensions => null iteration
					// Singlet: still have to iterate over I
					for ( SetArgumentsIterator it = I.begin();
							it != I.end(); ++it )
					{
						Labelling* temp = new Labelling();
						temp->add_label( *it, Labelling::lab_in );
						p.labellings.push_back( *temp );
					}
				}
				else
				{
					if ( debug )
						cerr << "\t\tCalling pref.\n";

					AF restricted = AF();
					this->af->restrictTo( &restriction, &restricted );

					// The same as above for prefSAT.
					I.adaptTo( &restricted );

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

