/******************************************************************
** TRANSFORM.hpp : transformation functions/classes declaration	**
*******************************************************************/

#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include <cmath>

#define m_ANS		"m_ANS"
#define m_NEWPRED	"m_NEWPRED"
#define m_NEWCONST	"m_NEWCONST"
#define m_COMPL	"m_COMPL"
#define m_NEG		"m_NEG_"
#define m_VARPREF	"m_VARPREF_"
#define m_TABH		"m_TABH"
#define m_TABPB	"m_TABPB"
#define m_TABNB 	"m_TABNB"
#define m_RULE		"rule"
#define m_FACT		"fact"
#define m_CONST	"const"
#define m_RULEVAR	"rulevar"
#define m_LABEL	"label"
#define m_MAP		"map"
#define m_MID		"m_id"
#define m_HEAD		"head"
#define m_PBL		"pbl"
#define m_NBL		"nbl"
#define m_SEL		"sel"
#define m_VAL		"val"
#define VAL		"VAL_"

#include "program_rep.hpp"

//Program transform(Program);
typedef multimap<int, string> argmap_t;
typedef multimap<int, string>::iterator argmap_it;
typedef multimap<int, string>::const_iterator argmap_constit;


class Program_Dom
{
	private:
		int num_clause;
		/* 1-nov */
		vector<int> facts;
		vector<int> rules;
		/*............*/
		
		argmap_t var_map;
		stringset_t constants;
		int label;
		int arity;
	public:
		Program_Dom(int);
		Program_Dom(const Program&);
		void set_num_clause(int );
		int get_num_clause() const;
		
		int get_arity()const;
		stringset_t get_constants()const;
		argmap_t get_var_map()const;
		//void set_arity(int);
		Program_Dom& operator=(const Program_Dom&);
		void add_var_map(int, string);
		void add_const(string);
		string write(bool) const;
		
};

Program gen_map(const Program&);

Program gen_table(const Program&, stringset_t);

void gen_table_fact(const Fact&, Fact*, int id);

Litset_t gen_table_rule(const Rule&, int);

Program gen_input(const Program&, bool);
LineVec_t gen_input_rule(const Rule&, int, bool);
LineVec_t gen_input_fact(const Fact&, int);

vector<int> var_rep(int, int, long int);

string write_meta(int);
string write_sel();

#define META  "in_AS(X) :-	head(X,R),pos_body_true(R),not neg_body_false(R). \n \
			pos_body_true(R) :- fact(R). \n \
			pos_body_exist(R) :- pbl(_,R).	\n \
			pos_body_true(R) :-	label(R),not pos_body_exist(R). \n \
			pbl_inbetween(X,Y,R) :-	pbl(X,R), pbl(Y,R), pbl(Z,R), X<Z, Z<Y. \n \
			pbl_notmax(X,R) :-	pbl(X,R), pbl(Y,R), X<Y.\n \
			pbl_notmin(X,R) :-	pbl(X,R), pbl(Y,R), Y<X.\n \
			pos_body_true_upto(R,X) :-	pbl(X,R), not pbl_notmin(X,R), in_AS(X).\n \
			pos_body_true_upto(R,X) :-	pos_body_true_upto(R,Y),pbl(X,R),in_AS(X), \n \
			Y<X, not pbl_inbetween(Y,X,R). \n \
			pos_body_true(R)	:-	pos_body_true_upto(R,X), not pbl_notmax(X,R).\n \
			neg_body_false(R) 	:-	nbl(X,R), in_AS(X).\n \
			:-	compl(X,Y),in_AS(X), in_AS(Y).\n"


#define SEL_VAL    "sel(L, R) v nsel(L, R)	:-	label(L),rule(R).\n \
				:-	sel(L,R1),sel(L,R2),R1<>R2.\n \
				% val(L,V,C) :-	sel(L,R),rulevar(R,V), const(C), not nval(L,V,C).\n \
				val(L,V,C) v nval(L,V,C) :-	sel(L,R),rulevar(R,V), const(C).\n \
				nval(L,V,C1) v nval(L,V,C2) :-	sel(L,R),rulevar(R,V), const(C1),const(C2),C1<>C2.\n \
				valued(L,V)	:-	val(L,V,_). \n \
				:-	label(L), rule(R), sel(L,R), rulevar(R,V), not valued(L,V). \n \
				used(L) :- label(L), sel(L,_). \n \
				%% This is for preventing different ordering of labels for the same rule \n \
				:- label(L1), label(L2), used(L1), not used(L2), L2<L1. \n  \
				%% These clauses are for preventing using diff labels for the same grounding \n \
				rulevar_inbetween(R, V1, V2) :- rulevar(R,V1), rulevar(R,V2), rulevar(R,V3), V1<V3, V3<V2. \n \
				rulevar_notmax(R, V) :- rulevar(R,V), rulevar(R,V1), V<V1. \n \
				rulevar_notmin(R, V) :- rulevar(R,V), rulevar(R,V1), V>V1. \n \
				same_grounding_upto(R,L1,L2,V) :- 	rule(R), sel(L1,R),sel(L2,R), L1<>L2, \n\
											rulevar(R,V), \n \
								    			val(L1,V,C), val(L2, V, C), \n \
										    	not rulevar_notmin(R,V). \n \
				same_grounding_upto(R,L1,L2,V) :- 	same_grounding_upto(R,L1,L2,V1), \n \
											rulevar(R,V), val(L1,V,C), val(L2,V,C), \n \
											V1<V, not rulevar_inbetween(R,V1,V). \n \
				same_grounding(R,L1,L2) :- same_grounding_upto(R,L1,L2,V), not rulevar_notmax(R,V). \n \
				rule_with_var(R) :-	rule(R), rulevar(R,_). \n \
				same_grounding(R,L1,L2) :- rule(R), sel(L1,R),sel(L2,R),L1<>L2, \n\
									not rule_with_var(R). \n \
				:- rule(R), label(L1), label(L2), same_grounding(R,L1,L2). \n \
				%% These clauses are for preventing permutation between labels \n \
				%% for the same set of ground rules \n \
				smaller_grounding_upto(R,L1,L2,V) :- 	rule(R), sel(L1,R),sel(L2,R), L1<>L2, \n \
												rulevar(R,V), \n \
						   					val(L1,V,C1), val(L2, V, C2), \n \
							  				C1<C2, not rulevar_notmin(R,V). \n \
				smaller_grounding_upto(R,L1,L2,V) :- 	same_grounding_upto(R,L1,L2,V1),\n \
											rulevar(R,V), val(L1,V,C1), val(L2,V,C2), \n \
											V1<V, C1<C2, not rulevar_inbetween(R,V1,V). \n \
				smaller(L1,L2) :- sel(L1,R1), sel(L2,R2), R1<R2. \n \
				smaller(L1,L2) :- sel(L1,R), sel(L2,R), \n \
						rulevar(R,V), smaller_grounding_upto(R,L1,L2,V). \n \
				:- label(L1), label(L2), smaller(L1,L2), L1>L2. \n "				


#endif

/* END OF TRANSFORM.HPP */
