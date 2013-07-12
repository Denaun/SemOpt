/**
 * @file SymbolicArgumentsSet.cpp
 * @class SymbolicArgumentsSet
 * @author Maurizio Zucchelli
 * @version 0.1.0
 * @date 2013-07-08
 */

#include "SymbolicArgumentsSet.h"

using namespace std;

/* Constructors */

/**
 * @brief Simple constructor.
 */
SymbolicArgumentsSet::SymbolicArgumentsSet()
{
	this->arguments = set<string>();
}

/**
 * @brief Constructor from a real set of objects.
 *
 * @param[in]	source	The set to be represented symbolically.
 */
SymbolicArgumentsSet::SymbolicArgumentsSet( const SetArguments source )
{
	for ( SetArgumentsIterator it = source.begin(); it != source.end(); ++it )
		this->add( (*it)->getName() );
}

/**
 * @brief Constructor from a Labelling.
 * @details The resulting object will contain only the Arguments labelled as `in`.
 *
 * @param[in]	source	The Labelling to be represented symbolically.
 */
SymbolicArgumentsSet::SymbolicArgumentsSet( const Labelling source )
{
	Labelling* src = const_cast< Labelling* >( &source );

	for ( SetArgumentsIterator it = src->inargs()->begin();
			it != src->inargs()->end(); ++it )
		this->add( (*it)->getName() );
}

/* Destructor */

SymbolicArgumentsSet::~SymbolicArgumentsSet()
{
	// Dummy destructor
}

/* Iterators */

/**
 * @brief Iterator to the begin of this set.
 *
 * @return An iterator pointing at the first of the elements in this set.
 */
SymbolicArgumentsSet::iterator SymbolicArgumentsSet::begin() const
{
	return arguments.begin();
}

/**
 * @brief Iterator to the end of this set.
 *
 * @return An iterator pointing at the last of the elements in this set.
 */
SymbolicArgumentsSet::iterator SymbolicArgumentsSet::end() const
{
	return arguments.end();
}

/* Getters */

/**
 * @brief Checks whether or not this set is empty.
 *
 * @return `true` if the set is empty, `false` otherwise.
 */
bool SymbolicArgumentsSet::isEmpty() const
{
	return arguments.empty();
}

/**
 * @brief Returns the cardinality of this set.
 *
 * @return The number of elements in this set.
 */
size_t SymbolicArgumentsSet::size() const
{
	return arguments.size();
}

/**
 * @brief Checks whether an argument exists in this set.
 *
 * @param[in]	element	The element to search for.
 *
 * @return `true` if the element belongs to the set, `false` otherwise.
 */
bool SymbolicArgumentsSet::exists( const string element ) const
{
	return ( arguments.find( element ) != this->end() );
}

/**
 * @brief Returns a new set containing the attack of this set.
 *
 * @param[in]	theAF	The AF containing the description of the attacks of this set.
 *
 * @return A symbolic representation of the attacks of this set.
 */
SymbolicArgumentsSet SymbolicArgumentsSet::getAttacks( const AF* theAF ) const
{
	return SymbolicArgumentsSet( *this->toSetArguments( theAF ).get_attacks() );
}

/**
 * @brief Converts this set in a set containing the real arguments.
 *
 * @param[in]	theAF	The AF with the real elements of this set.
 *
 * @return A set containing the elements of the AF indexed by this set.
 */
SetArguments SymbolicArgumentsSet::toSetArguments( const AF* theAF ) const
{
	SetArguments result = SetArguments();

	for ( SymbolicArgumentsSet::iterator it = this->begin(); it != this->end(); ++it )
		try
		{
			result.add_Argument( const_cast<AF*>( theAF )->getArgumentByName( *it ) );
		}
		catch ( const out_of_range& oor )
		{
			// Element not in the AF. Skip it.
		}

	return result;
}

/**
 * @brief Converts this set in a Labelling containing the real arguments marked as `in`.
 *
 * @param[in]	theAF	The AF with the real elements of this set.
 *
 * @return A Labelling containing the elements of the AF indexed by this set marked as `in`.
 */
