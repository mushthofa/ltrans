/******************************************************************
**	TRANSFORM.CPP : defines the transformation functions		**
*******************************************************************/

#include "transform.hpp"

void Program::transform_uni_pred()
{
	//cout<<"Program::transform_uni_pred"<<endl;
	int arr = this->maxarity();
	LineVec_it it;
	for(it=lines.begin();it!=lines.end();++it)	
	{
		if((*it)->type()==FACT)
		{
			//cout<<"transform fact = "<<((Fact*)*it)->str()<<endl;
			((Fact*)*it)->transform_uni_pred(arr+1);
		}
		else if((*it)->type()==RULE)
		{
			//cout<<"transform rule = "<<((Rule*)*it)->str()<<endl;
			((Rule*)*it)->transform_uni_pred(arr+1);
		}
		else
		{
			cerr<<"Error, assertion failed: type()==FACT||type()==RULE"<<endl;
			cerr<<"in Program::transform_uni_pred()"<<endl;
			assert(0);
		}
	}
	//cout<<"finished trans_uni_pred"<<endl;
}

void Fact::transform_uni_pred(int arr)
{
	//cout<<"Fact::transform_uni_pred"<<endl;
	lit.transform_uni_pred(arr);
}

void Rule::transform_uni_pred(int arr)
{
	//cout<<"Rule::transform_uni_pred"<<endl;
	Litset_it it;
	//cout<<"Transforming head = "<<head.lit.str()<<endl;
	head.lit.transform_uni_pred(arr);
	Literal curr;
	Litset_t new_pbl;
	Litset_t new_nbl;
	for(it = pbl.begin();it!=pbl.end();++it)
	{
		Literal curr = (*it);
		//cout<<"pbl lit = "<<curr.str()<<endl;
		curr.transform_uni_pred(arr);
		new_pbl.insert(curr);

	}
	for(it = nbl.begin();it!=nbl.end();++it)
	{
		Literal curr = (*it);
		//cout<<"nbl lit = "<<curr.str()<<endl;
		curr.transform_uni_pred(arr);
		new_nbl.insert(curr);
	}
	pbl = new_pbl;
	nbl = new_nbl;
}

void Literal::transform_uni_pred(int arr)
{
	/*cout<<"Liter transform_uni_pred"<<endl;
	cout<<"input = "<<str()<<endl;
	cout<<"input arity="<<pred_arity<<endl;
	cout<<"target arity="<<arr<<endl; */
	string o_name = pred_name;
	pred_name = m_NEWPRED;
	map_revit it;
	string arg;
	int index;
	if(!constant_map.empty())
	{
		for(it=constant_map.rbegin();it!=constant_map.rend();++it)
		{
			index = it->first;
			arg = it->second;
	//		cout<<"add const "<<index<<"=> "<<arg<<endl;
			constant_map.insert(pair<int,string>(index+1, arg));
			map_it curr_it = constant_map.find(index);
			constant_map.erase(curr_it);
		}
	}
	
	if(!var_map.empty())
	{
		for(it=var_map.rbegin();it!=var_map.rend();++it)
		{
			index = it->first;
			arg = it->second;
	//		cout<<"add var "<<index<<"=> "<<arg<<endl;
			var_map.insert(pair<int,string>(index+1, arg));
			map_it curr_it = var_map.find(index);
			var_map.erase(curr_it);
		}
	}
	//cout<<"finished moving args"<<endl;
	constant_map.insert(pair<int,string>(0,o_name));
	pred_arity++;
	int i;
	if(pred_arity<arr)
	{
		//cout<<"arity filling"<<endl;
		for(i=pred_arity;i<arr;i++)
		{
		//	cout<<"i = "<<i<<endl;
			constant_map.insert(pair<int, string>(i,m_NEWCONST));
			pred_arity++;
		}
	}
	//cout<<"output = "<<this->str()<<endl;
}

