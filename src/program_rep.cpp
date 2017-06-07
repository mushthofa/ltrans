/******************************************************************
**	PROGRAM_REP.CPP : program representations and operations	**
*******************************************************************/
#include "program_rep.hpp"

/* Definitions for Literal member methods */

Literal::Literal()
{
	pred_arity = 0;
	positive = true;

}

Literal::Literal(const string& name)
{
	pred_name = name;
	pred_arity = 0;
	positive = true;
}



Literal::Literal (const Literal& l)
{
	pred_name = l.name();
	pred_arity = l.arity();
	positive = l.sign();	
	constant_map = l.get_const();
	var_map = l.get_var();
}

void Literal::set_name(string s)
{
	pred_name = s;
}
Literal& Literal::operator=(const Literal& l)
{
	if(this!=&l)
	{
		this->pred_name = l.name();
		this->pred_arity = l.arity();
		this->positive = l.sign();
		/* TODO: MEMORY LOSS HERE!!!!! */
		/* Try to copy l's cmap to this->constant_map */
		/*this->constant_map.clear();
		map_it it;
		for(it=l.get_const().begin();it!=l.get_const().end();++it)
		{
			 this->constant_map.insert(pair<int, string>(it->first, it->second));
			
		}
		*/
		this->constant_map = l.get_const();
		this->var_map = l.get_var();
	}
	return *this;
}

void Literal::add_const(int pos, const string& name)
{	
	//cout<<"here"<<endl;
	//cout<<name<<endl;
	constant_map.insert(pair<int, string>(pos, name));
	pred_arity++;
}


void Literal::add_var(int pos, const string& name)
{
	//cout<<"Literal::add_var("<<pos<<", "<<name<<")"<<endl;
	var_map.insert(pair<int, string>(pos, name));
	pred_arity++;
}

string Literal::name() const
{
	return pred_name;
}

int Literal::arity() const
{
	return pred_arity;
}

void Literal::set_sign(bool s)
{
	positive = s;
}

bool Literal::sign() const
{
	return positive;
}

void Literal::set_const(map_t c_map)
{
	constant_map = c_map;
}

void Literal::set_var(map_t v_map)
{
	var_map = v_map;
}

map_t Literal::get_const() const
{
	return constant_map;
}

map_t Literal::get_var() const
{
	return var_map;
}

map_t Literal::params() const
{
	//cout<<"calling params()"<<endl;
	map_t myparams;
	map_constit it;
	if(!constant_map.empty())
	{
		for(it = constant_map.begin(); it!= constant_map.end(); ++it)
		{
			//cout<<it->second<<endl;
			myparams.insert(pair<int, string>(it->first, it->second));
		}
	}
	if(!var_map.empty())
	{
		for(it = var_map.begin(); it!= var_map.end(); ++it)
		{
			//cout<<it->second<<endl;
			myparams.insert(pair<int, string>(it->first, it->second));
		}
	}
	return myparams;
}

string Literal::str() const
{
	//cout<<"calling Literal::str()"<<endl;
	string out="";
	int i;
	if(!positive)
	{
		out += "-";
	}
	out += pred_name;
	if(pred_arity>0)
		out += "(";
	map_constit params_it;
	for(i=0;i<pred_arity;i++)
	{
		//cout<<"i="<<i<<endl;
		params_it = constant_map.find(i);
		if(params_it!=constant_map.end())
		{
			//cout<<params_it->second;
			out += params_it->second;
			
		}
		else
		{
			params_it = var_map.find(i);
			//cout<<"vars = "<<params_it->second;
			out += params_it->second;
		}
		if(i<pred_arity-1)
			out += ", ";
	}
	if(pred_arity>0)
		out += ")";
	return out;
}

/* Some boolean operator overloading for Literal
	required for set<Literal> container */

bool operator <(const Literal& l, const Literal& r)
{
	if(l.pred_arity!=r.pred_arity)
		return l.pred_arity<r.pred_arity;
	else return strcmp(l.str().c_str(),r.str().c_str())<0;
		
}

bool operator ==(const Literal& l, const Literal& r)
{
	if (l.pred_arity != r.pred_arity)
		return false;
	else return strcmp(l.str().c_str(),r.str().c_str())==0;
}

