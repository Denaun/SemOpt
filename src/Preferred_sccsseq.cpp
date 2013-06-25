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
 * @return A list containing the sets of Strongly
 *		Connected Components found by the algorithm
 */
list< SetArguments* > Preferred::SCCSSEQ() {
	if ( debug )
		cerr << "Entering SCCSSEQ.\n";

	// L'algoritmo di Tarjan si basa sulla DFS (Depth-First Search), mi serve dunque una struttura ausiliaria per attribuire un indice relativo alla DFS ad ogni Argument (nodo)
	// Definisco una lista contenente tutti gli Argument dell'AF a cui viene associato l'indice della DFS
	list< DFSNode* > nodes = list< DFSNode* >();

	for( SetArgumentsIterator i = af -> get_arguments() -> begin(); i != af -> get_arguments() -> end(); i++ )
		nodes.push_back( new DFSNode( *i ) );

	// SCC sarà la lista dei set di componenti fortemente connesse, lo stack è ausiliario per Tarjan (contiene lo stack dei nodi visitati in una particolare invocazione)
	list< SetArguments* > SCC = list< SetArguments* >();
	stack< DFSNode* > s = stack< DFSNode* >();

	int index = 0;

	// Si scorrono tutti i nodi assicurandosi che eventuali nodi non raggiungibili dal primo siano comunque visitati. Se il grafo è connesso il metodo TarjanAlg viene eseguito una sola volta (una sola ricerca in profondità)
	for( list< DFSNode* >::iterator i = nodes.begin(); i != nodes.end(); i++ )
		if( (*i) -> index == -1 )
		{
			if ( debug )
				cerr << "\tCalling TarjanAlg for " << (*i)->argument->getName() << endl;

			TarjanAlg( *i, &nodes, &SCC, &s, index );
		}

	return SCC;
}

/**
 * @brief Tarjan Algorithm for SCC determination
 *
 * @param node	
 * @param nodes	
 * @param SCC	
 * @param s		
 * @param index	
 */
void Preferred::TarjanAlg( DFSNode* node, list< DFSNode* >* nodes, list< SetArguments* >* SCC, stack< DFSNode* >* s, int index ) {
	if ( debug )
		cerr << "Entering TarjanAlg.\n";

	// Imposto la profondità del nodo in esame alla profondità minore finora incontrata
	// Al termine della procedura, se il lowlink del nodo è ancora pari al suo indice significa che esso è la radice di un SCC che sarà contenuto nello stack s. Questo viene dunque poppato ed inserito in un nuovo SCC
	
	node -> index = index;
	node -> lowlink = index;

	index++;

	s -> push( node );

	SetArguments* successors = node -> argument -> get_attacks();

	for( SetArgumentsIterator i = successors -> begin(); i != successors -> end(); i++ ) {
		// Devo ricavare il DFSNode associato all'argument attuale
		DFSNode* actual = searchArgument( ( *i ), nodes );
		
		if ( debug )
			cerr << "\tLooking at " << actual->argument->getName() << endl;

		// Scandisco tutti i nodi attaccati dal nodo attuale: se qualcuno non è ancora stato visitato lo visito, altrimenti determino quale dei due è più vicino alla radice
		if( actual -> index == -1 ) {
			if ( debug )
				cerr << "\t\tNot yet visited. Reiterating\n";

			TarjanAlg( actual, nodes, SCC, s, index );
			node -> lowlink = min( node -> lowlink, actual -> lowlink );
		} else if( stackSearch( *s, actual  ) ) {
			if ( debug )
				cerr << "\t\tIn stack.\n";

			node -> lowlink = min( node -> lowlink, actual -> lowlink );
		}

		if ( debug )
			cerr << "\tNode's lowlink: " << node->lowlink << endl;
	}

	// Se il nodo node è un nodo radice allora s è un nuovo SCC, lo trasferisco interamente in un nuovo SetArguments all'interno di SCC (poppando tutti i nodi finchè non si giunge al nodo sul quale stiamo lavorando a questo livello di invocazione ricorsiva)
	// Ovviamente la SCC non conterrà i DFSNode ma soltanto l'Argument
	if( node -> lowlink == node -> index ) {
		SetArguments* new_SCC = new SetArguments();
		DFSNode* temp;

		do {
			// Vedi il commento in stackSearch
			temp = s -> top();
			s -> pop();
			new_SCC -> add_Argument( temp -> argument );
		} while( temp -> argument != node -> argument  );

		SCC -> push_back( new_SCC );
	}

	if ( debug )
		cerr << "Finished TarjanAlg.\n";
}

/**
  * @brief Searches for an element in a stack
  */
bool Preferred::stackSearch( stack< DFSNode* > s, DFSNode* node ) {
	// pop rimuove l'elemento in testa ma non lo restituisce, per cui è necessario usare prima top() (che invece lo restituisce senza eliminarlo)
	while( DFSNode* actual = s.top() ) {
		s.pop();

		if( actual -> argument == node -> argument )
			return true;
	}

	return false;
}

DFSNode* Preferred::searchArgument( Argument* node, list< DFSNode* >* nodeList ) {
	for( list< DFSNode* >::iterator i = nodeList -> begin(); i != nodeList -> end(); i++ )
		if( ( *i ) -> argument == node )
			return ( *i );

	return NULL;
}
