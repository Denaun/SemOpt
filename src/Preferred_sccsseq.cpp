/**
 * @file	Preferred_sccsseq.cpp
 * @class	Preferred
 * @brief	Implementation of SCCSSEQ
 * @author	Mattia Rizzini
 * @date	2013-06-25
 */

#include "Preferred.h"

/**
 * @brief SCC sequence finder
 *
 * @return A list containing the sets of Strongly
 *		Connected Components found by the algorithm
 */
list< SCC* > Preferred::SCCSSEQ()
{
	if ( debug )
		cerr << "Entering SCCSSEQ.\n";

	// La struttura dati per la DFS è inizializzata nel costruttore di Preferred 

	// SCC sarà la lista dei set di componenti fortemente connesse, lo stack è ausiliario per Tarjan (contiene lo stack dei nodi visitati in una particolare invocazione)
	list< SCC* > SCCList = list< SCC* >();
	stack< DFSNode* > s = stack< DFSNode* >();

	int index = 0;

	// Si scorrono tutti i nodi assicurandosi che eventuali nodi non raggiungibili dal primo siano comunque visitati. Se il grafo è connesso il metodo TarjanAlg viene eseguito una sola volta (una sola ricerca in profondità)
	for( list< DFSNode* >::iterator i = DFSAF.begin(); i != DFSAF.end(); i++ )
		if( (*i) -> index == -1 )
		{
			if ( debug )
				cerr << "\tCalling TarjanAlg for " << (*i)->argument->getName() << endl;

			index = TarjanAlg( *i, &SCCList, &s, index );
		}

	
	// Determino i rapporti di parentela tra le SCC
	SCCParenthood( &SCCList );
	
	

	return SCCList;
}

/**
  * @brief	Function to determine the parenthood between SCCs
  *
  * @param	SCC
  */
void Preferred::SCCParenthood( list< SCC* >* SCCList )
{
	// Per ogni SCC nella lista i suoi padri possono essere soltanto i precedenti nella lista, faccio una scansione con doppio ciclo verificando se l'intersezione tra i nodi attaccati dalla SCC analizzata e la SCC attuale è vuota o meno (nel secondo caso la SCC analizzata è padre)
	for( list< SCC* >::iterator aSCC = SCCList -> begin(); aSCC != SCCList -> end(); ++aSCC )
		for( list< SCC* >::iterator fatherCandidate = SCCList -> begin(); fatherCandidate != aSCC; fatherCandidate++ )
		{
			SymbolicArgumentsSet candidateAttacks = ( *fatherCandidate ) -> argumentList -> getAttacks( this -> af );
			SymbolicArgumentsSet intersection = ( *aSCC ) -> argumentList -> intersect( &candidateAttacks );

			if( !intersection.isEmpty() )
				( *aSCC ) -> fathers.push_back( *fatherCandidate );
		}
}

/**
 * @brief Tarjan Algorithm for SCC determination
 *
 * @param node	The Node for which the argument is executed
 * @param SCC	The list of SCCs found
 * @param s		A temporal stack containing the actual SCC
 * @param index	An integer containing the level actually visited
 */
