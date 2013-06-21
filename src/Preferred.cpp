/*
 * Preferred.cpp
 *
 *  Created on: 10 Jun 2013
 *      Author: geryo
 */

#include "Preferred.h"

void Preferred::cleanlabs()
{
	while (!this->labellings.empty())
		this->labellings.pop_back();
}

/**
 * @brief Method for filling in the attribute sat_new_pigreek
 */
int Preferred::compute_new_pigreek()
{
	SetArguments subtraction = SetArguments();
	this->af->get_arguments()->setminus(this->C, &subtraction);

	for (SetArgumentsIterator it_sub = subtraction.begin();
			it_sub != subtraction.end(); it_sub++)
	{
		if (debug)
		{
			cout << "Subtraction " << (*it_sub)->getName() << endl;
		}
		//c2
		this->sat_new_pigreek.appendOrClause(
				OrClause(1, (*it_sub)->NotInVar()));
		this->sat_new_pigreek.appendOrClause(
				OrClause(2, (*it_sub)->OutVar(), (*it_sub)->UndecVar()));
		this->sat_new_pigreek.appendOrClause(
				OrClause(2, (*it_sub)->NotOutVar(), (*it_sub)->NotUndecVar()));
	}

	if (debug)
	{
		cout << "Set arguments: " << endl;
		cout << *(this->af->get_arguments()) << endl;

		cout << "C set:" << endl;
		cout << *(this->C) << endl;
	}

	OrClause noempty_clause = OrClause();
	for (SetArgumentsIterator it_args = this->C->begin();
			it_args != this->C->end(); it_args++)
	{
		//c1
		if (encoding == 0 || encoding == 1 || encoding == 2 || encoding == 3
				|| encoding == 4 || encoding == 5)
		{
			this->sat_new_pigreek.appendOrClause(
					OrClause(3, (*it_args)->InVar(), (*it_args)->OutVar(),
							(*it_args)->UndecVar()));

			this->sat_new_pigreek.appendOrClause(
					OrClause(2, (*it_args)->NotInVar(),
							(*it_args)->NotOutVar()));

			this->sat_new_pigreek.appendOrClause(
					OrClause(2, (*it_args)->NotInVar(),
							(*it_args)->NotUndecVar()));

			this->sat_new_pigreek.appendOrClause(
					OrClause(2, (*it_args)->NotOutVar(),
							(*it_args)->NotUndecVar()));
		}

		noempty_clause.appendVariable((*it_args)->InVar());

		SetArguments *pred = (*it_args)->get_attackers();

		if (debug)
		{
			cout << "predecessors of " << (*it_args)->getName() << ": ";

			for (SetArgumentsIterator it_pred = pred->begin();
					it_pred != pred->end(); it_pred++)
				cout << (*it_pred)->getName() << " ";
			cout << endl;
		}

		if (pred->empty())
		{
			//c3
			this->sat_new_pigreek.appendOrClause(
					OrClause(1, (*it_args)->InVar()));

			this->sat_new_pigreek.appendOrClause((*it_args)->NotOutVar());

			this->sat_new_pigreek.appendOrClause((*it_args)->NotUndecVar());
		}
		else
		{
			OrClause c4_last_clause = OrClause();
			OrClause c9_or_undec_clause = OrClause();
			OrClause c8_bigor_clause = OrClause();

			for (SetArgumentsIterator it_pred = pred->begin();
					it_pred != pred->end(); it_pred++)
			{
				//c5
				if (encoding == 0 || encoding == 1 || encoding == 3
						|| encoding == 5)
				{
					this->sat_new_pigreek.appendOrClause(
							OrClause(2, (*it_args)->NotInVar(),
									(*it_pred)->OutVar()));
				}

				//c4
				if (encoding == 0 || encoding == 2 || encoding == 3
						|| encoding == 5)
				{
					c4_last_clause.appendVariable((*it_pred)->NotOutVar());
				}

				//c9
				if (encoding == 0 || encoding == 1 || encoding == 4
						|| encoding == 5)
				{
					this->sat_new_pigreek.appendOrClause(
							OrClause(2, (*it_args)->NotUndecVar(),
									(*it_pred)->NotInVar()));

					c9_or_undec_clause.appendVariable((*it_pred)->UndecVar());
				}

				if (encoding == 0 || encoding == 2 || encoding == 4
						|| encoding == 5)
				{
					c8_bigor_clause.appendVariable((*it_pred)->InVar());
				}
			}

			//c4-end
			if (encoding == 0 || encoding == 2 || encoding == 3
					|| encoding == 5)
			{
				c4_last_clause.appendVariable((*it_args)->InVar());
				this->sat_new_pigreek.appendOrClause(c4_last_clause);
			}

			//c9-end
			if (encoding == 0 || encoding == 1 || encoding == 4
					|| encoding == 5)
			{
				c9_or_undec_clause.addHeadVariable((*it_args)->NotUndecVar());
				this->sat_new_pigreek.appendOrClause(c9_or_undec_clause);
			}

			//c8
			if (encoding == 0 || encoding == 2 || encoding == 4
					|| encoding == 5)
			{
				for (SetArgumentsIterator it_pred = pred->begin();
						it_pred != pred->end(); it_pred++)
				{
					OrClause to_add = OrClause();
					c8_bigor_clause.clone(&to_add);
					to_add.appendVariable((*it_pred)->NotUndecVar());
					to_add.appendVariable((*it_args)->UndecVar());
					this->sat_new_pigreek.appendOrClause(to_add);
				}
			}

		}
	}

	for (SetArgumentsIterator it_args = this->af->begin();
			it_args != this->af->end(); it_args++)
	{
		OrClause c7_last_clause = OrClause();
		SetArguments *pred = (*it_args)->get_attackers();

		if (pred->empty())
			continue;

		for (SetArgumentsIterator it_pred = pred->begin();
				it_pred != pred->end(); it_pred++)
		{

			//c6
			if (encoding == 0 || encoding == 2 || encoding == 3
					|| encoding == 4)
			{
				this->sat_new_pigreek.appendOrClause(
						OrClause(2, (*it_pred)->NotInVar(),
								(*it_args)->OutVar()));
			}

			//c7
			if (encoding == 0 || encoding == 1 || encoding == 3
					|| encoding == 4)
			{
				c7_last_clause.appendVariable((*it_pred)->InVar());
			}
		}

		//c7-end
		if (encoding == 0 || encoding == 1 || encoding == 3 || encoding == 4)
		{
			c7_last_clause.addHeadVariable((*it_args)->NotOutVar());
			this->sat_new_pigreek.appendOrClause(c7_last_clause);
		}

	}

	this->sat_new_pigreek.appendOrClause(noempty_clause);

	return this->sat_new_pigreek.size();
}

