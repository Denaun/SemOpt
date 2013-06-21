/**
 * @file 		AF.cpp
 * @class 		AF
 * @brief 		Class encompassing an Argumentation Framework
 * @author 		Mauro Vallati <m.vallati@hud.ac.uk>
 * @copyright	GNU Public License v2
 */


#include "AF.h"

/**
 * @brief 	Simple constructor
 */
AF::AF()
{
	this->arguments = new SetArguments();
	//this->raw_attacks = map<int, int >();
	//this->attacks = map<Argument *, SetArguments *>();
	//this->attackers = map<Argument *, SetArguments *>();
}


AF::~AF()
{
	// TODO Auto-generated destructor stub
}

/**
 * @brief 		Method for parsing a ASPARTIX compliant file
 * @details		It uses part of the code from Samer Nofal's, University of Liverpool
 * @param[in] file A string representing the (relative/absolute) path to the ASPARTIX compliant file
 * @retval bool It returns `True` if it can correctly parse the file, `False` otherwise
 */
bool AF::readFile(string file)
{
	string inLine;
	ifstream infile;
	infile.open(file.c_str());
	if (!infile.good())
		return false;
	infile >> inLine;
	while (inLine.find("arg") != string::npos && !infile.eof())
	{
		this->arguments->add_Argument(new Argument(
				inLine.substr(4, inLine.find_last_of(")") - 4),
				this->numArgs(),
				this
				));
		infile >> inLine;
	}

	while (!infile.eof())
	{
		if (inLine.find("att") != string::npos)
		{
			Argument *source = this->getArgumentByName(
					(inLine.substr(4,
							inLine.find_last_of(",") - 4)));
			Argument *target = this->getArgumentByName(
					(inLine.substr(inLine.find_last_of(",") + 1,
							inLine.find_last_of(")")
									- inLine.find_last_of(",")
									- 1)));

			source->add_attacks(target);
			target->add_attackers(source);
//			this->raw_attacks.push_back(
//					pair<int, int>(
//							this->argNameToCode(
//									(inLine.substr(4,
//											inLine.find_last_of(",") - 4))),
//							this->argNameToCode(
//									(inLine.substr(inLine.find_last_of(",") + 1,
//											inLine.find_last_of(")")
//													- inLine.find_last_of(",")
//													- 1)))));
		}
		infile >> inLine;
	}
	infile.close();
	return true;
}

/**
 * @brief		This method returns the pointer to the Argument whose name is given as parameter
 * @param[in] name	 The name of the argument
 * @retval Argument* The pointer to the Argument object, or NULL if not found
 */
Argument *AF::getArgumentByName(string name)
{
	return this->arguments->getArgumentByName(name);
}

/**
 * @brief 	Returns the number of arguments
 * @retval int
 */
int AF::numArgs()
{
	return this->arguments->cardinality();
}


/**
 * @brief		This method returns the pointer to the Argument whose identification number is given as parameter
 * @param[in] num	 The name of the argument
 * @retval Argument* The pointer to the Argument object, or NULL if not found
 */
Argument *AF::getArgumentByNumber(int num)
{
	return this->arguments->getArgumentByNumber(num);
}

/**
 * @brief 	Begin of the iterator for the set of arguments
 * @retval SetArgumentsIterator An iterator pointing at the first of the elements of the set of arguments
 */
SetArgumentsIterator AF::begin()
{
	return this->arguments->begin();
}

/**
 * @brief 	End of the iterator for the set of arguments
 * @retval SetArgumentsIterator An iterator pointing at the last of the elements of the set of arguments
 */
SetArgumentsIterator AF::end()
{
	return this->arguments->end();
}

/**
 * @brief	Returns a pointer to the set of arguments
 * @retval SetArguments*
 */
SetArguments *AF::get_arguments() const
{
	return this->arguments;
}
