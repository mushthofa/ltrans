#include <string>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/find.hpp>
#include <cstdio>
#include "as_rep.hpp"
#include "program_rep.hpp"
#include "transform.hpp"

#define m_PREV "previous_"
bool as_check(string, Program, string);
string first_line_pbtrue(int);
#define CHECK_SEL_VAL \
"	sel(R) v nsel(R) :- rule(R).\n\
	:- sel(R1), sel(R2), R1<>R2.\n\
	rulevar_inbetween(R, V1, V2) :- rulevar(R,V1), rulevar(R,V2), rulevar(R,V3), V1<V3, V3<V2. \n \
	rulevar_notmax(R, V) :- rulevar(R,V), rulevar(R,V1), V<V1. \n\
	rulevar_notmin(R, V) :- rulevar(R,V), rulevar(R,V1), V>V1. \n\
	val(R,V,C) v nval(R,V,C) :- sel(R), rulevar(R,V), const(C).\n\
	same_grounding_upto(R,L,X) :- sel(R),previous_sel(L,R), \n\
				val(R,X,C), previous_val(L,X,C), not rulevar_notmin(R,X).\n\
	same_grounding_upto(R,L,X) :- same_grounding_upto(R,L,Y), val(R,X,C), previous_val(L,X,C),\n\
							not rulevar_inbetween(R,X,Y).\n\
	same_grounding(R,L) :- same_grounding_upto(R,L,X), not rulevar_notmax(R,X).\n\
	:- rule(R),sel(R), previous_sel(L,R), same_grounding(R,L).\n\
  	nval(R,V,C)  :-	sel(R),rulevar(R,V), const(C), not val(R,V,C).\n\
	nval(R,V,C1) v nval(R,V,C2) :-	sel(R),rulevar(R,V), const(C1),const(C2),C1<>C2.\n\
 	valued(R,V)	:-	val(R,V,_). \n\
	:- rule(R), sel(R), rulevar(R,V), not valued(R,V).\n"


#define CHECK_CONST \
	"pos_body_true 	:- not pos_body_exist. \n \
	pbl_inbetween(X,Y) :-	pbl(X), pbl(Y), pbl(Z), X<Z, Z<Y. \n \
	pbl_notmax(X) :-	pbl(X), pbl(Y), X<Y.\n \
	pbl_notmin(X) :-	pbl(X), pbl(Y), Y<X.\n \
	pos_body_true_upto(X) :-	pbl(X), not pbl_notmin(X), in_AS(X).\n \
	pos_body_true_upto(X) :-	pos_body_true_upto(Y),pbl(X),in_AS(X), \n \
		Y<X, not pbl_inbetween(Y,X). \n \
	pos_body_true	:-	pos_body_true_upto(X), not pbl_notmax(X).\n \
   	neg_body_false 	:-	nbl(X), in_AS(X).\n \
   	violated :- pos_body_true, not neg_body_false, head(X), not in_AS(X).\n\
 	violated?\n"
