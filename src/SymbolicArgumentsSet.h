/**
 * @file SymbolicArgumentsSet.h
 * @class SymbolicArgumentsSet
 * @brief Class representing a set of arguments in a symbolical way (i.e. managing just the names of the arguments).
 * @author Maurizio Zucchelli
 * @version 0.1.0
 * @date 2013-07-08
 */

#ifndef SYMBOLIC_ARGUMENTS_SET_H_
#define SYMBOLIC_ARGUMENTS_SET_H_

#include <set>
#include <string>

#include "Argument.h"
#include "Labelling.h"
#include "SetArguments.h"

/**
 * @brief Class representing a set of arguments in a symbolical way.
 * @details
 * 	(i.e. managing just the names of the arguments).
 */
class SymbolicArgumentsSet 
{
	private:
		set<std::string> arguments;
	public:
		// Iterators
		typedef set<std::string>::iterator iterator;
		iterator begin() const;
		iterator end() const;

		// Constructors
		SymbolicArgumentsSet();
		SymbolicArgumentsSet( const SetArguments* );
		SymbolicArgumentsSet( const Labelling* );

		// Destructor
		virtual ~SymbolicArgumentsSet();

		// Getters
		bool isEmpty() const;
		size_type size() const;
		bool exists( const std::string ) const;
		SymbolicArgumentsSet get_attacks( const AF* ) const;
		SetArguments toSetArguments() const;
		Labelling toLabelling() const;

		// Setters
		void add( const std::string );
		void remove( const std::string );
		void clear();

		// Operations
		SymbolicArgumentsSet intersect( const SymbolicArgumentsSet* ) const;
		SymbolicArgumentsSet minus( const SymbolicArgumentsSet* ) const;
		SymbolicArgumentsSet& operator=( const SymbolicArgumentsSet* );
		bool operator==( const SymbolicArgumentsSet& ) const;
}

ostream& operator<<( ostream&, const SymbolicArgumentsSet& );

#endif /* defined SYMBOLIC_ARGUMENTS_SET_H_ */