bool operator !=(const Literal& l, const Literal& r)
{
	return !(l==r);
}


/* Fact member functions definitions */

Fact::Fact(int id)
{
	ID = id;
}

line_t Fact::type() const
{
	return FACT;
}

int Fact::maxarity() const
{
	return lit.arity();
}
void Fact::set_lit(const Literal& l)
{
	lit = l;
}

Literal Fact::get_lit() const
{
	return lit;
}

stringset_t Fact::get_preds() const
/* For a fact, only 1 predicate is returned */
{
	stringset_t mypred;
	mypred.insert(lit.name());		
	return mypred;
}

stringset_t Fact::get_negpreds() const
{
	stringset_t mypred;
	if(lit.sign()==false)
		mypred.insert(lit.name());		
	return mypred;
}

stringset_t Fact::get_consts() const
{
	stringset_t myconst;
	map_constit it;
	map_t temp_const = lit.get_const();
	if(lit.arity()>0)
	{	
		for(it = temp_const.begin(); it!=temp_const.end();++it)
		{
			myconst.insert(it->second);
			//cout<<"insert("<<it->second<<")"<<endl;
		}
		return myconst;
	}
}


string Fact::str() const
{
	return (lit.str()+".");
}

bool Fact::uniform_arity() const
{
	/* Always true */
	return true;
}

bool operator <(const Fact& l, const Fact& r)
{
	Literal ll = l.get_lit();
	Literal lr = r.get_lit();
	if(ll.arity()!=lr.arity())
		return ll.arity()<lr.arity();
	else return strcmp(ll.str().c_str(),lr.str().c_str())<0;
		
}

bool operator ==(const Fact& l, const Fact& r)
{
	
	Literal ll = l.get_lit();
	Literal lr = r.get_lit();
	if (ll.arity() != lr.arity())
		return false;
	else return strcmp(ll.str().c_str(),lr.str().c_str())==0;
}

bool operator !=(const Fact& l, const Fact& r)
{
	Literal ll = l.get_lit();
	Literal lr = r.get_lit();
	return !(ll==lr);
}


/* definitions for Rule member functions */


Rule::Rule(int id)
{
	ID = id;
	head.hasLit = false;
}

line_t Rule::type() const
{
	return RULE;
}

int Rule::maxarity() const
{
	int res = 0;
	Litset_constit mylit;
	if(head.hasLit)
	{
		res = head.lit.arity();
	}
	if(!pbl.empty())
	{
		for(mylit = pbl.begin(); mylit!=pbl.end(); ++mylit )
		{
			if(mylit->arity()>res)
				res = mylit->arity();
		}

	}
	if(!nbl.empty())
	{
		for(mylit = nbl.begin(); mylit!=nbl.end(); ++mylit )
		{
			if(mylit->arity()>res)
				res = mylit->arity();
		}
	}
	return res;
}
bool Rule::uniform_arity() const
{
	int curr;
	Litset_constit mylit;
	if(head.hasLit)
	{
		curr = head.lit.arity();
	}
	if(!pbl.empty())
	{
		for(mylit = pbl.begin(); mylit!=pbl.end(); ++mylit )
		{
			if(mylit->arity()!=curr)
				return false;
		}

	}
	if(!nbl.empty())
	{
		for(mylit = nbl.begin(); mylit!=nbl.end(); ++mylit )
		{
			if(mylit->arity()>curr)
				return false;
		}
	}
	return true;
}
void Rule::set_head(const Head& h)
{
	head = h;
}

Head Rule::get_head() const
{
	return head;
}

void Rule::add_pbl(const Literal& b)
{
	pbl.insert(b);
}

void Rule::add_nbl(const Literal& b)
{
	nbl.insert(b);
}

Litset_t Rule::get_pbl() const
{
	return pbl;
}

Litset_t Rule::get_nbl() const
{
	return nbl;
}

stringset_t Rule::get_preds() const
{
	stringset_t mypreds;
	Litset_constit it;
	if(head.hasLit)
	{
		mypreds.insert(head.lit.name());
	}
	if(!pbl.empty())
	{
		for(it = pbl.begin();it!=pbl.end();++it)
		{
			mypreds.insert(it->name());
		}
	}
	if(!nbl.empty())
	{
		for(it = nbl.begin();it!=nbl.end();++it)
		{
			mypreds.insert(it->name());
		}
	}
	return mypreds;
}