Program gen_map(const Program& input)
{
	Program output;
	stringset_it itp, itc;
	int arity = input.maxarity();
	stringset_t tconstants = input.get_consts();
	
	/* The following is actually not really always necessary, cause some 
	cases might not need new constants at all. Also, even if the program
	needs it, not all predicates needs it */
	
	/* Fix now, only add m_NEWCONST if input program 
		has different arities on the literals */
	
	if(!input.uniform_arity())
		tconstants.insert(m_NEWCONST);
	vecstring_t constants;
	//constants.resize(tconstants.size());
	for(itc=tconstants.begin();itc!=tconstants.end();++itc)
	{
		constants.push_back(*itc);
		//cout<<*itc<<endl;
	}
	vecstring_it c_it;
	stringset_t preds = input.get_preds();
	int id=1;
	long int perm_i;
	long int map_size = pow((double)constants.size(), arity);
	for(itp=preds.begin();itp!=preds.end();++itp)
	{
		
		for(perm_i=0; perm_i<map_size; perm_i++)
		{
			Literal lit;
			string map_name(m_MAP);
			lit.set_name(map_name);
			lit.add_const(0, *itp);
			vector<int> v_id = var_rep(constants.size(), arity, perm_i);
			vector<int>::iterator v_it;
			int arg_i = 1;
			for(v_it=v_id.begin(); v_it!=v_id.end(); ++v_it)
			{
				//cout<<"arg_i = "<<arg_i<<", constants = "<<constants.at(*v_it)<<endl;
				lit.add_const(arg_i++, constants.at(*v_it));
			}
			ostringstream strid;
			strid<<id;
			lit.add_const(arg_i++, m_MID + strid.str());
			Fact *fact = new Fact(id++);
			fact->set_lit(lit);
			output.add_line(fact);
		}
		
	}
	
	
	stringset_t neg_preds = input.get_negpreds();
	for(itp=neg_preds.begin();itp!=neg_preds.end();++itp)
	{
		
		for(perm_i=0; perm_i<map_size; perm_i++)
		{
			Literal lit;
			lit.set_name(m_MAP);
			lit.add_const(0, m_NEG + *itp);
			vector<int> v_id = var_rep(constants.size(), arity, perm_i);
			vector<int>::iterator v_it;
			int arg_i = 1;
			for(v_it=v_id.begin(); v_it!=v_id.end(); ++v_it)
			{
				lit.add_const(arg_i++,constants.at(*v_it));
			}
			ostringstream strid;
			strid<<id;
			lit.add_const(arg_i++, m_MID + strid.str());
			Fact *fact = new Fact(id++);
			fact->set_lit(lit);
			output.add_line(fact);
		}
		
		
	}
	return output;
}

vector<int> var_rep(int size, int n, long int index)
{
	//cout<<"enter var_rep"<<endl;
	vector<int> input;
	int i;
	for(i=0;i<size;i++)
		input.push_back(i);
	vector<int> result;
	int idx;
	for(idx=0; idx<n;idx++)
	{
		long int div = pow((double)size,(double)(n-idx-1));
		int rem;
		rem = (index/div)%size;
		result.push_back(input.at(rem));
	}
	//cout<<"finished var_rep"<<endl;
	return result;
}

Program_Dom::Program_Dom(int ar)
{
	arity = ar;
	
}

Program_Dom::Program_Dom(const Program& prog)
{
	arity = prog.maxarity();
	num_clause = prog.get_lines().size();
	constants = prog.get_consts(); 
	
	/* TODO : try adding predicate names in the constant list */
	/* NOT working! */
	/*
	stringset_t preds = prog.get_preds();
	stringset_it p_it;
	for(p_it=preds.begin(); p_it!=preds.end();++p_it)
	{
		constants.insert(*p_it);
	}
	*/
	/* Trying to increase label size as num of consts + num of preds */
	label = constants.size();
	/* now trying to undo it */
	
	stringset_t preds = prog.get_preds();
	label += preds.size();
	preds = prog.get_negpreds();
	label += preds.size();
	
	LineVec_it it;
	LineVec_t mylines = prog.get_lines();
	int curr_id;
	stringset_t curr_vars;
	stringset_it s_it;
	for(it=mylines.begin();it!=mylines.end();++it)	
	{

		if((*it)->type()==RULE)
		{
			curr_vars = ((Rule*)*it)->get_vars();
			curr_id = ((Rule*)*it)->getID();
			rules.push_back(curr_id);
			for(s_it=curr_vars.begin();s_it!=curr_vars.end();++s_it)
			{
				var_map.insert(pair<int,string>(curr_id,*s_it));
			
			}
		}
		else
		{
			curr_id = ((Fact*)*it)->getID();
			facts.push_back(curr_id);
		}
	}
}


