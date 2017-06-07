#include "query_b.hpp"

bool trans_qb(string q, int maxar, string& res)
{
	Literal qb_lit;
	Literal t_lit;
	logic_grammar program;
	tree_parse_info<> info = ast_parse(q.c_str(), program, space_p);
	if(info.full)
	{
		tree_it tree_begin = info.trees.begin();
		if(!parse_literal(qb_lit, tree_begin, RULE))
		{
			cerr << "Error parsing query: "<<q<<"."<<endl;
			return false;
		}
		else
		{
			t_lit.set_name(m_ANS);	
			map_t cons_map = qb_lit.get_const();
			map_t var_map = qb_lit.get_var();
			map_it it;
			string pred_name = ""; 
			if(!qb_lit.sign())
				pred_name += m_NEG;
			pred_name += qb_lit.name();
			t_lit.add_const(0, pred_name);
			for(it=cons_map.begin(); it!=cons_map.end(); ++it)
			{
				t_lit.add_const(it->first + 1, it->second);
			}
			for(it=var_map.begin(); it!=var_map.end(); ++it)
			{
				t_lit.add_var(it->first + 1, it->second);
			}
			while(t_lit.arity()<maxar+1) /* padding with newconst */
			{
				
				t_lit.add_const(t_lit.arity(), m_NEWCONST);
			}
			if(t_lit.arity()>maxar+1) 
				/* If query literal has more arity then the max arity of the program,
				then it is not a consequence of the program */
				res = "\tfalse :- not false.";
			else 
				res =  "\t:- not " + t_lit.str() + ".";
			return true;
		}
	}
	return false;
}