/**
 * @brief Method for deriving a labelling from a SAT Formulae
 * @details	This method is a wrapper for a SAT Solver
 * @param[in] sat	The SATFormulae as input
 * @param[out] sat	The computed Labelling
 * @retval bool `true` if a solution is found, `false` otherwise
 */
bool Preferred::satlab(SATFormulae sat, Labelling *lab)
{
	stringstream cnf_string(stringstream::in | stringstream::out);
	sat.toSS(&cnf_string);

	if (debug)
	{
		cout << "Preparing the satsolver" << "\n";
		cout << cnf_string.str();
	}

	vector<int> lastcompfound = vector<int>();
	int retsat = precosat_lib(&cnf_string, 3 * af->numArgs(), sat.size(),
			&lastcompfound);

	if (debug)
		cout << retsat;

	if (retsat != 20)
	{
		for (int i = 0; i < af->numArgs(); i++)
		{
			if (lastcompfound.at(i) > 0)
			{
				lab->add_label(af->getArgumentByNumber(i), Labelling::lab_in);
				if (debug)
				{
					cout << af->getArgumentByNumber(i)->getName() << endl;
				}
				continue;
			}
			if (lastcompfound.at(i + af->numArgs()) > 0)
			{
				lab->add_label(af->getArgumentByNumber(i), Labelling::lab_out);
				continue;
			}
			if (lastcompfound.at(i + 2 * af->numArgs()) > 0)
			{
				lab->add_label(af->getArgumentByNumber(i),
						Labelling::lab_undec);
				continue;
			}
		}
		if (debug)
		{
			cout << "in " << lab->inargs()->cardinality() << endl;
			cout << "out " << lab->outargs()->cardinality() << endl;
			cout << "undec " << lab->undecargs()->cardinality() << endl;
		}
		return true;
	}
	return false;
}

void Preferred::prefSAT(AF *the_af, SetArguments *the_c)
{
	this->cleanlabs();
	this->af = the_af;
	this->C = the_c;
	this->compute_new_pigreek();
	SATFormulae cnf = SATFormulae();

	this->sat_new_pigreek.clone(&cnf);
	do
	{
		Labelling prefcand = Labelling();
		SATFormulae cnfdf = SATFormulae();
		cnf.clone(&cnfdf);

		while (true)
		{
			Labelling res = Labelling();
			if (!this->satlab(cnfdf, &res))
			{
				break;
			}

			if (debug)
			{
				cout << endl;
				cout << "{";
				SetArgumentsIterator it;
				for (it = res.inargs()->begin(); it != res.inargs()->end();
						it++)
				{
					cout << (*it)->getName() << " ";
				}
				cout << "}" << endl;
			}

			res.clone(&prefcand);
			if (res.inargs()->cardinality() == C->cardinality())
				break;

			SetArgumentsIterator iter;
			for (iter = res.inargs()->begin(); iter != res.inargs()->end();
					iter++)
			{
				cnfdf.appendOrClause(OrClause(1, (*iter)->InVar()));
			}

			SetArguments difference = SetArguments();
			this->C->setminus(res.inargs(), &difference);
			OrClause remaining = OrClause();
			for (iter = difference.begin(); iter != difference.end(); iter++)
			{
				remaining.appendVariable((*iter)->InVar());
			}
			cnfdf.appendOrClause(remaining);
		}

		if (prefcand.empty())
			break;

		this->labellings.push_back(prefcand);

		OrClause oppsolution = OrClause();
		SetArgumentsIterator it;

		SetArguments difference = SetArguments();
		this->C->setminus(prefcand.inargs(), &difference);
		for (it = difference.begin(); it != difference.end(); it++)
		{
			oppsolution.appendVariable((*it)->InVar());
		}
		cnf.appendOrClause(oppsolution);

	} while (true);

	if (this->labellings.empty())
	{
		this->labellings.push_back(Labelling());
	}
}

Preferred::Preferred()
{
	this->encoding = 0;
	this->labellings = vector<Labelling>();
	this->sat_new_pigreek = SATFormulae();
}

Preferred::~Preferred()
{

}

Preferred::iterator Preferred::begin()
{
	return this->labellings.begin();
}

Preferred::iterator Preferred::end()
{
	return this->labellings.end();
}
