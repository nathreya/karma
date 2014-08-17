#include "stdafx.h"
#include "lex.h"
#include "lex_utils.h"
#include "preprocess.h"
#include "parse.h"

using namespace __karma::__lex;
using namespace __karma::__util;
using namespace __karma::__preprocess;
using namespace __karma::__parse;

int main() {
	string ex = "";
	vector<cpp_token> vcpp = preprocess(ex, "FILE");
	for(int i = 0; i < vcpp.size(); i++)
		cout << vcpp[i].get_file_name() << ' ' << vcpp[i].get_line_number() << ' ' << vcpp[i].get_qualifier() << ' ' << vcpp[i].get_c_suffix_token_kind() << ' ' << vcpp[i].wide() << '\n';
	vector<shared_ptr<cpp_token>> vscpp;
	for(int i = 0; i < vcpp.size(); i++) {
		shared_ptr<cpp_token> cpp = make_shared<cpp_token>(vcpp[i]);
		vscpp.push_back(cpp);
	}
	string s;
	std::cin >> s; 
}
