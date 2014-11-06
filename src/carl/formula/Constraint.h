/**
 * Constraint.h
 * @author Florian Corzilius<corzilius@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * @author Ulrich Loup
 * @since 2010-04-26
 * @version 2014-10-30
 */

#pragma once

//#define NDEBUG
//#define CONSTRAINT_WITH_FACTORIZATION

#include <iostream>
#include <cstring>
#include <sstream>
#include <assert.h>
#include "Relation.h"
#include "../interval/Interval.h"
#include "../interval/IntervalEvaluation.h"
#include "../core/Definiteness.h"
#include "../util/Common.h"
#include "config.h"


namespace carl
{
    // Forward declaration
    template<typename Pol>
    class ConstraintPool;
    
    /**
     * Class to create a constraint object.
     */
    template<typename Pol>
    class Constraint
    {
        friend class ConstraintPool<Pol>;
        
        private:
            /// A unique id.
            unsigned mID;
            /// The hash value.
            size_t mHash;
            /// The relation symbol comparing the polynomial considered by this constraint to zero.
            Relation mRelation;
            /// The polynomial which is compared by this constraint to zero.
            Pol mLhs;
            /// The factorization of the polynomial considered by this constraint.
            mutable Factors<Pol> mFactorization;
            /// A container which includes all variables occurring in the polynomial considered by this constraint.
            Variables mVariables;
            /// A map which stores information about properties of the variables in this constraint.
            mutable VarInfoMap<Pol> mVarInfoMap;
            /// Definiteness of the polynomial in this constraint.
            mutable Definiteness mLhsDefinitess;

            /**
             * Default constructor. (0=0)
             */
            Constraint();
            
            /**
             * Constructs the constraint:   _lhs _rel 0
             * @param _lhs The left-hand side of the constraint to construct being a polynomial.
             * @param _rel The relation symbol.
             * @param _id The unique id for this constraint. It should be maintained by a central instance
             *             as the offered ConstraintPool class, therefore the constructors are private and
             *             can only be invoked using the constraint pool or more precisely using the 
             *             method newConstraint( _lhs, _rel )  or
             *             newBound( x, _rel, b ) if _lhs = x - b and x is a variable
             *             and b is a rational.
             */
            Constraint( const Pol& _lhs, const Relation _rel, unsigned _id = 0 );
            
            /**
             * Copies the given constraint.
             * @param _constraint The constraint to copy.
             * @param _rehash A flag indicating whether to recalculate the hash value of the constraint to 
             *                 construct. This might be necessary in case the given constraint has been 
             *                 simplified but kept its hash value.
             */
            Constraint( const Constraint& _constraint, bool _rehash = false );

            /**
             * Destructor.
             */
            ~Constraint();
            
            /**
             * Initializes some basic information of the constraint, such as the definiteness of the left-hand 
             * side and specific information to each variable.
             */
            void init();
            
            void initVariableInformations()
            {
                // @todo Why does this not compile??
//                VariablesInformation<false,Pol> varinfos = mLhs.getVarInfo<false>();
//                for( auto varInfo = varinfos.begin(); varInfo != varinfos.end(); ++varInfo )
//                    mVarInfoMap.emplace_hint( mVarInfoMap.end(), *varInfo );
            }
            
        public:

            /**
             * @return The considered polynomial being the left-hand side of this constraint.
             *          Hence, the right-hand side of any constraint is always 0.
             */
            const Pol& lhs() const
            {
                return mLhs;
            }

            /**
             * @return A container containing all variables occurring in the polynomial of this constraint.
             */
            const Variables& variables() const
            {
                return mVariables;
            }

            /**
             * @return The relation symbol of this constraint.
             */
            Relation relation() const
            {
                return mRelation;
            }

            /**
             * @return The unique id of this constraint.
             */
            unsigned id() const
            {
                return mID;
            }

            /**
             * @return A hash value for this constraint.
             */
            size_t getHash() const
            {
                return mHash;
            }

            /**
             * @return true, if the polynomial p compared by this constraint has a proper factorization (!=p);
             *          false, otherwise.
             */
            bool hasFactorization() const
            {
                if( mFactorization.empty() )
                    initFactorization();
                return (mFactorization.size() > 1);
            }

            /**
             * @return The factorization of the polynomial compared by this constraint.
             */
            const Factors<Pol>& factorization() const
            {
                if( mFactorization.empty() )
                    initFactorization();
                return mFactorization;
            }