stringset_t Rule::get_negpreds() const
{
	stringset_t mypreds;
	Litset_constit it;
	if(head.hasLit)
	{
		if(head.lit.sign()==false)
			mypreds.insert(head.lit.name());
	}
	if(!pbl.empty())
	{
		for(it = pbl.begin();it!=pbl.end();++it)
		{
			if(it->sign()==false)
				mypreds.insert(it->name());
		}
	}
	if(!nbl.empty())
	{
		for(it = nbl.begin();it!=nbl.end();++it)
		{
			if(it->sign()==false)
				mypreds.insert(it->name());
		}
	}
	return mypreds;
}

stringset_t Rule::get_consts() const
{
	stringset_t myconsts;
	map_t myparams;
	Litset_constit Lit;
	map_constit mit;
	if(head.hasLit)
	{
		myparams = head.lit.get_const();
		for(mit = myparams.begin(); mit!=myparams.end(); ++mit)
			myconsts.insert(mit->second);
	}
	if(!pbl.empty())
	{
		for(Lit = pbl.begin();Lit!=pbl.end();++Lit)
		{
			myparams = Lit->get_const();
			for(mit = myparams.begin(); mit!=myparams.end(); ++mit)
				myconsts.insert(mit->second);
		}
	}
	if(!nbl.empty())
	{
		for(Lit = nbl.begin();Lit!=nbl.end();++Lit)
		{
			myparams = Lit->get_const();
			for(mit = myparams.begin(); mit!=myparams.end(); ++mit)
				myconsts.insert(mit->second);
		}
	}
	return myconsts;
}

stringset_t Rule::get_vars() const
{
	stringset_t myvars;
	map_t myparams;
	Litset_constit Lit;
	map_constit mit;
	if(head.hasLit)
	{
		myparams = head.lit.get_var();
		for(mit = myparams.begin(); mit!=myparams.end(); ++mit)
			myvars.insert(mit->second);
	}
	if(!pbl.empty())
	{
		for(Lit = pbl.begin();Lit!=pbl.end();++Lit)
		{
			myparams = Lit->get_var();
			for(mit = myparams.begin(); mit!=myparams.end(); ++mit)
				myvars.insert(mit->second);
		}
	}
	if(!nbl.empty())
	{
		for(Lit = nbl.begin();Lit!=nbl.end();++Lit)
		{
			myparams = Lit->get_var();
			for(mit = myparams.begin(); mit!=myparams.end(); ++mit)
				myvars.insert(mit->second);
		}
	}
	return myvars;
}

string Rule::str() const
{
	string out="";
	Litset_constit Lit;
	if(head.hasLit)
	{
		out += head.lit.str();
	}
	out += " :- ";
	int i=0;
	if(!pbl.empty())
	{
		for(Lit = pbl.begin();Lit!=pbl.end();++Lit)
		{
			out += Lit->str();
			++i;
			if(i<pbl.size())
				out += ", ";
		}
	}
	if(!nbl.empty())
	{
		for(Lit = nbl.begin();Lit!=nbl.end();++Lit)
		{
			if(Lit==nbl.begin())
			{
				if(!pbl.empty())
					out += ", ";
			}
			else
			{
				out += ", ";
			}
			out += "not ";
			out += Lit->str();
			
		}
	}
	out +=".";
	return out;
}

void Rule::normalize_hc()
{
	if(!head.hasLit)
	{
		Literal fhead(m_FALSE);
		Literal fnbl(m_FALSE);
		head.lit = fhead;
		head.hasLit = true;
		nbl.insert(fnbl);
	}

}

/* Definitions for Program member functions */

