/****************************************************************************
**	AS_GRAMMAR.HPP : defines the grammar and parse tree for an answer set	**
*****************************************************************************/

#ifndef AS_GRAMMAR_HPP
#define AS_GRAMMAR_HPP

using namespace boost::spirit;


struct as_grammar : public grammar<as_grammar>
{
	static const int constantID = 1;
	static const int variableID = 2;
	static const int one_paramID = 3;
	static const int termID = 4;
	static const int pred_nameID = 5;
	static const int literalID = 6;
	static const int answersetID = 7;



	template <typename ScannerT>
			struct definition
	{
		definition(as_grammar const& )
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
				
			answerset = inner_node_d
					[
					ch_p('{')>> infix_node_d
					[
					literal >> *(ch_p(',') >> literal)
					]
					>>ch_p('}')
					]
					| discard_node_d[ch_p('{')>>ch_p('}')];
			
	//  End grammar definition
	
		}
		rule<ScannerT, parser_context<>, parser_tag<termID> >   term;
		rule<ScannerT, parser_context<>, parser_tag<variableID> > variable;
		rule<ScannerT, parser_context<>, parser_tag<one_paramID> > one_param;
		rule<ScannerT, parser_context<>, parser_tag<constantID> > constant;
		rule<ScannerT, parser_context<>, parser_tag<pred_nameID> >   pred_name;
		rule<ScannerT, parser_context<>, parser_tag<literalID> >   literal;
		rule<ScannerT, parser_context<>, parser_tag<answersetID> >   answerset;
		rule<ScannerT, parser_context<>, parser_tag<answersetID> > const&
				start() const { return answerset; }
	};
};

#endif