void Program_Dom::set_num_clause(int num)
{
	num_clause = num;
}

void Program_Dom::add_var_map(int id, string var)
{
	var_map.insert(pair<int, string>(id, var));
}

void Program_Dom::add_const(string c)
{
	constants.insert(c);
}
int Program_Dom::get_num_clause() const
{
	return num_clause;
}

int Program_Dom::get_arity()const
{
	return arity;
}

stringset_t Program_Dom::get_constants()const
{
	return constants;
}

argmap_t Program_Dom::get_var_map()const
{
	return var_map;
}

Program_Dom& Program_Dom::operator=(const Program_Dom& rhs)
{
	if(this!=&rhs)
	{
		arity = rhs.get_arity();
		num_clause = rhs.get_num_clause();
		constants = rhs.get_constants();
		var_map = rhs.get_var_map();
	}
	return *this;

}

string Program_Dom::write(bool as_check) const /* For AS checking we don't need facts and labels */
{
	string output;
	ostringstream outputstream(ostringstream::out);
	int i;
	/* Changed... 1-nov */
	vector<int>::const_iterator v_it;
	for (v_it = rules.begin(); v_it != rules.end(); ++v_it)
		outputstream<<m_RULE<<"(r"<<*v_it<<")."<<endl;
	if(!as_check)
		for(v_it=facts.begin(); v_it!=facts.end(); ++v_it)
			outputstream<<m_FACT<<"(f"<<*v_it<<")."<<endl;
	/*....................*/
	
	stringset_it it;
	for(it=constants.begin();it!=constants.end();++it)
		outputstream<<m_CONST<<"("<<(*it)<<")."<<endl;
	
	argmap_constit var_it;
	for(var_it=var_map.begin();var_it!=var_map.end();++var_it)
	{
		outputstream<<m_RULEVAR<<"(r"<<var_it->first<<", "<<m_VARPREF<<var_it->second<<")."<<endl;
	}
	
	/* changing number of labels */
	/* Remove the plus one for the exponent n^k & */
	/* We don't need it because the first arguments are
	  always grounded with the predicate names */
	if(!as_check)
	{
		long int sel_size = pow((double)label , (int)arity /* + 1 */);
		for(i=1;i<=sel_size;i++)
			outputstream<<m_LABEL<<"(l"<<i<<")."<<endl;
	}
	output = outputstream.str();
	return output;
}

Program gen_table(const Program& input, stringset_t neg_preds)
{
	Program output;
	LineVec_it it;
	int id=1;
	LineVec_t lines = input.get_lines();
	stringset_it npreds_it;
	if(!neg_preds.empty())
	{
		for(npreds_it = neg_preds.begin();npreds_it!=neg_preds.end();++npreds_it)
		{
			Fact* neg_fact = new Fact(id++);
			Literal neg_lit(m_COMPL);
			neg_lit.add_const(0, *npreds_it);
			string neg_compl = m_NEG + *npreds_it;
			neg_lit.add_const(1, neg_compl);
			neg_fact->set_lit(neg_lit);
			output.add_line(neg_fact);
		}
	}
	for(it=lines.begin();it!=lines.end();++it)	
	{
		if((*it)->type()==FACT)
		{
			Fact* tab_fact = new Fact(id++);
			gen_table_fact(*((Fact*)*it), tab_fact, (*it)->getID());
			output.add_line(tab_fact);
			//cout<<"add fact : "<<tab_fact->str()<<endl;
		}
		else if((*it)->type()==RULE)
		{
			Litset_t lits = gen_table_rule(*((Rule*)*it), (*it)->getID());
			Litset_it f_it;
			for(f_it = lits.begin();f_it!=lits.end();++f_it)
			{
				Fact* tab_rule;
				tab_rule = new Fact(id++);
				tab_rule->set_lit(*f_it);
				output.add_line(tab_rule);
			}
		}
		else
		{
			cout<<"Error, assertion failed: type()==FACT||type()==RULE"<<endl;
			assert(0);
		}
	}
	return output;
}