            /**
             * @return The constant part of the polynomial compared by this constraint.
             */
            typename Pol::NumberType constantPart() const
            {
                if( mLhs.hasConstantTerm() )
                    return mLhs.trailingTerm()->coeff();
                else
                    return typename Pol::NumberType( 0 );
            }

            /**
             * @param _variable The variable for which to determine the maximal degree.
             * @return The maximal degree of the given variable in this constraint. (Monomial-wise)
             */
            unsigned maxDegree( const Variable& _variable ) const
            {
                auto varInfo = mVarInfoMap.find( _variable );
                if( varInfo == mVarInfoMap.end() ) return 0;
                return varInfo->second.maxDegree();
            }

            /**
             * @return The maximal degree of all variables in this constraint. (Monomial-wise)
             */
            unsigned maxDegree() const
            {
                unsigned result = 0;
                for( auto var = mVariables.begin(); var != mVariables.end(); ++var)
                {
                    unsigned deg = maxDegree( *var );
                    if( deg > result )
                        result = deg;
                }
                return result;
            }

            /**
             * @param _variable The variable for which to determine the minimal degree.
             * @return The minimal degree of the given variable in this constraint. (Monomial-wise)
             */
            unsigned minDegree( const Variable& _variable ) const
            {
                auto varInfo = mVarInfoMap.find( _variable );
                if( varInfo == mVarInfoMap.end() ) return 0;
                return varInfo->second.minDegree();
            }
            
            /**
             * @param _variable The variable for which to determine the number of occurrences.
             * @return The number of occurrences of the given variable in this constraint. (In 
             *          how many monomials of the left-hand side does the given variable occur?)
             */
            unsigned occurences( const Variable& _variable ) const
            {
                auto varInfo = mVarInfoMap.find( _variable );
                if( varInfo == mVarInfoMap.end() ) return 0;
                return varInfo->second.occurence();
            }
            
            /**
             * @param _variable The variable to find variable information for.
             * @return The whole variable information object.
             * Note, that if the given variable is not in this constraints, this method fails.
             * Furthermore, the variable information returned do provide coefficients only, if
             * the given flag _withCoefficients is set to true.
             */
            const VarInfo<Pol>& varInfo( const Variable& _variable, bool _withCoefficients = false ) const
            {
                auto varInfo = mVarInfoMap.find( _variable );
                assert( varInfo != mVarInfoMap.end() );
                if( _withCoefficients && !varInfo->second.hasCoeff() )
                {
                    // @todo Why does this not compile??
//                    varInfo->second = mLhs.getVarInfo<true>( _variable );
                }
                return varInfo->second;
            }

            /**
             * @param _rel The relation to check whether it is strict.
             * @return true, if the given relation is strict;
             *          false, otherwise.
             */
            static bool constraintRelationIsStrict( Relation _rel )
            {
                return (_rel == Relation::NEQ || _rel == Relation::LESS || _rel == Relation::GREATER);
            }
            
            /**
             * Checks if the given variable occurs in the constraint.
             * @param _var  The variable to check for.
             * @return true, if the given variable occurs in the constraint;
             *          false, otherwise.
             */
            bool hasVariable( const Variable& _var ) const
            {
                return mVariables.find( _var ) != mVariables.end();
            }
            
            /**
             * @return true, if it contains only integer valued variables.
             */
            bool integerValued() const
            {
                for( auto var = mVariables.begin(); var != mVariables.end(); ++var )
                {
                    if( var->getType() != VariableType::VT_INT )
                        return false;
                }
                return true;
            }
            
            /**
             * @return true, if it contains only real valued variables.
             */
            bool realValued() const
            {
                for( auto var = mVariables.begin(); var != mVariables.end(); ++var )
                {
                    if( var->getType() != VariableType::VT_REAL )
                        return false;
                }
                return true;
            }
            
            /**
             * Checks if this constraints contains an integer valued variable.
             * @return true, if it does;
             *          false, otherwise.
             */
            bool hasIntegerValuedVariable() const
            {
                for( auto var = mVariables.begin(); var != mVariables.end(); ++var )
                {
                    if( var->getType() == VariableType::VT_INT )
                        return true;
                }
                return false;
            }
            
            /**
             * Checks if this constraints contains an real valued variable.
             * @return true, if it does;
             *          false, otherwise.
             */
            bool hasRealValuedVariable() const
            {
                for( auto var = mVariables.begin(); var != mVariables.end(); ++var )
                {
                    if( var->getType() == VariableType::VT_REAL )
                        return true;
                }
                return false;
            }
            
