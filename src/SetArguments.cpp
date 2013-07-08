/**
 * @file 		SetArguments.cpp
 * @class 		SetArguments
 * @brief 		Class encompassing a set of arguments
 * @author 		Federico Cerutti <federico.cerutti@acm.org>
 * @copyright	GNU Public License v2
 */

#include "SetArguments.h"

/**
 * @brief Simple constructor
 */
SetArguments::SetArguments()
{
	this->arguments = map<string, Argument *>();
	this->key_assoc = map<int, string>();
}

/**
 * @brief	Add an argument to the set
 * @param[in] arg Pointer to an Argument
 */
void SetArguments::add_Argument(Argument *arg)
{
	this->arguments.insert(pair<string, Argument *>(arg->getName(), arg));
	this->key_assoc.insert(pair<int, string>(arg->getNumber(), arg->getName()));
}

/**
 * @brief		Retuns the cardinality of this set
 * @retval int
 */
int SetArguments::cardinality() const
{
	return (int) this->arguments.size();
}

/**
 * @brief		This method returns the pointer to the Argument whose name is given as parameter
 * @param[in] name	 The name of the argument
 * @retval Argument* The pointer to the Argument object, or NULL if not found
 */
Argument *SetArguments::getArgumentByName(string name)
{
	return this->arguments.at(name);
}

/**
 * @brief		This method returns the pointer to the Argument whose identification number is given as parameter
 * @param[in] num	 The name of the argument
 * @retval Argument* The pointer to the Argument object, or NULL if not found
 */
Argument *SetArguments::getArgumentByNumber(int num)
{
	return this->arguments.at(this->key_assoc.at(num));
}

/**
 * @brief	Check wether or not this set is empty
 * @retval bool
 */
bool SetArguments::empty()
{
	return this->arguments.empty();
}

/**
 * @brief	Build and return a SetArguments containing the args attacked by the local args outside the set
 */
SetArguments* SetArguments::get_attacks() const
{
	SetArguments* attacks = new SetArguments();

	// Iterate over the arguments of the set
	for( SetArgumentsIterator argument = this -> begin();
			argument != this -> end(); argument++ )
	{
		// Add the attacks to the set.
		// Clone doesn't clear the set nor adds an argument if it already exists.
		(*argument)->get_attacks()->clone( attacks );

		/*
		SetArguments* local_attacks = ( *argument ) -> get_attacks();

		// Iterate over the attacks of every argument.
		// An attack is added to the attack list if it's not already there and it's directed to the outside
		for( SetArgumentsIterator attack = local_attacks -> begin();
				attack != local_attacks -> end(); attack++ )
			if( !( this -> exists( *attack ) ) && !( attacks -> exists( *attack ) ) )
				attacks -> add_Argument( *attack );
		*/
	}

	return attacks;
}

/**
 * @brief 	Begin of the iterator for this set of arguments
 * @retval SetArgumentsIterator An iterator pointing at the first of the elements of this set
 */
SetArgumentsIterator SetArguments::begin() const
{
	return SetArgumentsIterator((this->arguments.begin()));
}

/**
 * @brief 	End of the iterator for this set of arguments
 * @retval SetArgumentsIterator An iterator pointing at the last of the elements of this set
 */
SetArgumentsIterator SetArguments::end() const
{
	return SetArgumentsIterator(this->arguments.end());
}

/**
 * @brief Check whether this set is a subset of `other`
 * @param[in] other	The other set to check
 * @retval bool
 */
bool SetArguments::is_subset_equal(SetArguments *other)
{
	if (this->empty() && other->empty())
		return true;
	if (this->empty() && !other->empty())
		return true;
	if (!this->empty() && other->empty())
		return false;
	if (this->cardinality() > other->cardinality())
		return false;
	for (SetArgumentsIterator it = this->begin(); it != this->end(); it++)
	{
		if (!other->exists((*it)))
			return false;
	}
	return true;
}

/**
 * @brief Check whether this set is a proper subset of `other`
 * @param[in] other	The other set to check
 * @retval bool
 */
bool SetArguments::is_subset(SetArguments *other)
{
	if ((this->cardinality() < other->cardinality()) &&
			this->is_subset_equal(other))
		return true;
	else
		return false;
}

/**
 * @brief Set Minus
 * @details	Outputs a set containing the difference between this Set
 * 			and the other one specified.
 * @param[in] other The other term of the subtraction
 * @param[out] res	The result of the subtraction
 * @retval void
 */
void SetArguments::setminus(SetArguments *other, SetArguments *result)
{
	this->clone(result);
	for (SetArgumentsIterator it = other->begin(); it != other->end(); it++)
	{
		if (result->exists((*it)))
			result->remove((*it));
	}
}

/**
 * @brief 	Check whether or not an argument exists in this set
 * @param[in] arg
 * @retval bool
 */
bool SetArguments::exists(Argument * arg) const
{
	return (this->arguments.find(arg->getName()) != this->arguments.end());
}

/**
 * @brief 	Remove an argument from this set
 * @param[in] arg
 * @retval void
 */
void SetArguments::remove(Argument * arg)
{
	this->arguments.erase(arg->getName());
}

/**
 * @brief 	Clone this set
 * @param[in] set The set that will be the new clone
 * @retval void
 */
void SetArguments::clone(SetArguments * set)
{
	for (SetArgumentsIterator it = this->begin(); it != this->end(); it++)
	{
		set->add_Argument((*it));
	}
}

/**
 * @brief Intersect this set the `other` set and write the result in `res`
 * @param[in] other The other term of the intersection
 * @param[out] res	The result of the intersection
 * @retval void
 */
void SetArguments::intersect(SetArguments *other, SetArguments *res)
{
	for (SetArgumentsIterator it = this->begin(); it != this->end(); it++)
	{
		// TODO Could be much easier to just call exists.....
		for (SetArgumentsIterator inner = other->begin(); inner != other->end();
				inner++)
		{
			if (*(*it) == *(*inner))
			{
				res->add_Argument(*it);
			}
		}
	}
}

/**
 * @brief Adapts the elements of `this` set to the AF, substituting the matching elements.
 * @details
 * 	*NOTE*: This method changes the structure of the SetArguments on which it is called.
 *
 * @param[in]	other	The AF source of the substitution arguments.
 */
void SetArguments::adaptTo( AF* other )
{
	for ( SetArgumentsIterator it = other->begin(); it != other->end(); ++it )
	{
		try
		{
			Argument* victim = this->getArgumentByName( (*it)->getName() );

			// No exception: the Argument is inside this set
			// Remove it and substitute it with the new one
			this->remove( victim );
			this->add_Argument( *it );
		}
		catch ( const out_of_range& oor )
		{
			// The Argument is outside this set. Nothing to do.
		}
	}
}

/**
 * @brief 		Overloading of the == operator
 * @param[in] 	other   The other term of the comparison
 * @retval bool
 */
bool SetArguments::operator==(const SetArguments &other) const
{
	if (this->cardinality() != other.cardinality())
		return false;
	for(SetArgumentsIterator it = this->begin(), it2 = other.begin(); it != this->end() and it2 != other.begin(); it++, it2++)
	{
		if ((*it) != (*it2))
			return false;
	}
	return true;
}

SetArguments::~SetArguments()
{
	// TODO Auto-generated destructor stub
}

ostream& operator<<(ostream& out, const SetArguments& r)
{
	out << "{";

	SetArgumentsIterator it;
	for (it = r.begin(); it != r.end();)
	{
		out << (*it)->getName();
		if (++it != r.end())
			out << " ";
	}
	out << "}";
	return out;
}