void gen_table_fact(const Fact& input, Fact* output, int id)
{
	
	Literal mylit = input.get_lit();
	Literal outlit;
	outlit.set_name(m_TABH);
	map_t args = mylit.get_const();
	map_it args_it = args.begin();
	string pred_name = args_it->second;
	if(!mylit.sign())
	{	
		pred_name = m_NEG + pred_name;
	}
	
	ostringstream strstr;
	strstr<<id;
	string rule_num = "r" + strstr.str();
	outlit.add_const(0, rule_num);
	strstr.str("");
	outlit.add_const(1, pred_name);
	int i=2;
	if(args.size()>=1)
	{
		for(args_it=args.begin();args_it!=args.end();++args_it)	//Skip the first, as it is already processed
		{
			if(args_it!=args.begin())
				outlit.add_const(i++, args_it->second);
		}
	}
	//cout<<"literal finished table:"<<outlit.str()<<endl;
	output->set_lit(outlit);
}

Litset_t gen_table_rule(const Rule& input, int id)
{
	ostringstream strstr;
	strstr<<id;
	string rule_num = "r" + strstr.str();

	Litset_t output;
	Literal hlit = input.get_head().lit;
	Literal outlit;
	outlit.set_name(m_TABH);
	outlit.add_const(0, rule_num);
	map_t args = hlit.get_const();
	map_it args_it = args.begin();
	string pred_name = args_it->second;
	if(hlit.sign()==false)
	{	
		pred_name = m_NEG + pred_name;
	}
	
	outlit.add_const(1, pred_name);
	if(args.size()>=1)
	{
		for(args_it=args.begin();args_it!=args.end();++args_it)	//Skip the first, as it is already processed
		{
			if(args_it!=args.begin())
				outlit.add_const( args_it->first + 1, args_it->second);
		}
	}
	
	args = hlit.get_var();
	for(args_it=args.begin();args_it!=args.end();++args_it)
	{
		//cout<<"getting var "<<args_it->second<<endl;
		outlit.add_const(args_it->first + 1, m_VARPREF + args_it->second);
	}
	
	//cout<<"literal finished table:"<<outlit.str()<<endl;
	output.insert(outlit);
	
	// Finished processing the head.... now do the same for the PBL and NBL
	
	Litset_t pbl = input.get_pbl();
	Litset_it l_it;
	for(l_it = pbl.begin();l_it!=pbl.end();++l_it)
	{
		Literal pblit;
		pblit.set_name(m_TABPB);
		args = l_it->get_const();
		args_it = args.begin();
		pred_name = args_it->second;
		if(l_it->sign()==false)
		{	
			pred_name = m_NEG + pred_name;
		}
		pblit.add_const(0, rule_num);
		pblit.add_const(1, pred_name);
		if(args.size()>=1)
		{
			for(args_it=args.begin();args_it!=args.end();++args_it) 
				//Skip the first, as it is already processed
			{
				if(args_it!=args.begin())
					pblit.add_const(args_it->first + 1, args_it->second);
			}
		}
	
		args = l_it->get_var();
		for(args_it=args.begin();args_it!=args.end();++args_it)
		{
			//cout<<"getting var "<<args_it->second<<endl;
			pblit.add_const(args_it->first + 1, m_VARPREF + args_it->second);
		}
	
		//cout<<"literal finished table:"<<outlit.str()<<endl;
		output.insert(pblit);
			
	}
	Litset_t nbl = input.get_nbl();
	for(l_it=nbl.begin();l_it!=nbl.end();++l_it)
	{
		Literal nblit;
		nblit.set_name(m_TABNB);
		args = l_it->get_const();
		args_it = args.begin();
		pred_name = args_it->second;
		if(l_it->sign()==false)
		{	
			pred_name = m_NEG + pred_name;
		}
		nblit.add_const(0, rule_num);
		nblit.add_const(1, pred_name);
		if(args.size()>=1)
		{
			for(args_it=args.begin();args_it!=args.end();++args_it) 
				//Skip the first, as it is already processed
			{
				if(args_it!=args.begin())
					nblit.add_const(args_it->first + 1, args_it->second);
			}
		}
	
		args = l_it->get_var();
		for(args_it=args.begin();args_it!=args.end();++args_it)
		{
			//cout<<"getting var "<<args_it->second<<endl;
			nblit.add_const(args_it->first + 1, m_VARPREF + args_it->second);
		}
	
		//cout<<"literal finished table:"<<outlit.str()<<endl;
		output.insert(nblit);
	}
	return output;
}