            /**
             * @return true, if this constraint is a bound.
             */
            bool isBound() const
            {
                return ( mRelation != Relation::NEQ && mVariables.size() == 1 && maxDegree( *mVariables.begin() ) == 1 );
            }
            
            /**
             * @return true, if this constraint is a lower bound.
             */
            bool isLowerBound() const
            {
                if( isBound() )
                {
                    if( mRelation == Relation::EQ ) return true;
                    const typename Pol::NumberType& coeff = mLhs.lterm()->coeff();
                    if( coeff < 0 )
                        return (mRelation == Relation::LEQ || mRelation == Relation::LESS );
                    else
                    {
                        assert( coeff > 0 );
                        return (mRelation == Relation::GEQ || mRelation == Relation::GREATER );
                    }
                }
                return false;
            }
            
            /**
             * @return true, if this constraint is an upper bound.
             */
            bool isUpperBound() const
            {
                if( isBound() )
                {
                    if( mRelation == Relation::EQ ) return true;
                    const typename Pol::NumberType& coeff = mLhs.lterm()->coeff();
                    if( coeff > 0 )
                        return (mRelation == Relation::LEQ || mRelation == Relation::LESS );
                    else
                    {
                        assert( coeff < 0 );
                        return (mRelation == Relation::GEQ || mRelation == Relation::GREATER );
                    }
                }
                return false;
            }
            
            /**
             * Checks whether the given assignment satisfies this constraint.
             * @param _assignment The assignment.
             * @return 1, if the given assignment satisfies this constraint.
             *          0, if the given assignment contradicts this constraint.
             *          2, otherwise (possibly not defined for all variables in the constraint,
             *                       even then it could be possible to obtain the first two results.)
             */
            unsigned satisfiedBy( const EvaluationMap<typename Pol::NumberType>& _assignment ) const;
            
            /**
             * Checks, whether the constraint is consistent.
             * It differs between, containing variables, consistent, and inconsistent.
             * @return 0, if the constraint is not consistent.
             *          1, if the constraint is consistent.
             *          2, if the constraint still contains variables.
             */
            unsigned isConsistent() const;
            
            /**
             * Checks whether this constraint is consistent with the given assignment from 
             * the its variables to interval domains.
             * @param _solutionInterval The interval domains of the variables.
             * @return 1, if this constraint is consistent with the given intervals;
             *          0, if this constraint is not consistent with the given intervals;
             *          2, if it cannot be decided whether this constraint is consistent with the given intervals.
             */
            unsigned consistentWith( const EvaluationMap<Interval<double>>& _solutionInterval ) const;
            
            /**
             * @param _var The variable to check the size of its solution set for.
             * @return true, if it is easy to decide whether this constraint has a finite solution set
             *                in the given variable;
             *          false, otherwise.
             */
            bool hasFinitelyManySolutionsIn( const Variable& _var ) const;
            
            /**
             * Compares this constraint with the given constraint.
             * @return true,   if this constraint is LEXOGRAPHICALLY smaller than the given one;
             *          false,  otherwise.
             */
            bool operator<( const Constraint& _constraint ) const;
            
            /**
             * Compares this constraint with the given constraint.
             * @return  true,   if this constraint is equal to the given one;
             *          false,  otherwise.
             */
            bool operator==( const Constraint& _constraint ) const;
            
            /**
             * Prints the representation of the given constraints on the given stream.
             * @param _ostream The stream to print on.
             * @param _constraint The constraint to print.
             * @return The given stream after printing.
             */
            template<typename P>
            friend std::ostream& operator<<( std::ostream& _out, const Constraint<P>& _constraint );
            
            /**
             * Calculates the coefficient of the given variable with the given degree. Note, that it only
             * computes the coefficient once and stores the result.
             * @param _variable The variable for which to calculate the coefficient.
             * @param _degree The according degree of the variable for which to calculate the coefficient.
             * @return The ith coefficient of the given variable, where i is the given degree.
             */
            Pol coefficient( const Variable& _var, unsigned _degree ) const;
            
            /**
             * If this constraint represents a substitution (equation, where at least one variable occurs only linearly),
             * this method detects a (there could be various possibilities) corresponding substitution variable and term.
             * @param _substitutionVariable Is set to the substitution variable, if this constraint represents a substitution.
             * @param _substitutionTerm Is set to the substitution term, if this constraint represents a substitution.
             * @return true, if this constraints represents a substitution;
             *         false, otherwise.
             */
            bool getSubstitution( Variable& _substitutionVariable, Pol& _substitutionTerm ) const;
               
