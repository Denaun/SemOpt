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
			TarjanAlg( *i, &SCC, &s, index );

	return SCC;
}

/**
  * @brief Tarjan Algotithm for SCC determination
  */
void Preferred::TarjanAlg( DFSNode* node, list< SetArguments* >* SCC, stack< DFSNode* >* s, int index ) {
	// Imposto la profondità del nodo in esame alla profondità minore finora incontrata
	// Al termine della procedura, se il lowlink del nodo è ancora pari al suo indice significa che esso è la radice di un SCC che sarà contenuto nello stack s. Questo viene dunque poppato ed inserito in un nuovo SCC
	node -> index = index;
	node -> lowlink = index;

	index++;

	s -> push_back( node );

	SetArguments* successors = node -> argument -> get_attacks();

	for( auto i = successors -> begin(); i != successors -> end(); i++ ) {
		// Scandisco tutti i nodi attaccati dal nodo attuale: se qualcuno non è ancora stato visitato lo visito, altrimenti determino quale dei due è più vicino alla radice
		if( *i -> index == -1 ) {
			TarjanAlg( *i, SCC, s, index );
			node -> lowlink = min( node -> lowlink, *i -> lowlink );
		} else if( stackSearch( s, *i  ) )
			node -> lowlink = min( node -> lowlink, *i -> lowlink );
	}

	// Se il nodo node è un nodo radice allora s è un nuovo SCC, lo trasferisco interamente in un nuovo SetArguments all'interno di SCC (poppando tutti i nodi finchè non si giunge al nodo sul quale stiamo lavorando a questo livello di invocazione ricorsiva)
	// Ovviamente la SCC non conterrà i DFSNode ma soltanto l'Argument
	if( node -> lowlink == node -> index ) {
		SetArguments* new_SCC = new SetArguments();

		do {
			DFSNode* temp = s -> pop_back();
			new_SCC -> push_back( temp -> argument );
		} while( temp -> argument != node -> argument  );

		SCC -> push_back( new_SCC );
	}
}

/**
  * @brief Searches for an element in a stack
  */
bool Preferred::stackSearch( stack< DFSNode* > s, DFSNode* node ) {
	stack< DFSNode* > temp = *s;

	while( DFSNode* actual = temp -> pop_back() )
		if( actual -> argument == node -> argument )
			return true;

	return false;
}