Program gen_input(const Program& input, bool as_check)
{
	Program output;
	int arr = input.maxarity();
	LineVec_it it, it_res;
	LineVec_t res;
	int id=1;
	LineVec_t lines = input.get_lines();
	for(it=lines.begin();it!=lines.end();++it)	
	{
		if((*it)->type()==FACT)
		{
			if(!as_check) /* For AS check, input program does not need facts from original program */
			{
				res = gen_input_fact(*((Fact*)*it), id++);
				for(it_res=res.begin();it_res!=res.end();++it_res)
				{
					output.add_line(*it_res);
				}
			}
		}
		else if((*it)->type()==RULE)
		{
			res = gen_input_rule(*((Rule*)*it), id++, as_check);
			for(it_res=res.begin();it_res!=res.end();++it_res)
			{
				output.add_line(*it_res);
			}
		}
		else
		{
			cout<<"Error, assertion failed: type()==FACT||type()==RULE"<<endl;
			cout<<"in Program::transform_uni_pred()"<<endl;
			assert(0);
		}
	}
	return output;
}

LineVec_t gen_input_fact(const Fact& fact, int id)
{
	int num = fact.getID();
	ostringstream numstr;
	numstr<<num;
	Rule* in_fact = new Rule(id);
	Literal hlit;
	hlit.set_name(m_HEAD);
	hlit.add_var(0,"A");
	string fact_num = "f" + numstr.str();
	hlit.add_var(1, fact_num);
	Head h;
	h.hasLit = true;
	h.lit = hlit;
	in_fact->set_head(h);
	Literal sel_lit;
	/* changed for fact, 1-nov */
	sel_lit.set_name(m_FACT);
	//sel_lit.add_var(0,"L");
	sel_lit.add_const(0, fact_num);
	/* ............... */
	in_fact->add_pbl(sel_lit);
	Literal map_lit;
	map_lit.set_name(m_MAP);
	Literal factlit = fact.get_lit();
	map_t constants = factlit.get_const();
	map_it it;
	string pred_name = constants.begin()->second;
	if(factlit.sign()==false)
	{
		pred_name = m_NEG + pred_name;
	}
	map_lit.add_const(0, pred_name);
	int i=1;
	for(it=constants.begin();it!=constants.end();++it)
	{
		if(it!=constants.begin())
		{
			map_lit.add_const(it->first, it->second);
			i++;
		}
	}
	map_lit.add_var(i, "A");
	in_fact->add_pbl(map_lit);
	LineVec_t input_fact;
	input_fact.push_back(in_fact);
	return input_fact;
}

