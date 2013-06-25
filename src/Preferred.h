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
#include "AF.h"
#include "SATFormulae.h"
#include <iostream>
#include <sstream>

using namespace std;

int precosat_lib(stringstream *the_cnf, int num_var, int num_cl, vector<int> *result);

extern bool debug;

class Preferred
{
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
	// Metodi aggiunti
	// Risoluzione, il secondo parametro conterrà i nodi della soluzione
	void pref( AF*, SetArguments* );
	// Restituisce due set contenente l'uno i nodi dell'insieme passato come secondo parametro che sono attaccati dai nodi del terzo parametro e l'altro tutti gli altri
	void boundcond( AF*, SetArguments*, SetArguments* );
	// Dato l'arg framework restituisce tutti i set SCC presenti ordinati per strati (il primo nella sequenza restituita non sarà attaccato da nessuno, i successivi possono essere attaccati solo dai precedenti)
	void SCCSEQ( AF* );
	// Cerca nell'AF i nodi che non sono attaccati da nessuno e li restituisce (altro valore restituito è il set di nodi non attaccati dai nodi liberi contenuti nel primo set)
	void Grounded( SetArguments*, SetArguments* );
	iterator begin();
	iterator end();
};

#endif /* PREFERRED_H_ */
