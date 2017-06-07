/******************************************************************
**	LOGIC_GRAMMAR.HPP : defines the grammar and parse tree		**
*******************************************************************/

#ifndef LOGIC_GRAMMAR_HPP
#define LOGIC_GRAMMAR_HPP

using namespace boost::spirit;


struct logic_grammar : public grammar<logic_grammar>
{
	static const int constantID = 1;
	static const int variableID = 2;
	static const int one_paramID = 3;
	static const int termID = 4;
	static const int pred_nameID = 5;
	static const int literalID = 6;
	static const int goalID = 7;
	static const int bodyID = 8;
	static const int headID = 9;
	static const int pruleID= 10;
	static const int pfactID= 11;
	static const int lineID= 12;




    template <typename ScannerT>
    struct definition
    {
        definition(logic_grammar const& )
        {
	//  Start grammar 
		   
		constant =  leaf_node_d[+digit_p | (lower_p >> *(alnum_p|ch_p('_'))) ];
		variable =  leaf_node_d[upper_p >> *(alnum_p|ch_p('_'))];
		one_param = constant | variable;
		pred_name = leaf_node_d[lower_p >> *(alnum_p|ch_p('_'))];
		term = 	root_node_d[pred_name] >> 
			inner_node_d
				[
					ch_p('(') >> infix_node_d
					[
						one_param  >> *(ch_p(',') >> one_param)
					] >> ch_p(')')
				]
			| root_node_d[pred_name];
		
		literal = (root_node_d[ch_p('-')]>>term) | 
			term ;
				
		goal = (root_node_d[str_p("not")]>>literal) |
			literal;	
		head = !goal;
		body = infix_node_d[ goal>>*(ch_p(',')>>goal)];
		prule = !literal >> root_node_d[str_p(":-")] >> 
				body ;//>> discard_node_d[ch_p('.')];	// we read the '.' in getline.
		pfact = !literal; //discard_last_node_d[!literal >> ch_p('.')];
		line = prule | pfact;
		
	//  End grammar definition
	
        }
	rule<ScannerT, parser_context<>, parser_tag<termID> >   term;
	rule<ScannerT, parser_context<>, parser_tag<variableID> > variable;
	rule<ScannerT, parser_context<>, parser_tag<one_paramID> > one_param;
	rule<ScannerT, parser_context<>, parser_tag<constantID> > constant;
	rule<ScannerT, parser_context<>, parser_tag<pred_nameID> >   pred_name;
	rule<ScannerT, parser_context<>, parser_tag<literalID> >   literal;
	rule<ScannerT, parser_context<>, parser_tag<goalID> >   goal;
	rule<ScannerT, parser_context<>, parser_tag<bodyID> >   body;
	rule<ScannerT, parser_context<>, parser_tag<headID> >   head;
	rule<ScannerT, parser_context<>, parser_tag<pruleID> >   prule;
	rule<ScannerT, parser_context<>, parser_tag<pfactID> >   pfact;
	rule<ScannerT, parser_context<>, parser_tag<lineID> >   line;
	rule<ScannerT, parser_context<>, parser_tag<lineID> > const&
	start() const { return line; }
    };
};

#endif

