#include <algorithm>
#include "as_rep.hpp"
#include "program_rep.hpp"
/* Definitions for Literal member methods */
AS::AS()
{
	size = 0;
}

void AS::add_answer(const Literal& lt)
{
	answers.push_back(lt);
	//cout<<lt.str()<<endl;
	size++;
}

VecLit_t AS::get_answers() const
{
	return answers;
}
string AS::str() const
{
	VecLit_constit it;
	string output;
	output = "{";
	int i = 0;
	for(it=answers.begin(); it!=answers.end(); ++it)
	{
		i++;
		output += it->str();
		if(i!=size)
			output += ", ";
	}
	output += "}";
	return output;
}
string AS::str_as() const
{
	VecLit_constit it;
	string output;
	output = "{";
	int i = 0;
	for(it=answers.begin(); it!=answers.end(); ++it)
	{
		i++;
		string pred_name = it->name();
		if(pred_name.compare(m_ANS)==0)
		{
			output += it->str();
			if(i!=size)
				output += ", ";
		}
	}
	output += "}";
	return output;
}

void AS::sort() 
{
	std::sort(answers.begin(), answers.end());
}
		
bool parse_AS(AS* curr_as, const tree_it& it)
{
	bool res;
	parser_id node_id = it->value.id();
	tree_it child_it;
	if(node_id==as_grammar::answersetID)
	{
		for(child_it=it->children.begin();child_it!=it->children.end();++child_it)
		{
			Literal lit;
			parse_literal(lit, child_it);
			curr_as->add_answer(lit);
		}
	}
	else if(node_id==as_grammar::literalID || node_id==as_grammar::pred_nameID)
	{
		//cout<<"Here"<<endl;
		Literal lit;
		parse_literal(lit, it);
		curr_as->add_answer(lit);
	}
	return true;
}

bool parse_literal(Literal& lit, tree_it const& it)
{
	//cout<<"parse literal"<<endl;
	tree_it child_it, data_it;
	if(it->value.id()==as_grammar::literalID)
	{
		lit.set_sign(false);
		data_it = it->children.begin();
	}
	else if(it->value.id()==as_grammar::pred_nameID)
	{
		lit.set_sign(true);
		data_it = it;
	}
	string content (data_it->value.begin(), data_it->value.end());
	trim(content);
	//cout<<"pred = "<<content<<endl;
	lit.set_name(content);
	int i = 0;
	for(child_it = data_it->children.begin();child_it!=data_it->children.end();++child_it)
	{
		string arg_name(child_it->value.begin(), child_it->value.end());
		trim(arg_name);
		//cout<<"got argument= "<<arg_name<<endl;
		if(child_it->value.id()==as_grammar::constantID)
		{
			lit.add_const(i++, arg_name);
		}
		else if(child_it->value.id()==as_grammar::variableID)
		{
			lit.add_var(i++,arg_name);
		}
		else
		{
			cout<<"Assertion failed: id==constantID||id==variableID"<<endl;
			assert(0);
		}
		arg_name.clear();
		
	}
	content.clear();
	return true;
}