LineVec_t gen_input_rule(const Rule& rule, int id, bool as_check)
{
	LineVec_t output;
	
	/* This creates the rule id string*/
	int myid = rule.getID();
	ostringstream rulestr;
	rulestr << myid;
	string rule_str = "r" + rulestr.str();
	
	/* Do input generation for rule heads */
	Rule* hrule = new Rule(id++);
	
	/* first create the head which is always head(A,L) */
	Literal hin_lit;
	hin_lit.set_name(m_HEAD);
	hin_lit.add_var(0, "A");
	/* For AS check, input program does not need argument L for labels */
	if(!as_check)
		hin_lit.add_var(1, "L");
	Head hin_head;
	hin_head.lit = hin_lit;
	hin_head.hasLit = true;
	hrule->set_head(hin_head);
	
	/* Then create the tabH according to the head of the rule */
	Literal hlit = rule.get_head().lit;
	map_t constants = hlit.get_const();
	map_t vars = hlit.get_var();
	Literal tabh_lit;
	tabh_lit.set_name(m_TABH);
	map_it args_it;
	string pred_name = constants.find(0)->second;
	if(hlit.sign()==false)
	{
		pred_name = m_NEG + pred_name;
	}
	//cout<<"Pred = "<<pred_name<<endl;
	tabh_lit.add_const(0, rule_str);
	tabh_lit.add_const(1, pred_name);
	for(args_it=constants.begin();args_it!=constants.end();++args_it)
	{
		if(args_it!=constants.begin())		/* Skips over the first (pred name) */
		{
			//cout<<"add constant "<<args_it->first + 1<<"==>"<<args_it->second<<endl;
			tabh_lit.add_const(args_it->first + 1, args_it->second);
		}
	}
	for(args_it=vars.begin();args_it!=vars.end();++args_it)
	{
		//cout<<"add variable "<<args_it->first + 1<<"==>"<<args_it->second<<endl;
		tabh_lit.add_const(args_it->first + 1, m_VARPREF + args_it->second);
	}
	//cout<<"finished converting head"<<endl;
	//cout<<"result is : "<<tabh_lit.str()<<endl;
	hrule->add_pbl(tabh_lit);
	
	/* Then add the sel(L,rN) */
	Literal sel_lit;
	sel_lit.set_name(m_SEL);
	if(!as_check)
	{
		sel_lit.add_var(0,"L");
		sel_lit.add_const(1, rule_str);
	}
	else
	{
		sel_lit.add_const(0, rule_str);
	}
	hrule->add_pbl(sel_lit);
	
	/* Then put all the variables valuation literals
		val(L, var_X, Val_X).....
	*/
	
	for(args_it=vars.begin();args_it!=vars.end();++args_it)
	{
		Literal val_lit;
		val_lit.set_name(m_VAL);
		if(as_check)
			val_lit.add_const(0, rule_str);
		else
			val_lit.add_var(0,"L");
		val_lit.add_const(1, m_VARPREF + args_it->second);
		val_lit.add_var(2, VAL + args_it->second);
		hrule->add_pbl(val_lit);
	
	}
	
	/* Finally, put the map literal */
	Literal map_lit;
	map_lit.set_name(m_MAP);
	map_lit.add_const(0, pred_name);
	int i=1;
	for(args_it=constants.begin(); args_it!=constants.end(); ++args_it)
	{
		if(args_it!=constants.begin())		/* Skips over the first (pred name) */
		{
			//cout<<"constant = "<<args_it->second<<endl;
			map_lit.add_const(args_it->first, args_it->second);
			i++;
		}
	}
	
	for(args_it=vars.begin();args_it!=vars.end();++args_it)
	{
		//cout<<"var = "<<args_it->second<<endl;
		map_lit.add_const(args_it->first, VAL + args_it->second);
		i++;
	}
	
	map_lit.add_var(i, "A");
	hrule->add_pbl(map_lit);
	
	/* Finish generating a rule for the head,
		now push it into the result vector
	*/
	output.push_back(hrule);
	
	/* Do input generation for each pbl's */
	Litset_t pbl = rule.get_pbl();
	Litset_it pbl_it;
	for(pbl_it = pbl.begin();pbl_it!=pbl.end();++pbl_it)
	{
		Rule* pblrule = new Rule(id++);
		/* first create the head which is always pbl(A,L) */
		Literal pblin_lit;
		pblin_lit.set_name(m_PBL);
		pblin_lit.add_var(0, "A");
		if(!as_check)
			pblin_lit.add_var(1, "L");
		Head pblin_head;
		pblin_head.lit = pblin_lit;
		pblin_head.hasLit = true;
		pblrule->set_head(pblin_head);
		
		/* Then create the tabPB according to the head of the rule */
		Literal pbllit = *pbl_it;
		constants = pbllit.get_const();
		vars = pbllit.get_var();
		Literal tabpb_lit;
		tabpb_lit.set_name(m_TABPB);
		pred_name = constants.find(0)->second;
		if(pbllit.sign()==false)
		{
			pred_name = m_NEG + pred_name;
		}
		//cout<<"Pred = "<<pred_name<<endl;
		tabpb_lit.add_const(0, rule_str);
		tabpb_lit.add_const(1, pred_name);
		for(args_it=constants.begin();args_it!=constants.end();++args_it)
		{
			if(args_it!=constants.begin())		/* Skips over the first (pred name) */
			{
				//cout<<"add constant "<<args_it->first + 1<<"==>"<<args_it->second<<endl;
				tabpb_lit.add_const(args_it->first + 1, args_it->second);
			}
		}
		for(args_it=vars.begin();args_it!=vars.end();++args_it)
		{
			//cout<<"add variable "<<args_it->first + 1<<"==>"<<args_it->second<<endl;
			tabpb_lit.add_const(args_it->first + 1, m_VARPREF + args_it->second);
		}
		//cout<<"finished converting head"<<endl;
		//cout<<"result is : "<<tabh_lit.str()<<endl;
		pblrule->add_pbl(tabpb_lit);
	
		/* Then add the sel(L,rN) */
		Literal pblsel_lit;
		pblsel_lit.set_name(m_SEL);
		if(!as_check)
		{
			pblsel_lit.add_var(0,"L");
			pblsel_lit.add_const(1, rule_str);
		}
		else
			pblsel_lit.add_const(0, rule_str);
		pblrule->add_pbl(pblsel_lit);
	
		/* Then put all the variables valuation literals
			val(L, var_X, Val_X).....
		*/
		for(args_it=vars.begin();args_it!=vars.end();++args_it)
		{
			Literal val_lit;
			val_lit.set_name(m_VAL);
			if(as_check)
				val_lit.add_const(0, rule_str);
			else
				val_lit.add_var(0,"L");
			val_lit.add_const(1, m_VARPREF + args_it->second);
			val_lit.add_var(2, VAL + args_it->second);
			pblrule->add_pbl(val_lit);
	
		}
	
		/* Finally, put the map literal */
		Literal map_lit;
		map_lit.set_name(m_MAP);
		map_lit.add_const(0, pred_name);
		i=1;
		for(args_it=constants.begin(); args_it!=constants.end(); ++args_it)
		{
			if(args_it!=constants.begin())		/* Skips over the first (pred name) */
			{
				//cout<<"constant = "<<args_it->second<<endl;
				map_lit.add_const(args_it->first, args_it->second);
				i++;
			}
		}
	
		for(args_it=vars.begin();args_it!=vars.end();++args_it)
		{
			//cout<<"var = "<<args_it->second<<endl;
			map_lit.add_const(args_it->first, VAL + args_it->second);
			i++;
		}
	
		map_lit.add_var(i, "A");
		pblrule->add_pbl(map_lit);
	
		/* Finish generating a rule for the pbl,
			now push it into the result vector
		*/
		output.push_back(pblrule);
	
	}
	
	
	/* Do input generation for each nbl's */
	Litset_t nbl = rule.get_nbl();
	Litset_it nbl_it;
	for(nbl_it = nbl.begin();nbl_it!=nbl.end();++nbl_it)
	{
		Rule* nblrule = new Rule(id++);
		/* first create the head which is always nbl(A,L) */
		Literal nblin_lit;
		nblin_lit.set_name(m_NBL);
		nblin_lit.add_var(0, "A");
		if(!as_check)
			nblin_lit.add_var(1, "L");
		Head nblin_head;
		nblin_head.lit = nblin_lit;
		nblin_head.hasLit = true;
		nblrule->set_head(nblin_head);
		
		/* Then create the tabNB according to the head of the rule */
		Literal nbllit = *nbl_it;
		constants = nbllit.get_const();
		vars = nbllit.get_var();
		Literal tabnb_lit;
		tabnb_lit.set_name(m_TABNB);
		pred_name = constants.find(0)->second;
		if(nbllit.sign()==false)
		{
			pred_name = m_NEG + pred_name;
		}
		//cout<<"Pred = "<<pred_name<<endl;
		tabnb_lit.add_const(0, rule_str);
		tabnb_lit.add_const(1, pred_name);
		for(args_it=constants.begin();args_it!=constants.end();++args_it)
		{
			if(args_it!=constants.begin())		/* Skips over the first (pred name) */
			{
				//cout<<"add constant "<<args_it->first + 1<<"==>"<<args_it->second<<endl;
				tabnb_lit.add_const(args_it->first + 1, args_it->second);
			}
		}
		for(args_it=vars.begin();args_it!=vars.end();++args_it)
		{
			//cout<<"add variable "<<args_it->first + 1<<"==>"<<args_it->second<<endl;
			tabnb_lit.add_const(args_it->first + 1, m_VARPREF + args_it->second);
		}
		//cout<<"finished converting head"<<endl;
		//cout<<"result is : "<<tabh_lit.str()<<endl;
		nblrule->add_pbl(tabnb_lit);
	
		/* Then add the sel(L,rN) */
		Literal nblsel_lit;
		nblsel_lit.set_name(m_SEL);
		if(!as_check)
		{
			nblsel_lit.add_var(0,"L");
			nblsel_lit.add_const(1, rule_str);
		}
		else
		{
			nblsel_lit.add_const(0, rule_str);
		}	
		nblrule->add_pbl(nblsel_lit);
	
		/* Then put all the variables valuation literals
		val(L, var_X, Val_X).....
		*/
		for(args_it=vars.begin();args_it!=vars.end();++args_it)
		{
			Literal val_lit;
			val_lit.set_name(m_VAL);
			if(as_check)
				val_lit.add_const(0, rule_str);
			else
				val_lit.add_var(0,"L");
			val_lit.add_const(1, m_VARPREF + args_it->second);
			val_lit.add_var(2, VAL + args_it->second);
			nblrule->add_pbl(val_lit);
	
		}
	
		/* Finally, put the map literal */
		Literal map_lit;
		map_lit.set_name(m_MAP);
		map_lit.add_const(0, pred_name);
		i=1;
		for(args_it=constants.begin(); args_it!=constants.end(); ++args_it)
		{
			if(args_it!=constants.begin())		/* Skips over the first (pred name) */
			{
				//cout<<"constant = "<<args_it->second<<endl;
				map_lit.add_const(args_it->first, args_it->second);
				i++;
			}
		}
	
		for(args_it=vars.begin();args_it!=vars.end();++args_it)
		{
			//cout<<"var = "<<args_it->second<<endl;
			map_lit.add_const(args_it->first, VAL + args_it->second);
			i++;
		}
	
		map_lit.add_var(i, "A");
		nblrule->add_pbl(map_lit);
	
		/* Finish generating a rule for the pbl,
		now push it into the result vector
		*/
		output.push_back(nblrule);
	
	}
	return output;
}

string write_meta(int arity)
{
	ostringstream output(ostringstream::out);
	output<<"%% Meta Interpreter taken as-is: "<<endl;
	output<<META<<endl;
	output<<"\t"<<m_ANS<<"(Pred";
	int i;
	for(i=1; i<=arity;i++)
		output<<", A"<<i;
	output<<") :- in_AS(X), "<<m_MAP<<"(Pred";
	for(i=1; i<=arity;i++)
		output<<", A"<<i;
	output<<", X)."<<endl;
	return output.str();
}

string write_sel()
{
	ostringstream output(ostringstream::out);
	output<<"%% Selection and valuation: "<<endl;
	output<<SEL_VAL<<endl;
	return output.str();
}




/*End of transform definition file */