Labelling SymbolicArgumentsSet::toLabelling( const AF* theAF ) const
{
	Labelling result = Labelling();

	for ( SymbolicArgumentsSet::iterator it = this->begin(); it != this->end(); ++it )
		try
		{
			result.add_label(
					const_cast<AF*>( theAF )->getArgumentByName( *it ),
					Labelling::lab_in );
		}
		catch ( const out_of_range& oor )
		{
			// Element not in the AF. Skip it.
		}

	return result;
}

/* Setters */

/**
 * @brief Adds an element to this set.
 *
 * @param[in]	element	The name of the element to be added.
 */
void SymbolicArgumentsSet::add( const string element )
{
	arguments.insert( element );
}

/**
 * @brief Removes an element from this set.
 *
 * @param[in]	element	The name of the element to be removed.
 */
void SymbolicArgumentsSet::remove( const string element )
{
	arguments.erase( element );
}

/**
 * @brief Removes every element from this set.
 */
void SymbolicArgumentsSet::clear()
{
	arguments.clear();
}

/* Operators */

/**
 * @brief Calculates the intersection between the elements of this set and the other.
 *
 * @param[in]	other	The other operand.
 *
 * @return A set containing the elements belonging to both the sets.
 */
SymbolicArgumentsSet SymbolicArgumentsSet::intersect( const SymbolicArgumentsSet* other ) const
{
	SymbolicArgumentsSet result = SymbolicArgumentsSet();

	for ( SymbolicArgumentsSet::iterator it = this->begin(); it != this->end(); ++it )
		if ( other->exists( *it ) )
			result.add( *it );

	return result;
}

/**
 * @brief Calculates the difference between the elements of this set and the other.
 *
 * @param[in]	other	The other operand.
 *
 * @return A set containing the elements belonging to this set but not the other.
 */
SymbolicArgumentsSet SymbolicArgumentsSet::minus( const SymbolicArgumentsSet* other ) const
{
	SymbolicArgumentsSet result = *this;

	for ( SymbolicArgumentsSet::iterator it = this->begin(); it != this->end(); ++it )
		if ( other->exists( *it ) )
			result.remove( *it );

	return result;
}

/**
 * @brief Calculates the union between the elements of this set and the other.
 *
 * @param[in]	other	The other operand.
 *
 * @return A set containing the elements belonging to at least one of the sets.
 */
SymbolicArgumentsSet SymbolicArgumentsSet::merge( const SymbolicArgumentsSet* other ) const
{
	SymbolicArgumentsSet result = *other;

	for ( SymbolicArgumentsSet::iterator it = this->begin(); it != this->end(); ++it )
			result.add( *it );

	return result;
}

/**
 * @brief Assignment operator.
 *
 * @param[in]	other	The set source of the assignment.
 *
 * @return A reference to this set, now a copy of the other one.
 */
SymbolicArgumentsSet& SymbolicArgumentsSet::operator=( const SymbolicArgumentsSet& other )
{
	this->arguments = other.arguments;

	return *this;
}

/**
 * @brief Equality operator.
 *
 * @param[in]	other	The other operand.
 *
 * @return `true` if the two sets have the same elements, `false` otherwise.
 */
bool SymbolicArgumentsSet::operator==( const SymbolicArgumentsSet& other ) const
{
	return this->arguments == other.arguments;
}

/**
 * @brief Minority operator.
 *
 * @param[in]	other	The other operand.
 *
 * @return `true` if the two sets have the same elements, `false` otherwise.
 */
bool SymbolicArgumentsSet::operator<( const SymbolicArgumentsSet& other ) const
{
	return this -> arguments.size() < other.arguments.size();
}

/* Output */

/**
 * @brief Output operator
 *
 * @param[out]	out		The output stream to write on.
 * @param[in]	object	The set to be written.
 *
 * @return The output stream.
 */
ostream& operator<<( ostream& out, const SymbolicArgumentsSet& object )
{
	out << "{";

	for ( SymbolicArgumentsSet::iterator it = object.begin(); it != object.end(); )
	{
		out << *it;
		if ( ++it != object.end() )
			out << " ";
	}

	out << "}";

	return out;
}
