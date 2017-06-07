#include "as_check.hpp"

bool as_check(string as_input, Program p, string f)
{
	if(as_input.size()==0)
		return false;
	int arity = p.maxarity();
	ostringstream outstring(ostringstream::out);
	stringset_t neg_preds = p.get_negpreds();
	p.normalize_hc();
	outstring<<"%%  Program domain: "<<endl;
	Program_Dom dom (p);
	outstring<<dom.write(true);
	outstring<<endl;
	
	outstring<<"%%  Generating program mapping : "<<endl;
	Program map_program = gen_map(p);
	outstring<<map_program.write(false);
	outstring<<endl;
	p.transform_uni_pred();
	outstring<<"%%  After transformation into a single predicate : "<<endl;
	outstring<<p.write(true);
	
	Program tab_program = gen_table(p, neg_preds);
	outstring<<"%%  Table program :"<<endl;
	outstring<<tab_program.write(false);
	
	Program in_program = gen_input(p, true);
	outstring<<"%% Generating input program for meta interpreter: "<<endl;
	outstring<<in_program.write(false);
	as_grammar asg;
	tree_parse_info<> info = ast_parse(as_input.c_str(), asg, space_p);
	if(info.full)
	{
		AS* cand_as = new AS();
		tree_it tree_begin = info.trees.begin();
		parse_AS(cand_as, tree_begin);
		/*
		cout<<"Checking answer set:"<<endl;
		cout<<cand_as->str()<<endl;
		*/
		VecLit_t answers;
		VecLit_it it;
		answers = cand_as->get_answers();
		for(it = answers.begin();it!=answers.end();++it)
		{
			if(it->name().compare(m_ANS)!=0)
			{
				if(it->name().compare("in_AS")!=0)	/* Put previous_ to pred names except for in_AS */
					outstring<<m_PREV;
				outstring<<it->str()<<"."<<endl;
			}
		}
		outstring<<CHECK_SEL_VAL<<endl;
		outstring<<first_line_pbtrue(arity)<<endl;
		outstring<<CHECK_CONST<<endl;
		cout<<outstring.str()<<endl;
		delete cand_as;
	}
	else
	{
		cerr<<"Parsing answer set failed : "<<endl;
		cerr<<as_input<<endl;
		return false;
	}
	//cout<<outstring.str()<<endl;
	
	string filename_check = f + ".check";
	ofstream output;
	output.open(filename_check.c_str());
	output.write(outstring.str().c_str(), strlen(outstring.str().c_str()));
	FILE* check_output;
	char check_a[1000];
	string commandline = "dlv "+filename_check+" -silent -brave";
	check_output = popen(commandline.c_str(),"r");
	fgets(check_a, sizeof(check_a), check_output);
	string check_line(check_a);
	output.close();
	if(find_first(check_line,"true"))
		return false;
	else
	
		return true;
	
}


string first_line_pbtrue(int a)
{
	ostringstream outstring(ostringstream::out);
	outstring<<"pos_body_exist :- map(";
	for(int i=0;i<=a;i++)
		outstring<<"_,";
	outstring<<"ID), pbl(ID).";
	return outstring.str();
}
