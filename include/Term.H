#ifndef TERM_H
#define TERM_H
#include <string.h>
#include "TMonoid.H"
#include "TOrdering.H"
#include <iostream>

using namespace std;

class Polynomial;

class Term {
	public: 
		TMonoid* const owner;
	protected:
		degreeType degree;
		size_t hash;
		degreeType* indets;

		Term(TMonoid* const m) : owner(m) {
			indets = (degreeType*)malloc(owner->N*sizeof(degreeType));
		}
	
		Term(TMonoid* const m, const degreeType* i) : owner(m)  {
			indets = (degreeType*)malloc(owner->N*sizeof(degreeType));
			memcpy(indets, i, owner->N*sizeof(degreeType));
			setHash();
			setDegree();
		}

		// Multiplication constructor
		Term(TMonoid* const m, const degreeType* a, const degreeType* b) : owner(m) {
			indets = (degreeType*)malloc(owner->N*sizeof(degreeType));
			for(size_t i = 0; i < owner->N; i++) {
				indets[i] = a[i] + b[i];
			}
			setHash();
			setDegree();
		}

		~Term() {
			free(indets);
		}

		void setDegree() {
			degree = indets[0];
			for(size_t k = 1; k < owner->N; k++) {
				degree += indets[k];
			}
		}		
		void setHash() {
		        hash = indets[0];
		        for(size_t i = 1; i < owner->N; i++)
		        {
				// Problem: Large degree of x[1] gets 'cut' ?
                		hash <<= owner->D;
	        	        hash += indets[i];
        		}
		}

		void set(size_t i, degreeType value) const {
			this->indets[i] = value;
		}

	public:
		degreeType operator[](size_t i) const {
			return indets[i];
		}

		degreeType at(size_t i) const {
			return indets[i];
		}

		size_t size() const {
			return owner->N;
		}

		const TMonoid* monoid() const {
			return owner;
		}

		degreeType deg() const {
			return degree;
		}

		bool isDivisibleBy(const Term* other) const;

		const Term* mul(const Term* other) const {
			if(other->degree == 0) { return this; }
			return owner->createElement(new Term(owner, indets, other->indets));
		}

		const Term* lcm(const Term* other) const;

		const Term* div(const Term* other) const {
			Term* result = new Term(owner);
			for(size_t i = 0; i < owner->N; i++) {
				result->indets[i] = indets[i] - other->indets[i];
			}		
			result->setDegree();
			result->setHash();	
			return owner->createElement(result);
		}

		vector<long> getValues() const {
			vector<long> v;
			for(size_t i = 0; i < owner->N; i++) {
				v.push_back( indets[i] );
			}
			return v;
		}

		bool equal(const Term* other) const {
			for(size_t i = 0; i < owner->N; i++) {
				if(other->indets[i] != indets[i]) return false;
			}
			return true;
		}

		// Multiplies a set of terms with the current term
		// Uses OpenMP-Parallelization
		vector<const Term*> mulAll(Polynomial& in, int threads, double& timer) const;

		friend class TMonoid;
};

class TermComparator {
	public:
		const TOrdering* O;
		bool gt; // if greater

		TermComparator(const TOrdering* O, bool gt = false) : O(O), gt(gt) {}

		bool operator() (const Term* const& lhs, const Term* const& rhs) const
		{
			return gt ? O->cmp(rhs, lhs) < 0 : O->cmp(lhs, rhs) < 0;
		}
};


ostream& operator<< (ostream &out, const Term &term);
#endif