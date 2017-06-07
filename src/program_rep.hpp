/****************************************************************************
**	PROGRAM_REP.HPP : Declaration for program representation classes 	**
*****************************************************************************/
#ifndef PROGRAM_REP
#define PROGRAM_REP
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <boost/spirit/core.hpp>
#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/tree/tree_to_xml.hpp>
#include <boost/algorithm/string.hpp>
#include "logic_grammar.hpp"
#define m_FALSE "m_FALSE"

using namespace std;
using namespace boost::spirit;
using namespace boost::algorithm;

/* some typedefs constant and vars map/set data structures */

typedef map<int, string> map_t;
typedef map<int, string>::iterator map_it;
typedef map<int, string>::const_iterator map_constit;
typedef map<int, string>::reverse_iterator map_revit;
typedef set<string> stringset_t;
typedef set<string>::iterator stringset_it;
typedef vector<string> vecstring_t;
typedef vector<string>::iterator vecstring_it;
typedef enum {FACT, RULE} line_t;

/* typedefs for parse_tree info/nodes */

typedef char const* char_t;
typedef tree_match<char_t> parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator tree_it;

/*	Class Literal represents one literal in a fact/rule. 
	For a literal (+/-)pred(A1, A2... An) we store the sign,
	predicate name, and maps of the constants and variables
	in the argument. For a constant C in position i, we store
	the value pair<int, string>(i,"C") in constant_map member.
	Similarly for variables.
*/
	
class Literal
{
	protected:
		bool positive;	
		string pred_name;
		int pred_arity;
		map_t constant_map;
		map_t var_map;
	
	public:
		Literal();
		Literal(const string&);
		Literal (const Literal& );	// copy constructor, for passing by-value
		void set_name(string);
		string name() const;
		int arity() const;
		void set_sign(bool);
		bool sign() const;
		void add_const(int, const string&);
		void add_var(int, const string&);
		void set_const(map_t);
		void set_var(map_t);
		map_t get_const() const;	//return only constants
		map_t get_var() const;		//return only vars
		map_t params() const;		// params() returns map of ALL parameters: constant and vars
		Literal& operator = (const Literal&);	//assignment operator, for assignment in the rule head struct
		// These operators are needed for set<Literal>
		friend bool operator < (const Literal& , const Literal&);
		friend bool operator == (const Literal& , const Literal&);
		friend bool operator != (const Literal& , const Literal&);
		
		string str() const;		// return a std::string representation of the literal: "[-]pred(A1....An)"
		void transform_uni_pred(int);
};

/*	A head is represented as a struct. 
	hasLit signals whether the head is empty/not 

*/

struct Head_struct
{
	bool hasLit;
	Literal lit;
};

typedef Head_struct Head;


/* 	class program_line is the abstract class to represent one line of program
	it can be a fact, or a rule.
*/

typedef set<Literal> Litset_t;
typedef set<Literal>::iterator Litset_it;
typedef set<Literal>::const_iterator Litset_constit;

class Program_Line	
{
	protected:
	int ID;
	
	public:
	int getID() const
	{	
		return ID;
	}
	virtual bool uniform_arity() const=0;
	virtual int maxarity() const = 0;
	virtual line_t type() const = 0;
	virtual stringset_t get_preds() const = 0;
	virtual stringset_t get_negpreds() const = 0;
	virtual stringset_t get_consts() const = 0;
	//virtual stringset_t get_vars() const = 0;
	virtual Literal get_lit() const = 0;
	virtual Head get_head() const = 0;
	virtual Litset_t get_pbl() const = 0;
	virtual Litset_t get_nbl() const = 0;
	virtual string str() const = 0;
};

/* Fact class definition */
class Fact: public Program_Line
{
	private :
		Literal lit;
	
	public :
		Fact();
		Fact(int);
		line_t type() const;
		int maxarity() const;
		void set_lit(const Literal&);
		Literal get_lit() const;
		stringset_t get_preds() const;
		stringset_t get_negpreds() const;
		stringset_t get_consts() const;
		//stringset_t var_sets() const;
		string str() const;
		void transform_uni_pred(int);
		bool uniform_arity() const;
		Litset_t get_pbl() const {};
		Litset_t get_nbl() const {};
		Head get_head() const {};
		friend bool operator < (const Fact& , const Fact&);
		friend bool operator == (const Fact& , const Fact&);
		friend bool operator != (const Fact& , const Fact&);

};





/* 	One rule class definition
	Each rule contains head member, pbl
	and nbl. 
*/

class Rule: public Program_Line
{	
	private :
		Head head;
		Litset_t pbl;
		Litset_t nbl;

	public:
		Rule(int);
		line_t type() const;
		int maxarity() const;
		void set_head(const Head&);
		void add_pbl(const Literal&);
		void add_nbl(const Literal&);
		Head get_head() const;
		Litset_t get_pbl() const;
		Litset_t get_nbl() const;
		stringset_t get_preds() const;
		stringset_t get_negpreds() const;
		stringset_t get_consts() const;
		stringset_t get_vars() const;	//only for Rule
		string str() const;
		void normalize_hc();
		void transform_uni_pred(int);
		bool uniform_arity() const;
		
		Literal get_lit() const {};
};




typedef vector<Program_Line*> LineVec_t;
typedef vector<Program_Line*>::iterator LineVec_it;
typedef vector<Program_Line*>::const_iterator LineVec_constit;

class Program
{
	protected:
		LineVec_t lines;
		int facts;
		int rules;
	
	public:
		Program();
		Program(const Program&);
		~Program();
		void add_line(Program_Line*);
		bool parse_line(int, tree_it const&);
		stringset_t get_preds() const;
		stringset_t get_negpreds() const;
		stringset_t get_consts() const;
		LineVec_t get_lines() const;
		
		int maxarity() const;
		string write(bool) const;
		void normalize_hc();
		void transform_uni_pred();
		bool uniform_arity() const;
		
};

/* Tree parsing and program representation */

bool parse_fact(Fact* , tree_it const& );
bool parse_rule(Rule* , tree_it const& );
bool parse_literal(Literal& , tree_it const&, line_t );
bool parse_body(Rule* , tree_it const& );


#endif

/* End of Program_Rep.hpp declarations */