Program::Program()
{
	facts = 0;
	rules = 0;
}
Program::Program(const Program& p)
{
	LineVec_it it;
	LineVec_t plines = p.get_lines();
	for(it=plines.begin();it!=plines.end();++it)
	{
		if((*it)->type()==FACT)
		{
			Fact* myline;
			myline = new Fact((*it)->getID());
			myline->set_lit((*it)->get_lit());
			this->add_line(myline);
		}
		else if((*it)->type()==RULE)
		{
			Rule* myline;
			myline = new Rule((*it)->getID());
			myline->set_head((*it)->get_head());
			Litset_it b_it;
			Litset_t bl;
			bl = (*it)->get_pbl();
			for(b_it=bl.begin();b_it!=bl.end();++b_it)
				myline->add_pbl(*b_it);
			bl = (*it)->get_nbl();
			for(b_it=bl.begin();b_it!=bl.end();++b_it)
				myline->add_nbl(*b_it);
			this->add_line(myline);
		}
		
		
	}
}

Program::~Program()
{
	LineVec_it it;
	//cout<<"Program::desctructor"<<endl;
	for(it=lines.begin();it!=lines.end();++it)
	{
		delete (*it);
	}
}
void Program::add_line(Program_Line* line)
{
	lines.push_back(line);
	if(line->type()==FACT)
		facts++;
	else if(line->type()==RULE)
		rules++;
	else
	{
		cerr<<"Assertion failed:line_type==FACT||line_type==RULE"<<endl;
		assert(0);
	}
}

stringset_t Program::get_preds() const
{
	stringset_t mypreds, curr_preds;
	LineVec_constit it;
	for(it=lines.begin();it!=lines.end();++it)
	{
		curr_preds = (*it)->get_preds();
		mypreds.insert(curr_preds.begin(), curr_preds.end());
	}
	return mypreds;
}

stringset_t Program::get_negpreds() const
{
	stringset_t mypreds, curr_preds;
	LineVec_constit it;
	for(it=lines.begin();it!=lines.end();++it)
	{
		curr_preds = (*it)->get_negpreds();
		mypreds.insert(curr_preds.begin(), curr_preds.end());
	}
	return mypreds;
}

stringset_t Program::get_consts() const
{
	stringset_t myconsts,  curr_consts;
	LineVec_constit it;
	for(it=lines.begin();it!=lines.end();++it)
	{
		curr_consts = (*it)->get_consts();
		myconsts.insert(curr_consts.begin(), curr_consts.end());
	}
	return myconsts;
}

int Program::maxarity() const
{
	int maxar = 0;
	int ar;
	LineVec_constit it;
	for(it=lines.begin();it!=lines.end();++it)
	{
		ar = (*it)->maxarity();
		if(ar>maxar)
			maxar = ar;
	}
	return maxar;
}

bool Program::uniform_arity() const
{
	int maxar = this->maxarity();
	int ar;
	LineVec_constit it;
	for(it=lines.begin();it!=lines.end();++it)
	{
		if(!((*it)->uniform_arity()))
			return false;
	}
	for(it=lines.begin();it!=lines.end();++it)
	{
		ar = (*it)->maxarity();
		if(ar<maxar)
			return false;
	}
	return true;
}

LineVec_t Program::get_lines() const
{
	return lines;
}

string Program::write(bool comment) const
{
	string output = "";
	LineVec_constit it;
	for(it=lines.begin();it!=lines.end();++it)
	{
		/*
		if((*it)->type()==FACT)
			cout<<"Fact(";
		else
			cout<<"Rule(";
		cout<<(*it)->getID();
		cout<<"): ";
		*/
		if(comment)
			output += "%% ";
		output += (*it)->str();
		output +="\n";
	}
	output += "\n";
	/*
		// Debug a bit 
	stringset_t preds = this->get_preds();
	stringset_it preds_it;
	cout<<"Predicates : {";
	int i= 0;
	for(preds_it=preds.begin(); preds_it!=preds.end(); ++preds_it)
	{
		cout<<(*preds_it);
		++i;
		if(i<preds.size())
			cout<<", ";
				
	}
	cout<<"}"<<endl;
	
	stringset_t consts = this->get_consts();
	stringset_it consts_it;
	cout<<"Constants : {";
	i=0;
	for(consts_it=consts.begin(); consts_it!=consts.end(); ++consts_it)
	{
		cout<<(*consts_it);
		++i;
		if(i<consts.size())
			cout<<", ";
				
	}
	cout<<"}"<<endl;
	

	*/
	return output;
}

