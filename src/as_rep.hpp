#ifndef AS_REP_HPP
#define AS_REP_HPP
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <boost/spirit/core.hpp>
#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/tree/tree_to_xml.hpp>
#include <boost/algorithm/string.hpp>
#include "as_grammar.hpp"
#include "program_rep.hpp"
#include "transform.hpp"

using namespace std;
using namespace boost::spirit;
using namespace boost::algorithm;

/* some typedefs constant and vars map/set data structures */

typedef map<int, string> map_t;
typedef map<int, string>::iterator map_it;
typedef map<int, string>::const_iterator map_constit;
typedef map<int, string>::reverse_iterator map_revit;
typedef char const* char_t;
typedef tree_match<char_t> parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator tree_it;

typedef vector<Literal> VecLit_t;
typedef vector<Literal>::iterator VecLit_it;
typedef vector<Literal>::const_iterator VecLit_constit;

class AS
{
	private:
		VecLit_t answers;
		int size;
	public:
		AS();
		VecLit_t get_answers() const;
		void add_answer(const Literal& lt);
		string str() const;
		string str_as() const;
		void sort();
};

bool parse_AS(AS*, const tree_it&);
bool parse_literal(Literal&, tree_it const&);

#endif
