/*
 * Preferred.h
 *
 *  Created on: 10 Jun 2013
 *      Author: geryo
 */

#ifndef PREFERRED_H_
#define PREFERRED_H_

#include "Labelling.h"
#include "SetArguments.h"
#include <vector>
#include <list>
#include <stack>
#include "AF.h"
#include "SATFormulae.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

int precosat_lib(stringstream *the_cnf, int num_var, int num_cl, vector<int> *result);

extern bool debug;
extern bool stages;

// Una SCC è composta da un SetArgument e dalla lista di SCC padri (necessario per i miglioramenti)
// Deve essere una struttura globale
	struct SCC {
		SetArguments* argumentList;
		list< SCC* > fathers;
		
		SCC( SetArguments* _argumentList ) {
			this -> argumentList = _argumentList;
		}

		inline bool operator == ( const SCC &other ) const {
			return this -> argumentList == other.argumentList;
		}
	};

class Preferred
{
	// Mantengo una struttura a livello di classe che rappresenti l'AF associandovi dei valori necessari ad effettuare la ricerca in profondità
	// Un DFSNode è l'elemento base di questa struttura
	struct DFSNode {
		Argument* argument;
		int index;
		int lowlink;
		
		DFSNode( Argument* _argument ) {
			this -> argument = _argument;
			this -> index = -1;
			this -> lowlink = -1;
		}
	};

	list< DFSNode* > DFSAF;

	AF *af; //!< @brief The Argumentation Framework considered
	SetArguments *C; //!< @brief the set of arguments to consider
	int encoding;
	SATFormulae sat_new_pigreek;
	vector<Labelling> labellings;
	int compute_new_pigreek();
	bool satlab(SATFormulae, Labelling *);
	void cleanlabs();
public:
	typedef vector<Labelling>::iterator iterator;
	Preferred();
	virtual ~Preferred();
	void prefSAT(AF *, SetArguments *);

	// Metodo aggiunti
	// Risoluzione
	void pref( AF*, SetArguments* );

	iterator begin();
	iterator end();

private:
	// Metodi aggiunti
	// Restituisce due set contenente l'uno i nodi dell'insieme passato come secondo parametro che sono attaccati dai nodi del terzo parametro e l'altro tutti gli altri
	void boundcond( SetArguments*, SetArguments*, SetArguments*, SetArguments* );
	// Dato l'arg framework restituisce tutti i set SCC presenti ordinati per strati (il primo nella sequenza restituita non sarà attaccato da nessuno, i successivi possono essere attaccati solo dai precedenti)
	// Gli SCCS sono trovati tramite l'algoritmo di Tarjan che prevede l'esecuzione multipla (su più vertici)
	list< SCC* > SCCSSEQ();
	void TarjanAlg( DFSNode*, list< SCC* >*, stack< DFSNode* >*, int );
	// Cerca nell'AF i nodi che non sono attaccati da nessuno e li restituisce (altro valore restituito è il set di nodi non attaccati dai nodi liberi contenuti nel primo set)
	void Grounded( SetArguments*, SetArguments* );

	// Metodi aggiuntivi per SCCSSEQ
	void SCCParenthood( list< SCC* >* );
	void initDFSAF();
	bool stackSearch( stack< DFSNode* >, DFSNode* );
	DFSNode* searchArgument( Argument* );
};

#endif /* PREFERRED_H_ */