            /**
             * Applies some cheap simplifications to the constraints.
             *
             * @return The simplified constraints, if simplifications could be applied;
             *         The constraint itself, otherwise.
             */
            Constraint* simplify() const;
            
            /**
             * Initializes the stored factorization.
             */
            void initFactorization() const;
            
            /**
             * Gives the string representation of this constraint.
             * @param _unequalSwitch A switch to indicate which kind of unequal should be used.
             *         For p != 0 with infix:  0: "p != 0", 1: "p <> 0", 2: "p /= 0"
             *                    with prefix: 0: "(!= p 0)", 1: (or (< p 0) (> p 0)), 2: (not (= p 0))
             * @param _infix An infix string which is printed right before the constraint.
             * @param _friendlyVarNames A flag that indicates whether to print the variables with their internal representation (false)
             *                           or with their dedicated names.
             * @return The string representation of this constraint.
             */
            std::string toString( unsigned _unequalSwitch = 0, bool _infix = true, bool _friendlyVarNames = true ) const;
            
            /**
             * Prints the properties of this constraints on the given stream.
             * @param _out The stream to print on.
             * @param _friendlyVarNames A flag that indicates whether to print the variables with their internal representation (false)
             *                           or with their dedicated names.
             */
            void printProperties( std::ostream& _out = std::cout ) const;
            
            /**
             * Gives the string to the given relation symbol.
             * @param _rel The relation symbol.
             * @return The resulting string.
             */
            static std::string relationToString( const Relation _rel );
            
            /**
             * Checks whether the given value satisfies the given relation to zero.
             * @param _value The value to compare with zero.
             * @param _relation The relation between the given value and zero.
             * @return true,  if the given value satisfies the given relation to zero;
             *          false, otherwise.
             */
            static bool evaluate( const typename Pol::NumberType& _value, Relation _rel );
            
            /**
             * Inverts the given relation symbol.
             * @param _rel The relation symbol to invert.
             * @return The resulting inverted relation symbol.
             */
            static Relation invertRelation( const Relation _rel );
            
            /**
             * Turns around the given relation symbol.
             * @param _rel The relation symbol to invert.
             * @return The resulting inverted relation symbol.
             */
            static Relation turnAroundRelation( const Relation _rel );
            
            /**
             * Compares _constraintA with _constraintB.
             * @return  2, if it is easy to decide that _constraintA and _constraintB have the same solutions. _constraintA = _constraintB
             *           1, if it is easy to decide that _constraintB includes all solutions of _constraintA;   _constraintA -> _constraintB
             *          -1, if it is easy to decide that _constraintA includes all solutions of _constraintB;   _constraintB -> _constraintA
             *          -2, if it is easy to decide that _constraintA has no solution common with _constraintB; not(_constraintA and _constraintB)
             *          -3, if it is easy to decide that _constraintA and _constraintB can be intersected;      _constraintA and _constraintB = _constraintC
             *          -4, if it is easy to decide that _constraintA is the inverse of _constraintB;           _constraintA xor _constraintB
             *           0, otherwise.
             */
            static signed compare( const Constraint* _constraintA, const Constraint* _constraintB );
    };
}    // namespace carl

#define CONSTRAINT_HASH( _lhs, _rel, _type ) (size_t)((size_t)(std::hash<_type>()( _lhs ) << 3) ^ (size_t)_rel)

namespace std
{
    /**
     * Implements std::hash for constraints.
     */
    template<typename Pol>
    struct hash<carl::Constraint<Pol>>
    {
    public:
        /**
         * @param _constraint The constraint to get the hash for.
         * @return The hash of the given constraint.
         */
        size_t operator()( const carl::Constraint<Pol>& _constraint ) const 
        {
            return _constraint.getHash();
        }
    };
    
    /**
     * Implements std::hash for vectors of constraints.
     */
    template<typename Pol>
    struct hash<std::vector<const carl::Constraint<Pol>* >>
    {
    public:
        /**
         * @param _arg The vector of constraints to get the hash for.
         * @return The hash of the given vector of constraints.
         */
        size_t operator()( const std::vector<const carl::Constraint<Pol>*>& _arg ) const
        {
            size_t result = 0;
            for( auto cons = _arg.begin(); cons != _arg.end(); ++cons )
            {
                result <<= 5;
                result ^= (*cons)->id();
            }
            return result;
        }
    };
} // namespace std

#include "Constraint.tpp"