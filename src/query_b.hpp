#ifndef QUERY_HPP
#define QUERY_HPP
#include <boost/spirit/core.hpp>
#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/tree/tree_to_xml.hpp>

#include <fstream>
#include <cstdlib>
#include <string>

#include "logic_grammar.hpp"
#include "program_rep.hpp"
#include "transform.hpp"


bool trans_qb(string, int, string& );

#endif