bool Program::parse_line(int id, tree_it const& node)
{
	parser_id node_id = node->value.id();
	bool res;
	if(node_id==logic_grammar::literalID || node_id==logic_grammar::pred_nameID)
		/* Current line is a fact */
	{
		Fact* curr_line;
		curr_line = new Fact(id);
		res = parse_fact(curr_line, node);
		this->add_line(curr_line);
	}
	else if(node_id==logic_grammar::pruleID)
	{
		Rule* curr_line;
		curr_line = new Rule(id);
		res = parse_rule(curr_line, node);
		this->add_line(curr_line);
	}
	
	return res;		
}

void Program::normalize_hc()
{
	LineVec_it it;
	for(it=lines.begin();it!=lines.end();++it)	
	{
		if((*it)->type()==RULE)
			((Rule*)(*it))->normalize_hc();
	}
}

bool parse_fact(Fact* fact, tree_it const& node)
{
	//cout<<"parse fact"<<endl;
	bool res;
	Literal mylit ;
	res = parse_literal(mylit, node, FACT);
	fact->set_lit(mylit);
	return res;
}

bool add_body_lit(Rule* myrule, tree_it const& it)
{
	//cout<<"add body lit"<<endl;
	bool res;
	tree_it child_it;
	Literal lit;
	if(it->value.id()==logic_grammar::goalID)
	// 'not' body literal
	{
		child_it = it->children.begin();
		res = parse_literal(lit, child_it,RULE);
		if(!res)
			return false;
		myrule->add_nbl(lit);
	}
	else

	{
		res = parse_literal(lit, it, RULE);
		if(!res)
			return false;
		myrule->add_pbl(lit);
	}
	return true;
}

bool parse_body(Rule* myrule, tree_it const& it)
{
	//cout<<"parse body"<<endl;
	bool res = true;
	tree_it child_it;
	int i=0;
	if(it->value.id()==logic_grammar::bodyID)
	// muliple body literals;
	{
		for(child_it=it->children.begin();child_it!=it->children.end();++child_it)
		{
			//cout<<"i="<<i++<<endl;
			res = res && add_body_lit(myrule, child_it);
			if(!res)
				break;
		}
		
	}
	else	// one body literal only
	{
		res = add_body_lit(myrule, it);

	}
	return res;
}

bool parse_rule(Rule* myrule, tree_it const& it)
{
	//cout<<"parse rule"<<endl;
	bool res;
	Head head;
	int size = (it->children.end()-it->children.begin());	// size=1 if no head, 2 otherwise
	tree_it child_it = it->children.begin();
	if(size==1)
	{
		head.hasLit = false;
		myrule->set_head(head);
		res = parse_body(myrule, child_it);
		return res;
	}
	else if(size==2)
	{
		//cout<<"has head"<<endl;
		head.hasLit = true;
		
		res = parse_literal(head.lit, child_it, RULE);
		if(!res)
			return false;
		//cout<<"sethead"<<endl;;
		myrule->set_head(head);
		++child_it;
		//cout<<"process body"<<endl;
		res = parse_body(myrule, child_it);
		return res;
	}
	else
	{
		cerr<<"Assertion failed:size==1||size==2"<<endl;
		assert(0);
		return false;
	}
}

bool parse_literal(Literal& lit, tree_it const& it, line_t type)
{
	//cout<<"parse literal"<<endl;
	tree_it child_it, data_it;
	if(it->value.id()==logic_grammar::literalID)
	{
		lit.set_sign(false);
		data_it = it->children.begin();
	}
	else if(it->value.id()==logic_grammar::pred_nameID)
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
		if(child_it->value.id()==logic_grammar::constantID)
		{
			lit.add_const(i++, arg_name);
		}
		else if(child_it->value.id()==logic_grammar::variableID)
		{
			if(type==FACT)
			{
				cerr<<"Error: safety condition requires only constants can be arguments of a fact."<<endl;
				return false;
			}
			lit.add_var(i++,arg_name);
		}
		else
		{
			cerr<<"Assertion failed: id==constantID||id==variableID"<<endl;
			assert(0);
		}
		arg_name.clear();
		
	}
	content.clear();
	return true;
}



/* END OF PROGRAM_REP DEFINITION*/
