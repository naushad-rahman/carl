#pragma once

#include "../../core/Variable.h"
#include "../bitvector/BVVariable.h"
#include "../uninterpreted/UninterpretedFunction.h"
#include "../uninterpreted/UVariable.h"
#include "../../util/variant_util.h"

#include <boost/variant.hpp>

namespace carl
{

	class ModelVariable: private boost::variant<Variable,BVVariable,UVariable,UninterpretedFunction> {
        /**
         * Base type we are deriving from.
         */
        using Super = boost::variant<Variable,BVVariable,UVariable,UninterpretedFunction>;
        
    public:
		friend bool operator==(const ModelVariable& lhs, const ModelVariable& rhs);
        /**
         * Default constructor.
         */
        ModelVariable(): Super()
        {}

        /**
         * Initializes the ModelVariable from some valid type of the underlying variant.
         */
        template<typename T>
        ModelVariable(const T& _t): Super(_t)
        {}

        /**
         * Assign some value to the underlying variant.
         * @param t Some value.
         * @return *this.
         */
        template<typename T>
        ModelVariable& operator=(const T& _t) {
            Super::operator=(_t);
            return *this;
        }
		
		std::size_t hash() const {
			return carl::variant_hash(static_cast<const Super&>(*this));
		}
        
        /**
         * @return true, if the stored value is a variable.
         */
        bool isVariable() const
        {
            return type() == typeid(carl::Variable);
        }
        
        /**
         * @return true, if the stored value is a bitvector variable.
         */
        bool isBVVariable() const
        {
            return type() == typeid(carl::BVVariable);
        }

        /**
         * @return true, if the stored value is an uninterpreted variable.
         */
        bool isUVariable() const
        {
            return type() == typeid(carl::UVariable);
        }

        /**
         * @return true, if the stored value is a function.
         */
        bool isFunction() const
        {
            return type() == typeid(carl::UninterpretedFunction);
        }
        
        /**
         * @return The stored value as a variable.
         */
        carl::Variable::Arg asVariable() const
        {
            assert( isVariable() );
            return boost::get<carl::Variable>(*this);
        }
        
        /**
         * @return The stored value as a bitvector variable.
         */
        const carl::BVVariable& asBVVariable() const
        {
            assert( isBVVariable() );
            return boost::get<carl::BVVariable>(*this);
        }

        /**
         * @return The stored value as an uninterpreted variable.
         */
        const carl::UVariable& asUVariable() const
        {
            assert( isUVariable() );
            return boost::get<carl::UVariable>(*this);
        }
        
        /**
         * @return The stored value as a function.
         */
        const carl::UninterpretedFunction& asFunction() const
        {
            assert( isFunction() );
            return boost::get<carl::UninterpretedFunction>(*this);
        }
    };
	
	

	/**
	 * @return true, if the first and the second are either both variables or both functions 
	 *               and in the first case the variables are equal (equal ids)
	 *                or in the second case the functions are equal (equal ids).
	 */
	inline bool operator==(const ModelVariable& lhs, const ModelVariable& rhs) {
		if (lhs.which() != rhs.which()) {
			return false;
		} else if (lhs.isVariable()) {
			return lhs.asVariable() == rhs.asVariable();
		} else if (lhs.isBVVariable()) {
			return lhs.asBVVariable() == rhs.asBVVariable();
		} else if (lhs.isUVariable()) {
			return lhs.asUVariable() == rhs.asUVariable();
		} else {
			assert(lhs.isFunction());
			return lhs.asFunction() == rhs.asFunction();
		}
	}
	
	/**
	 * @return true, if the first argument is a variable and the second is a function 
	 *                or if both are variables and the first is smaller (lower id)
	 *                or if both are function and the first smaller (lower id).
	 */
	inline bool operator<(const ModelVariable& lhs, const ModelVariable& rhs) {
		if (lhs.isVariable()) {
			if (rhs.isVariable()) return lhs.asVariable() < rhs.asVariable();
			return true;
		} else if (lhs.isBVVariable()) {
			if (rhs.isBVVariable()) return lhs.asBVVariable() < rhs.asBVVariable();
			return !rhs.isVariable();
		} else if (lhs.isUVariable()) {
			if (rhs.isUVariable()) return lhs.asUVariable() < rhs.asUVariable();
			return rhs.isFunction();
		} else if (lhs.isFunction()) {
			if (rhs.isFunction()) return lhs.asFunction() < rhs.asFunction();
			return false;
		} else {
			assert(false);
			return false;
		}
	}
	
	inline std::ostream& operator<<(std::ostream& os, const ModelVariable& mv) {
		if (mv.isVariable()) {
			return os << mv.asVariable();
		} else if (mv.isBVVariable()) {
			return os << mv.asBVVariable();
		} else if (mv.isUVariable()) {
			return os << mv.asUVariable();
		} else {
			assert(mv.isFunction());
			return os << mv.asFunction();
		}
	}
}

namespace std {
	template<>
	struct hash<carl::ModelVariable>: boost::static_visitor<std::size_t> {
		std::size_t operator()(const carl::ModelVariable& mv) const {
			return mv.hash();
		}
	};
}
