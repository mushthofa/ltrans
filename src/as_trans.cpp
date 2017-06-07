#include "as_trans.hpp"
#include "as_grammar.hpp"

bool lit_trans(const Literal& old, Literal& n)
{
	map_t oldconst = old.get_const();
	string pred_name = oldconst[0];
	/* CHECK whether it has m_NEG as substring */
	if(pred_name.compare(0,strlen(m_NEG),m_NEG)==0)
	{
		pred_name.erase(0,strlen(m_NEG));
		n.set_sign(false);
	}
	n.set_name(pred_name);
	int i;
	for(i=1; i<oldconst.size(); i++)
	{
		if(oldconst[i].compare(m_NEWCONST)!=0)
			n.add_const(i-1, oldconst[i]);
	}
	return true;
}

bool as_trans(AS* old, AS* n)
{
	
	VecLit_t oldas = old->get_answers();
	VecLit_constit it;
	for(it=oldas.begin(); it!=oldas.end(); ++it)
	{
		if(it->name().compare("m_ANS")==0)
		{
			Literal litn;	
			lit_trans(*it, litn);
			n->add_answer(litn);
		}
	}
	return true;
}

bool trans_answerset(string ins, string& out)
{
	as_grammar as;
	if(ins.size()==0)
		return false;
	//cout<<ins<<endl;
	tree_parse_info<> info = ast_parse(ins.c_str(), as, space_p);
	if(info.full)
	{
		//cout<<"parsed"<<endl;
		AS* curr_as = new AS();
		tree_it tree_begin = info.trees.begin();
		parse_AS(curr_as, tree_begin);
		AS* t_as = new AS();
		as_trans(curr_as, t_as);
		out = t_as->str();
		delete t_as;
		delete curr_as;
	}
	else
	{
		cerr<<"Parsing answer set failed : "<<endl;
		cerr<<ins<<endl;
		return false;
	}
	return true;
}

