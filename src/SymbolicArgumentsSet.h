/**
 * @file SymbolicArgumentsSet.h
 * @class SymbolicArgumentsSet
 * @author Maurizio Zucchelli
 * @version 0.1.0
 * @date 2013-07-08
 */

#ifndef SYMBOLIC_ARGUMENTS_SET_H_
#define SYMBOLIC_ARGUMENTS_SET_H_

#include <set>
#include <string>
#include <stdexcept>

class AF;
class Argument;
class SetArguments;
class Labelling;

/**
 * @brief Class representing a set of Arguments in a symbolical way.
 * @details
 * 	(i.e. managing just the names of the arguments).
 */
class SymbolicArgumentsSet 
{
	private:
		std::set<std::string> arguments;
	public:
		// Constructors
		SymbolicArgumentsSet();
		SymbolicArgumentsSet( const SetArguments );
		SymbolicArgumentsSet( const Labelling );

		// Destructor
		virtual ~SymbolicArgumentsSet();

		// Iterators
		typedef std::set<std::string>::iterator iterator;
		iterator begin() const;
		iterator end() const;

		// Getters
		bool isEmpty() const;
		size_t size() const;
		bool exists( const std::string ) const;
		SymbolicArgumentsSet getAttacks( const AF* ) const;
		SymbolicArgumentsSet getAttackers( const AF* ) const;
		SetArguments toSetArguments( const AF* ) const;
		Labelling toLabelling( const AF* ) const;

		// Setters
		void add( const std::string );
		void remove( const std::string );
		void clear();

		// Operations
		SymbolicArgumentsSet intersect( const SymbolicArgumentsSet* ) const;
		SymbolicArgumentsSet minus( const SymbolicArgumentsSet* ) const;
		SymbolicArgumentsSet merge( const SymbolicArgumentsSet* ) const;
		SymbolicArgumentsSet& operator=( const SymbolicArgumentsSet& );
		bool operator==( const SymbolicArgumentsSet& ) const;
};

#include "Argument.h"
#include "SetArguments.h"
#include "Labelling.h"
#include "AF.h"

std::ostream& operator<<( std::ostream&, const SymbolicArgumentsSet& );

#endif /* defined SYMBOLIC_ARGUMENTS_SET_H_ */
