#ifndef AS_TRANS_HPP
#define AS_TRANS_HPP

#include <boost/spirit/core.hpp>
#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/tree/tree_to_xml.hpp>
//#include "as_grammar.hpp"
#include "as_rep.hpp"
#include "transform.hpp"

bool as_trans(AS*, AS*);
bool lit_trans(const Literal&, Literal&);
bool trans_answerset(string, string&);
		
#endif