int Preferred::TarjanAlg( DFSNode* node, list< SCC* >* SCCList, stack< DFSNode* >* s, int index )
{
	if ( debug )
		cerr << "\tEntering TarjanAlg.\n";

	// Imposto la profondità del nodo in esame alla profondità minore finora incontrata
	// Al termine della procedura, se il lowlink del nodo è ancora pari al suo indice significa che esso è la radice di un SCC che sarà contenuto nello stack s. Questo viene dunque poppato ed inserito in un nuovo SCC
	
	node -> index = index;
	node -> lowlink = index;

	index++;

	s -> push( node );

	SetArguments* successors = node -> argument -> get_attacks();

	for( SetArgumentsIterator i = successors -> begin(); i != successors -> end(); i++ ) {
		// Devo ricavare il DFSNode associato all'argument attuale
		DFSNode* actual = searchArgument( ( *i ) );
		
		if ( debug )
			cerr << "\t\tLooking at " << actual->argument->getName() << endl;

		// Scandisco tutti i nodi attaccati dal nodo attuale: se qualcuno non è ancora stato visitato lo visito, altrimenti determino quale dei due è più vicino alla radice
		if( actual -> index == -1 ) {
			if ( debug )
				cerr << "\t\t\tNot yet visited. Recurring\n";

			index = TarjanAlg( actual, SCCList, s, index );
			node -> lowlink = min( node -> lowlink, actual -> lowlink );
		} else if( stackSearch( *s, actual ) ) {
			if ( debug )
				cerr << "\t\t\tFound in stack.\n";

			node -> lowlink = min( node -> lowlink, actual -> index );
		} else if ( debug )
			cerr << "\t\t\tNot found in stack.\n";

		if ( debug )
			cerr << "\t\t" << node -> argument -> getName() << "'s lowlink: " << node->lowlink << endl;
	}

	// Se il nodo node è un nodo radice allora s è un nuovo SCC, lo trasferisco interamente in un nuovo SetArguments all'interno di SCC (poppando tutti i nodi finchè non si giunge al nodo sul quale stiamo lavorando a questo livello di invocazione ricorsiva)
	// Ovviamente la SCC non conterrà i DFSNode ma soltanto l'Argument
	if( node -> lowlink == node -> index ) {
		if ( debug )
			cerr << "\t--- Building SCC ---" << endl;

		SymbolicArgumentsSet* new_SCC = new SymbolicArgumentsSet();
		DFSNode* temp;

		do {
			// Vedi il commento in stackSearch
			temp = s -> top();
			s -> pop();
			new_SCC -> add( temp -> argument -> getName() );
		} while( temp -> argument != node -> argument  );

		SCCList -> push_front( new SCC( new_SCC ) );
	}

	if ( debug )
		cerr << "\tFinished TarjanAlg on node " << node -> argument -> getName() << endl;
	return index;
}

/**
  * @brief Initialization of the support structure for DFS (called in constructor)
  */
void Preferred::initDFSAF()
{
	if ( debug )
		cerr << "Initializing DFSAF structure\n";

	// Inizializzo la struttura dati e copio gli Argument
	this -> DFSAF = list< DFSNode* >();

	for( SetArgumentsIterator i = af -> get_arguments() -> begin(); i != af -> get_arguments() -> end(); i++ )
		DFSAF.push_back( new DFSNode( *i ) );
}

/**
  * @brief Searches for an element in a stack
  *
  * @param s The stack to search in
  * @param node The DFSNode to search for
  */
bool Preferred::stackSearch( stack< DFSNode* > s, DFSNode* node )
{
	if ( debug )
		cerr << "\t\tSearching in stack for " << node -> argument -> getName() << endl;
	
	DFSNode* actual;

	// pop rimuove l'elemento in testa ma non lo restituisce, per cui è necessario usare prima top() (che invece lo restituisce senza eliminarlo)
	while( !s.empty() ) {
		actual = s.top();
		s.pop();

		/*
		if ( debug )
			cerr << "\t\t\tNode in stack: " << actual -> argument -> getName() << endl;
		*/

		if( actual -> argument == node -> argument )
			return true;
	}

	return false;
}

/**
  * @brief Searches the DFSNode corresponding to the given Argument in DFSAF
  *
  * @param node The Argument to search for
  */
Preferred::DFSNode* Preferred::searchArgument( Argument* node )
{
	if ( debug )
		cerr << "\t\tSearching DFSNode corresponding to Argument " << node -> getName() << endl;

	for( list< DFSNode* >::iterator i = DFSAF.begin(); i != DFSAF.end(); i++ )
		if( ( *i ) -> argument == node )
			return ( *i );

	return NULL;
}
