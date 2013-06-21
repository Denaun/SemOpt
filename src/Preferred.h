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
	iterator begin();
	iterator end();
};

#endif /* PREFERRED_H_ */
