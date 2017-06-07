/***************************************************************************
 *   Copyright (C) 2008 by Mushthofa   								*
 *      														*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* TODO : Fix the newconst insertion in the program map : DONE */
/*		Fix Memory loss 							*/
/* 		Allow explicit negation to be processed and queried DONE*/

#include <boost/spirit/core.hpp>
#include <boost/spirit/tree/ast.hpp>
#include <boost/spirit/tree/tree_to_xml.hpp>
#include <boost/algorithm/string/trim.hpp>



#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cassert>
#include <GetPot>
#include <algorithm>


/* Trying popen */
#include <cstdio>

#include "logic_grammar.hpp"
#include "program_rep.hpp"
#include "transform.hpp"
#include "query_b.hpp"
#include "as_rep.hpp"
#include "as_trans.hpp"
#include "as_check.hpp"

string ignore_comments(const string&);
int main(int argc, char *argv[])
{
	logic_grammar program;
	Program input_program;
	string str, filename, filename_out;
	ifstream input;
	ofstream output;
	streambuf *buf;
	bool trans_only, brave;
	vecstring_t as_produced;
	ostringstream outstring(ostringstream::out);
	int curr_id = 1;
	string query_b;
	GetPot cl(argc, argv);
	filename = (string)cl.next_nominus();
	if(cl.search(1,"-t"))
		trans_only = true;
	else
		trans_only = false;
	brave = false;
	if(argc>=2)
	{
		query_b="";
		if(cl.search("-b"))
		{
			query_b = cl.next("");
			brave = true;
		}
		
		input.open(filename.c_str(), ifstream::in);
		if(input.is_open())
		{
			/* Redirect cin and outstring */
			buf = input.rdbuf();
			cin.rdbuf(buf);
			if(!trans_only)
			{
				filename_out = filename + ".trns";
				output.open(filename_out.c_str()); 
			}
		}
		else
		{
			cerr<<"Error opening file "<<filename<<endl;
			exit(1);
		}
	}
	else
	{
		/*cerr<<"No input file given"<<endl;
		exit(1);
		*/
		if(!trans_only)
		{
			filename_out = "stdin.trns";
			output.open(filename_out.c_str()); 
		}
		
	}
	while(getline(cin,str,'.'))
	{
		
		trim(str);
		str = ignore_comments(str);
		trim(str);
		if(cin.eof()&& str.size()>0)
		{
			cerr << "Error parsing clause "<<curr_id<<":"<<endl;
			cerr <<str<<endl;
			cerr <<" Unexpected end of file. (Missing a '.' ?)"<<endl;
			if(input.is_open())
				input.close();
			return EXIT_FAILURE;
		}
		/* sometimes there are empty lines.. we continue */
		
		if(str.size()==0)
			continue;
		
		tree_parse_info<> info = ast_parse(str.c_str(), program, space_p);
		
		if (info.full)
		{
		
				
			/* We parsed one line, put the result into object input_program */
			
			tree_it tree_begin = info.trees.begin();
			if(!input_program.parse_line(curr_id, tree_begin))
			{
				cerr << "Error parsing clause "<<curr_id<<":"<<endl;
				cerr <<str<<"."<<endl;
				if(input.is_open())
					input.close();
				return EXIT_FAILURE;
			}
			curr_id++;	
		}
		else
		{
			cerr << "%% Error parsing clause "<<curr_id<<":"<<endl;
			cerr <<str<<"."<<endl;
			if(input.is_open())
				input.close();
			return EXIT_FAILURE;
		}
	}
	if(input.is_open())
		input.close();
	
	int maxarity = input_program.maxarity();
	if(maxarity==0)
	{
		cerr<<"%% Program has only max arity = 0!"<<endl;
		cerr<<"%% No need to do transformation."<<endl;
		input_program.write(false);
		return EXIT_SUCCESS;
	}
	Program copy_program (input_program);
	outstring<<"%%  Initial program: "<<endl;
	outstring<<input_program.write(true);
	stringset_t neg_preds = input_program.get_negpreds();
	input_program.normalize_hc();
	outstring<<"%%  After normalize hard constraints: "<<endl;
	outstring<<input_program.write(true);
	outstring<<endl;
	/* Where do we put the program domain generation?
		after making the transformation to single predicate,
		or before? choice affect how the program domain 
		will be generated
	*/
	outstring<<"%%  Program domain: "<<endl;
	Program_Dom dom (input_program);
	outstring<<dom.write(false);
	outstring<<endl;
	
	outstring<<"%%  Generating program mapping : "<<endl;
	Program map_program = gen_map(input_program);
	outstring<<map_program.write(false);
	outstring<<endl;
	input_program.transform_uni_pred();
	outstring<<"%%  After transformation into a single predicate : "<<endl;
	outstring<<input_program.write(true);
	
	Program tab_program = gen_table(input_program, neg_preds);
	outstring<<"%%  Table program :"<<endl;
	outstring<<tab_program.write(false);
	
	Program in_program = gen_input(input_program, false);
	outstring<<"%% Generating input program for meta interpreter: "<<endl;
	outstring<<in_program.write(false);
	
	outstring<<"%% Writing the selection and valuation program: "<<endl;
	outstring<<write_sel()<<endl;
	outstring<<"%% Writing the meta-interpreter program: "<<endl;
	outstring<<write_meta(maxarity)<<endl;
	if(!query_b.empty())
	{
		string res;
		if(trans_qb(query_b, maxarity, res))
			outstring<<res<<endl;
		else
		{
			cerr<<"Query string is not well formed."<<endl;
			if(output.is_open())
				output.close();
			exit(1);
		}
	}
	if(trans_only)
		cout<<outstring.str()<<endl;
	else
	{
		//cout<<"here"<<endl;
		output.write(outstring.str().c_str(), strlen(outstring.str().c_str()));
		//cout<<outstring.str()<<endl;
		if(output.is_open())
			output.close();	
		FILE* as_out;
		char as_line[1000];
		string commandline = "dlv "+filename_out+" -silent -pfilter=sel,val,in_AS,m_ANS ";
		as_out = popen(commandline.c_str(),"r");
		bool find_as=false;
		while(fgets(as_line, sizeof(as_line), as_out))
		{
			string as_candidate (as_line);
			trim(as_candidate);
			as_grammar asg;
			tree_parse_info<> info_as = ast_parse(as_candidate.c_str(), asg, space_p);
			if(info_as.full)
			{
				AS* cand_as = new AS();
				tree_it tree_begin_as = info_as.trees.begin();
				parse_AS(cand_as, tree_begin_as);
				cand_as->sort();
				string as_sorted = cand_as->str_as();
				if(find(as_produced.begin(), as_produced.end(),as_sorted)!=as_produced.end())
					continue;	
				else
					as_produced.push_back(as_sorted);
			}
			//cout<<"checking candidate answer set: "<<as_candidate<<endl;
			if(as_check(as_candidate, copy_program, filename))
			{
				string trans_as;
				trans_answerset(as_candidate, trans_as);
				cout<<trans_as<<endl;
				cout<<endl;
			}
		}
		pclose(as_out);
	}
	return EXIT_SUCCESS;
	
}

/* Ignoring comments on the string read from the input */

string ignore_comments(const string& input)
{
	string::size_type index1, index2, length;
	string temp;
	index1 = input.find('%', 0);
	if(index1==string::npos)
		return input;
	else
	{
		index2 = input.find('\n',index1);
		if(index2==string::npos)
			return input.substr(0, index1);
		else
		{
			length = index2 - index1;
			temp = input;
			temp.erase(index1, length);
			return ignore_comments(temp);
		}
	}
}
