// cannot handle recursive macros
// can handle most concatenations with number-suffixes
// expression parser can handle most conditional preprocessor expressions
// conditional preprocessor directive cannot handle multi-character literals (eg. '\0')
// does not support preprocessor directives in macros

#include "stdafx.h"
#include "preprocess.h"

namespace __karma {
	namespace __preprocess {
		namespace __token {
			string const _line_macro = "__LINE__";
			string const _time_macro = "__TIME__";
			string const _date_macro = "__DATE__";
			string const _timestamp_macro = "__TIMESTAMP__";
			string const _stdc_macro = "__STDC__";
			string const _file_macro = "__FILE__";
			string const _defined = "defined";
			string const _va_args_macro = "__VA_ARGS__";
			string const _pragma_operator = "_Pragma";
			string const _months [] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
			string const _week_days [] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
		}

		void replace_macro(vector <token> & source, int & i, vector <pair<pair<string,string>,string>> symbol_table, string & _file_name, int & _line_number) {
			time_t t = time(0);
			struct tm* now = localtime(&t);
			int hour = now->tm_hour;
			int min = now->tm_min;
			int sec = now->tm_sec;
			int mday = now->tm_mday;
			int mon = now->tm_mon;
			int year = now->tm_year;
			time_t rawtime;
			tm * timeinfo;
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			int wday = timeinfo->tm_wday;
			for(int j = symbol_table.size() - 1; j >= 0; j--) {
				string now = symbol_table[j].first.first;
				if(source[i].get_id() == token::IDENTIFIER && source[i].get_qualifier() == _time_macro) {
					source.erase(source.begin() + i, source.begin() + i + 1);
					source.insert(source.begin() + i, token("\"" + ((hour < 10) ? "0" + itos(hour) : itos(hour)) + ":" + ((min < 10) ? "0" + itos(min) : itos(min)) + ":" + ((sec < 10) ? "0" + itos(sec) : itos(sec)) + "\""));
					break;
				}
				else if(source[i].get_id() == token::IDENTIFIER && source[i].get_qualifier() == _date_macro) {
					source.erase(source.begin() + i, source.begin() + i + 1);
					source.insert(source.begin() + i, token("\"" + itos(mon + 1) + " " + itos(mday) + " " + itos(year + 1900) + "\""));
					break;
				}
				else if(source[i].get_id() == token::IDENTIFIER && source[i].get_qualifier() == _line_macro) {
					source.erase(source.begin() + i, source.begin() + i + 1);
					source.insert(source.begin() + i, token(itos(_line_number)));
					break;
				}
				else if(source[i].get_id() == token::IDENTIFIER && source[i].get_qualifier() == _file_macro) {
					source.erase(source.begin() + i, source.begin() + i + 1);
					source.insert(source.begin() + i, token("\"" + _file_name + "\""));
					break;
				}
				else if(source[i].get_id() == token::IDENTIFIER && source[i].get_qualifier() == _stdc_macro) {
					source.erase(source.begin() + i, source.begin() + i + 1);
					source.insert(source.begin() + i, token(itos(1)));
					break;
				}
				else if(source[i].get_id() == token::IDENTIFIER && source[i].get_qualifier() == _timestamp_macro) {
					source.erase(source.begin() + i, source.begin() + i + 1);
					source.insert(source.begin() + i, token("\"" + _week_days[wday] + " " + _months[mon] + " " + ((mday < 10) ? "0" + itos(mday) : itos(mday)) + " " + ((hour < 10) ? "0" + itos(hour) : itos(hour)) + ":" + ((min < 10) ? "0" + itos(min) : itos(min)) + ":" + ((sec < 10) ? "0" + itos(sec) : itos(sec)) + " " + itos(year + 1900) + "\""));
					break;
				}
				if(source[i].get_qualifier() == now) {
					int index = i;
					//macros with no parameters.
					if(symbol_table[j].second == "") {
						source.erase(source.begin() + i,source.begin() + i + 1);
						vector<token> temp = lex(symbol_table[j].first.second);
						for(int i = 0; i < temp.size(); i++) {
							if(temp[i].get_id() == token::PASTE &&(i == 0 || i == temp.size() - 1)) {
								cerr << '\n' << '\n';
								msg(_file_name, _line_number, "Error: expected two tokens to concatenate.\nRegion given here for reference:");
								cerr << '\n' << '\n' << '\t';
								for(int j = i; j < temp.size(); j++)
									cerr << temp[i].get_qualifier() + _space;
								caret(1);
								exit(1);
								break;
							}
							else if(temp[i].get_id() == token::PASTE) {
								string b = temp[i - 1].get_qualifier();
								string a = temp[i + 1].get_qualifier();
								if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) b += "ull";
								else if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) b += "ll";
								else if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) b += "l";
								else if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) b += "f";
								else if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) b += "d";
								else if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) b += "u";
								if(temp[i - 1].get_id() == token::WQUOTE || temp[i - 1].get_id() == token::WCHARACTER) b.insert(0, "L");
								if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) a += "ull";
								else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) a += "ll";
								else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) a += "l";
								else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) a += "d";
								else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) a += "f";
								else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) a += "u";
								if(temp[i + 1].get_id() == token::WQUOTE || temp[i + 1].get_id() == token::WCHARACTER) a.insert(0, "L");
								vector<token> tokl;
								vector<token> vb = lex(b), va = lex(a);
								for(int i = 0; i < vb.size(); i++)
									replace_macro(vb, i, symbol_table, _file_name, _line_number);
								for(int i = 0; i < va.size(); i++)
									replace_macro(va, i, symbol_table, _file_name, _line_number);
								tokl.insert(tokl.end(), vb.begin(), vb.end());
								tokl.insert(tokl.end(), va.begin(), va.end());
								a.clear();
								for(int i = 0; i < tokl.size(); i++) {
									if(tokl[i].get_id() == token::WQUOTE || tokl[i].get_id() == token::WCHARACTER)
										a += "L";
									a += tokl[i].get_qualifier();
									if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) a += "ull";
									else if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) a += "ll";
									else if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) a += "l";
									else if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) a += "d";
									else if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) a += "f";
									else if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) a += "u";
								}
								tokl.clear();
								tokl = lex(a);
								temp.erase(temp.begin() + i - 1, temp.begin() + i + 2);
								temp.insert(temp.begin() + i - 1, tokl.begin(), tokl.end());
								i--;
							}
							else if(temp[i].get_id() == token::HASH && i == temp.size() - 1) {
								cerr << '\n' << '\n';
								msg(_file_name, _line_number, "Error: expected a token to stringize.\nRegion given here for reference:");
								cerr << '\n' << '\n' << '\t';
								for(int j = i; j < temp.size(); j++)
									cerr << temp[i].get_qualifier() + _space;
								cerr << '\n' << '\n' << '\t';
								caret(1);
								exit(1);
								break;
							}
							else if(temp[i].get_id() == token::HASH) {
								vector<token> va;
								va.push_back(temp[i + 1]);
								for(int i = 0; i < va.size(); i++)
									replace_macro(va, i, symbol_table, _file_name, _line_number);
								string a;
								for(int i = 0; i < va.size(); i++) {
									if(va[i].get_id() == token::WQUOTE || va[i].get_id() == token::WCHARACTER)
										a += "L";
									a += va[i].get_qualifier();
									if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) a += "ull";
									else if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) a += "ll";
									else if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) a += "l";
									else if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) a += "d";
									else if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) a += "f";
									else if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) a += "u";
								}
								if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) a += "ll";
								else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) a += "l";
								else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) a += "ull";
								else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) a += "d";
								else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) a += "f";
								else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) a += "u";
								if(temp[i + 1].get_id() == token::WQUOTE || temp[i + 1].get_id() == token::WCHARACTER) a += "L" + temp[i + 1].get_qualifier();
								for(int i = 0; i < a.length(); i++) {
									if(a[i] == '\"') {
										a.erase(i,1);
										a.insert(i,"\\\"");
										i += 2;
									}
								}
								string s("\"");
								a.insert(a.begin(),s.begin(), s.end());
								a += "\"";
								vector<token> tokl = lex(a);
								temp.erase(temp.begin() + i, temp.begin() + i + 2);
								temp.insert(temp.begin() + i, tokl.begin(), tokl.end());
							}
						}
						source.insert(source.begin() + i,temp.begin(),temp.end());
						i--;
						break;
					}
					else {
						int k = i;
						string pars = symbol_table[j].second;
						bool zero = false;
						if(pars == "-") {
							zero = true;
						}
						if(source[k + 1].get_id() == token::OPEN_PARENTHESIS) {
							vector<token> save;
							save.push_back(source[k]);
							save.push_back(token("("));
							string actual;
							int balanced = 1;
							k += 2;
							while(balanced != 0) {
								if(source[k].get_id() == token::OPEN_PARENTHESIS)
									balanced++;
								else if(source[k].get_id() == token::CLOSE_PARENTHESIS)
									balanced--;
								if(balanced == 0) break;
								else if(source[k].get_id() == token::WQUOTE || source[k].get_id() == token::WCHARACTER) {
									actual += "L" + source[k].get_qualifier() + _space;
								}
								else if(source[k].get_id() == token::IDENTIFIER && source[k + 1].get_id() == token::IDENTIFIER)
									actual += source[k].get_qualifier() + _space;
								else {
									actual += source[k].get_qualifier();
									if(source[k].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) actual += "l";
									else if(source[k].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) actual += "ll";
									else if(source[k].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) actual += "ull";
									else if(source[k].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) actual += "d";
									else if(source[k].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) actual += "u";
									else if(source[k].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) actual += "f";
								}
								if(k >= source.size()) {
									msg(_file_name, _line_number, "Error: unmatched parenthesis in the unexpanded macro.");
									int _caret = 0;
									cerr << '\n' << '\n' << '\t';
									for(int i = 0; i < save.size() && save[i].get_id() != token::NEW_LINE; i++) {
										if(save[i].get_qualifier() == "...") {
											cerr << "__VA_ARGS__ ";
											_caret += 13;
										}
										else if(save[i].get_qualifier() == "__VA_ARGS__") {
											cerr << "... ";
											_caret += 4;
										}
										else {
											cerr << save[i].get_qualifier() << ' ';
											_caret += save[i].get_qualifier().length() + 1;
										}
									}
									cerr << '\n' << '\n' << '\t';
									_caret -= 2;
									caret(_caret);
									exit(1);
								}
								save.push_back(source[k]);
								k++;
							}
							//count the number of actual macro parameters,(excluding inner parameters)
							int comma = 0;
							bool e = false;
							vector<token> l = lex(actual);
							int lcount = l.size();
							int lbal = 0;
							for(int i = 0; i < l.size(); i++) {
								if(l[i].get_id() == token::OPEN_PARENTHESIS)
									lbal++;
								else if(l[i].get_id() == token::CLOSE_PARENTHESIS)
									lbal--;
								else if(l[i].get_id() == token::COMMA && lbal == 0)
									comma++;
							}
							//count the theoretical number of parameters
							int comp_comma = 0;
							int va = 0;
							string theoretical;
							if(zero) theoretical = "";
							else theoretical = symbol_table[j].second;
							l = lex(theoretical);
							save.clear();
							save.insert(save.begin(), token("("));
							vector<token> mn = lex(symbol_table[j].first.first);
							save.insert(save.begin(), mn.begin(), mn.end());
							vector<token> mp = lex(symbol_table[j].second);
							save.insert(save.end(), mp.begin(), mp.end());
							save.push_back(token(")"));
							vector<token> mc = lex(symbol_table[j].first.second);
							save.insert(save.end(), mc.begin(), mc.end());
							for(int i = 0; i < l.size(); i++) {
								if(l[i].get_id() == token::COMMA)
									comp_comma++;
								if(l[i].get_qualifier() == _va_args_macro) {
									e = true;
									va++;
								}
							}
							//count the number of variable arguments
							if(va > 1) {
								msg(_file_name, _line_number, "Error: multiple '...' variable arguments encountered in macro definition.\nMacro definition given here for reference: ");
								int _caret = 0;
								cerr << '\n' << '\n' << '\t';
								int cinc = 0;
								for(int i = 0; i < save.size() && save[i].get_id() != token::NEW_LINE; i++) {
									if(save[i].get_qualifier() == "__VA_ARGS__")
										cerr << "... ";
									else
										cerr << save[i].get_qualifier() << ' ';
									if(save[i].get_qualifier() == "__VA_ARGS__")
										cinc++;
									if(cinc >= 2);
									else {
										if(save[i].get_qualifier() == "__VA_ARGS__")
											_caret += 4;
										else
											_caret += save[i].get_qualifier().length() + 1;
									}
								}
								cerr << '\n' << '\n' << '\t';
								caret(_caret);
								exit(1);
							}
							if(((zero && lcount != 0) ||(lcount == 0 && !zero)) && !(e && !(comma == 0 && comp_comma == 1) && comma >= comp_comma - 1));
							else if(comma == comp_comma && !e) {
								//zero parameter macros, parenthesis with no parameters
								if(zero) {
									source.erase(source.begin() + index,source.begin() + k + 1);
									string code = symbol_table[j].first.second;
									vector<token> temp = lex(symbol_table[j].first.second);
									for(int i = 0; i < temp.size(); i++) {
										if(temp[i].get_id() == token::PASTE &&(i == 0 || i == temp.size() - 1)) {
											cerr << '\n' << '\n';
											msg(_file_name, _line_number, "Error: expected two tokens to concatenate.\nRegion given here for reference:");
											cerr << '\n' << '\n' << '\t';
											for(int j = i; j < temp.size(); j++)
												cerr << temp[i].get_qualifier() + _space;
											caret(1);
											exit(1);
											break;
										}
										else if(temp[i].get_id() == token::PASTE) {
											string b = temp[i - 1].get_qualifier();
											string a = temp[i + 1].get_qualifier();
											if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) b += "ull";
											else if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) b += "ll";
											else if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) b += "l";
											else if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) b += "f";
											else if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) b += "d";
											else if(temp[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) b += "u";
											if(temp[i - 1].get_id() == token::WQUOTE || temp[i - 1].get_id() == token::WCHARACTER) b.insert(0, "L");
											if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) a += "ull";
											else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) a += "ll";
											else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) a += "l";
											else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) a += "d";
											else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) a += "f";
											else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) a += "u";
											if(temp[i + 1].get_id() == token::WQUOTE || temp[i + 1].get_id() == token::WCHARACTER) a.insert(0, "L");
											vector<token> tokl;
											vector<token> vb = lex(b), va = lex(a);
											for(int i = 0; i < vb.size(); i++)
												replace_macro(vb, i, symbol_table, _file_name, _line_number);
											for(int i = 0; i < va.size(); i++)
												replace_macro(va, i, symbol_table, _file_name, _line_number);
											tokl.insert(tokl.end(), vb.begin(), vb.end());
											tokl.insert(tokl.end(), va.begin(), va.end());
											a.clear();
											for(int i = 0; i < tokl.size(); i++) {
												if(tokl[i].get_id() == token::WQUOTE || tokl[i].get_id() == token::WCHARACTER)
													a += "L";
												a += tokl[i].get_qualifier();
												if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) a += "ull";
												else if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) a += "ll";
												else if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) a += "l";
												else if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) a += "d";
												else if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) a += "f";
												else if(tokl[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) a += "u";
											}
											tokl.clear();
											tokl = lex(a);
											temp.erase(temp.begin() + i - 1, temp.begin() + i + 2);
											temp.insert(temp.begin() + i - 1, tokl.begin(), tokl.end());
											i--;
										}
										else if(temp[i].get_id() == token::HASH && i == temp.size() - 1) {
											cerr << '\n' << '\n';
											msg(_file_name, _line_number, "Error: expected a token to stringize.\nRegion given here for reference:");
											cerr << '\n' << '\n' << '\t';
											for(int j = i; j < temp.size(); j++)
												cerr << temp[i].get_qualifier() + _space;
											cerr << '\n' << '\n' << '\t';
											caret(1);
											exit(1);
											break;
										}
										else if(temp[i].get_id() == token::HASH) {
											vector<token> va;
											va.push_back(temp[i + 1]);
											for(int i = 0; i < va.size(); i++)
												replace_macro(va, i, symbol_table, _file_name, _line_number);
											string a;
											for(int i = 0; i < va.size(); i++) {
												if(va[i].get_id() == token::WQUOTE || va[i].get_id() == token::WCHARACTER)
													a += "L";
												a += va[i].get_qualifier();
												if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) a += "ull";
												else if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) a += "ll";
												else if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) a += "l";
												else if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) a += "d";
												else if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) a += "f";
												else if(va[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) a += "u";
											}
											if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) a += "ll";
											else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) a += "l";
											else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) a += "ull";
											else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) a += "d";
											else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) a += "f";
											else if(temp[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) a += "u";
											if(temp[i + 1].get_id() == token::WQUOTE || temp[i + 1].get_id() == token::WCHARACTER) a += "L" + temp[i + 1].get_qualifier();
											for(int i = 0; i < a.length(); i++) {
												if(a[i] == '\"') {
													a.erase(i,1);
													a.insert(i,"\\\"");
													i += 2;
												}
											}
											string s("\"");
											a.insert(a.begin(),s.begin(), s.end());
											a += "\"";
											vector<token> tokl = lex(a);
											temp.erase(temp.begin() + i, temp.begin() + i + 2);
											temp.insert(temp.begin() + i, tokl.begin(), tokl.end());
										}
									}
									source.insert(source.begin() + index,temp.begin(),temp.end());
									i--;
									break;
								}
								vector<string> parameters;
								vector<string> tparameters;
								vector<token> ac =lex(actual);
								string ident;
								int acbal = 0;
								//tokenize the actual parameters
								for(int i = 0; i < ac.size(); i++) {
									if(ac[i].get_id() == token::NEW_LINE) _line_number++;
									else if(ac[i].get_id() == token::OPEN_PARENTHESIS) {
										acbal++;
										ident += ac[i].get_qualifier() + _space;
									}
									else if(ac[i].get_id() == token::CLOSE_PARENTHESIS) {
										acbal--;
										ident += ac[i].get_qualifier() + _space;
									}
									else if(ac[i].get_id() == token::COMMA && acbal == 0) {
										trim(ident);
										parameters.push_back(ident);
										ident.clear();
									}
									else if(ac[i].wide() == true) {
										ident += "L" + ac[i].get_qualifier() + _space;
									}
									else if(ac[i].get_id() == token::IDENTIFIER && ac[i + 1].get_id() == token::IDENTIFIER)
										ident += ac[i].get_qualifier() + _space;
									else {
										ident += ac[i].get_qualifier();
										if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) ident += "d";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) ident += "f";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) ident += "l";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) ident += "ull";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) ident += "ll";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) ident += "u";
										ident += _space;
									}
								}
								trim(ident);
								parameters.push_back(ident);
								ident.clear();
								ac = lex(theoretical);
								//lex theoretical documents.
								for(int i = 0; i < ac.size(); i++) {
									if(ac[i].get_id() == token::COMMA) {
										trim(ident);
										tparameters.push_back(ident);
										ident.clear();
									}
									else {
										if(ac[i].wide()) ident += "L";
										ident += ac[i].get_qualifier();
										if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) ident += "d";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) ident += "f";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) ident += "l";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) ident += "ull";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) ident += "ll";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) ident += "u";
										ident += _space;
									}
								}
								trim(ident);
								tparameters.push_back(ident);
								ident.clear();
								for(int i = 0; i < tparameters.size(); i++) {
									if(tparameters[i] == "") {
										msg(_file_name,_line_number,"Error: expected a parameter in the formal macro definition in the '#define' preprocessor directive.\nMacro definition given here for reference:");
										int _caret = save[0].get_qualifier().length() + save[1].get_qualifier().length();
										cerr << '\n' << '\n' << '\t';
										int cinc = 0;
										cerr << save[0].get_qualifier() << ' ' << save[1].get_qualifier() << ' ';
										for(int i = 2; i < save.size() && save[i].get_id() != token::NEW_LINE; i++) {
											if(save[i].get_qualifier() == "__VA_ARGS__")
												cerr << "... ";
											else if(save[i].get_qualifier() == "...")
												cerr << "__VA_ARGS__ ";
											else
												cerr << save[i].get_qualifier() << ' ';
											if(cinc > 0);
											else {
												if(save[i].get_qualifier() == "__VA_ARGS__")
													_caret += 4;
												else if(save[i].get_qualifier() == "...")
													_caret += 12;
												else
													_caret += save[i].get_qualifier().length() + 1;
											}
											if(save[i].get_id() == token::COMMA && save[i + 1].get_id() == token::CLOSE_PARENTHESIS) cinc = 1;
										}
										cerr << '\n' << '\n' << '\t';
										caret(_caret);
										exit(1);
									}
								}
								string code = symbol_table[j].first.second;
								vector<token> v = lex(code);
								//surround parameters with quotes
								for(int i = 0; i < parameters.size(); i++) {
									for(int j = 0; j < v.size(); j++) {
										if(v[j].get_qualifier() == _hash && j == v.size() - 1) {
											msg(_file_name, _line_number, "Error: expected an initial token to concatenate.");
											int _caret = save[0].get_qualifier().length() + save[1].get_qualifier().length();
											cerr << '\n' << '\n' << '\t';
											int cinc = 0;
											cerr << save[0].get_qualifier() << ' ' << save[1].get_qualifier() << ' ';
											for(int i = 2; i < save.size() && save[i].get_id() != token::NEW_LINE; i++) {
												if(save[i].get_qualifier() == "__VA_ARGS__")
													cerr << "... ";
												else if(save[i].get_qualifier() == "...")
													cerr << "__VA_ARGS__ ";
												else
													cerr << save[i].get_qualifier() << ' ';
												if(cinc > 0);
												else {
													if(save[i].get_qualifier() == "__VA_ARGS__")
														_caret += 4;
													else if(save[i].get_qualifier() == "...")
														_caret += 12;
													else
														_caret += save[i].get_qualifier().length() + 1;
												}
											}
											cerr << '\n' << '\n' << '\t';
											caret(_caret);
											exit(1);
										}
										if(v[j].get_qualifier() == _hash && v[j + 1].get_qualifier() == tparameters[i]) {
											string save = parameters[i];
											vector<token> vec = lex(save);
											for(int i = 0; i < vec.size(); i++)
												replace_macro(vec, i, symbol_table, _file_name, _line_number);
											save.clear();
											for(int i = 0; i < vec.size(); i++) {
												if(vec[i].wide() == true)
													save.insert(save.length(), "L" + vec[i].get_qualifier() + _space);
												else {
													if(vec[i].get_id() == token::IDENTIFIER && vec[i + 1].get_id() == token::IDENTIFIER)
														save.insert(save.length(), vec[i].get_qualifier() + _space);
													else {
														save.insert(save.length(), vec[i].get_qualifier());
														if(vec[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) save += "d";
														else if(vec[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) save += "u";
														else if(vec[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) save += "f";
														else if(vec[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) save += "l";
														else if(vec[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) save += "ull";
														else if(vec[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) save += "ll";
													}
												}
											}
											trim(save);
											for(int i = 0; i < save.length(); i++) {
												if(save[i] == '\"') {
													save.erase(i, 1);
													save.insert(i, "\\\"");
													i++;
												}
											}
											v.erase(v.begin() + j, v.begin() + j + 2);
											v.insert(v.begin() + j, token(_double_quote + save + _double_quote));
										}
										else if(v[j].get_id() == token::HASH) {
											string save = v[j + 1].get_qualifier();
											if(v[j + 1].wide()) save.insert(0, "L");
											if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) save += "d";
											else if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) save += "u";
											else if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) save += "f";
											else if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) save += "l";
											else if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) save += "ull";
											else if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) save += "ll";
											for(int i = 0; i < save.length(); i++) {
												if(save[i] == '\"') {
													save.erase(i, 1);
													save.insert(i, "\\\"");
													i++;
												}
											}
											v.erase(v.begin() + j, v.begin() + j + 2);
											v.insert(v.begin() + j, token(_double_quote + save + _double_quote));
										}
										else if(v[j].get_qualifier() == tparameters[i]) {
											v.erase(v.begin() + j,v.begin() + j + 1);
											v.insert(v.begin() + j,parameters[i]);
										}
									}
								}
								//concatenate two parameters
								for(int i = 0; i < v.size(); i++) {
									if((i == 0 || i == v.size() - 1) && v[i].get_id() == token::PASTE) {
										msg(_file_name,_line_number,"Error: expected an initial token to concatenate.");
										int _caret = save[0].get_qualifier().length() + save[1].get_qualifier().length();
										cerr << '\n' << '\n' << '\t';
										int cinc = 0;
										cerr << save[0].get_qualifier() << ' ' << save[1].get_qualifier() << ' ';
										for(int i = 2; i < save.size() && save[i].get_id() != token::NEW_LINE; i++) {
											if(save[i].get_qualifier() == "__VA_ARGS__")
												cerr << "... ";
											else if(save[i].get_qualifier() == "...")
												cerr << "__VA_ARGS__ ";
											else
												cerr << save[i].get_qualifier() << ' ';
											if(cinc > 0);
											else {
												if(save[i].get_qualifier() == "__VA_ARGS__")
													_caret += 4;
												else if(save[i].get_qualifier() == "...")
													_caret += 12;
												else
													_caret += save[i].get_qualifier().length() + 1;
											}
											if(save[i].get_id() == token::PASTE) cinc = 1;
										}
										cerr << '\n' << '\n' << '\t';
										_caret -= 1;
										caret(_caret);
										exit(1);
									}
									if(i != 0 && i != v.size() - 1 && v[i].get_id() == token::PASTE) {
										//resolve macro parameters before concatenation
										string sprev = v[i - 1].get_qualifier();
										if(v[i - 1].wide()) sprev.insert(0, "L");
										if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) sprev += "l";
										else if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) sprev += "u";
										else if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) sprev += "ll";
										else if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) sprev += "ull";
										else if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) sprev += "d";
										else if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) sprev += "f";
										vector<token> prev = lex(sprev);
										string snext = v[i + 1].get_qualifier();
										if(v[i + 1].wide()) snext.insert(0, "L");
										if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) snext += "l";
										else if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) snext += "u";
										else if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) snext += "ll";
										else if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) snext += "ull";
										else if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) snext += "d";
										else if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) snext += "f";
										vector<token> next = lex(snext);
										for(int i = 0; i < prev.size(); i++)
											replace_macro(prev,i,symbol_table,_file_name,_line_number);
										for(int i = 0; i < next.size(); i++)
											replace_macro(next,i,symbol_table,_file_name,_line_number);
										string ins;
										for(int i = 0; i < prev.size(); i++) {
											if(prev[i].wide() == true)
												ins.insert(ins.length(), "L" + prev[i].get_qualifier());
											else {
												ins += prev[i].get_qualifier();
												if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) ins += "l";
												else if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) ins += "u";
												else if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) ins += "ll";
												else if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) ins += "ull";
												else if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) ins += "d";
												else if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) ins += "f";
											}
										}
										for(int i = 0; i < next.size(); i++) {
											if(next[i].wide() == true)
												ins.insert(ins.length(), "L" + next[i].get_qualifier());
											else {
												ins += next[i].get_qualifier();
												if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) ins += "l";
												else if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) ins += "u";
												else if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) ins += "ll";
												else if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) ins += "ull";
												else if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) ins += "d";
												else if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) ins += "f";
											}
										}
										vector<token> l = lex(ins);
										v.erase(v.begin() + i - 1,v.begin() + i + 2);
										v.insert(v.begin() + i - 1,l.begin(),l.end());
										i--;
									}
								}
								code.clear();
								for(int i = 0; i < v.size(); i++) {
									if(v[i].wide() == true)
										code.insert(code.length(), "L" + v[i].get_qualifier() + _space);
									else {
										code += v[i].get_qualifier();
										if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) code += "l";
										else if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) code += "u";
										else if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) code += "d";
										else if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) code += "f";
										else if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) code += "ull";
										else if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) code += "ll";
										code += _space;
									}
								}
								trim(code);
								vector<token> _lexed = lex(code);
								source.erase(source.begin() + index,source.begin() + k + 1);
								source.insert(source.begin() + index,_lexed.begin(),_lexed.end());
								i--;
							}
							else if(e && !(comma == 1 && comp_comma == 0) && comma >= comp_comma - 1) {
								vector<string> parameters;
								vector<string> tparameters;
								vector<token> ac = lex(actual);
								string ident;
								int acbal = 0;
								for(int i = 0; i < ac.size(); i++) {
									if(ac[i].get_id() == token::NEW_LINE) _line_number++;
									else if(ac[i].get_id() == token::OPEN_PARENTHESIS) {
										acbal++;
										ident += ac[i].get_qualifier() + _space;
									}
									else if(ac[i].get_id() == token::CLOSE_PARENTHESIS) {
										acbal--;
										ident += ac[i].get_qualifier() + _space;
									}
									else if(ac[i].get_id() == token::COMMA && acbal == 0) {
										trim(ident);
										parameters.push_back(ident);
										ident.clear();
									}
									else if(ac[i].wide() == true) {
										ident += "L" + ac[i].get_qualifier() + _space;
									}
									else {
										if(ac[i].get_id() == token::IDENTIFIER && ac[i + 1].get_id() == token::IDENTIFIER)
											ident += ac[i].get_qualifier() + _space;
										else {
											ident += ac[i].get_qualifier();
											if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) ident += "d";
											else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) ident += "u";
											else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) ident += "f";
											else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) ident += "l";
											else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) ident += "ull";
											else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) ident += "ll";
											ident += _space;
										}
									}
								}
								trim(ident);
								parameters.push_back(ident);
								ident.clear();
								ac = lex(theoretical);
								for(int i = 0; i < ac.size(); i++) {
									if(ac[i].get_id() == token::COMMA) {
										trim(ident);
										tparameters.push_back(ident);
										ident.clear();
									}
									else {
										if(ac[i].wide()) ident += "L";
										ident += ac[i].get_qualifier();
										if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) ident += "d";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) ident += "u";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) ident += "f";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) ident += "l";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) ident += "ull";
										else if(ac[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) ident += "ll";
										ident += _space;
									}
								}
								trim(ident);
								tparameters.push_back(ident);
								if(tparameters.size() - 1 == parameters.size()) parameters.push_back("");
								ident.clear();
								for(int i = 0; i < tparameters.size(); i++) {
									if(tparameters[i] == "") {
										msg(_file_name, _line_number, "Error: expected a parameter in the formal macro definition in the '#define' preprocessor directive.\nMacro definition given here for reference:");
										int _caret = save[0].get_qualifier().length() + save[1].get_qualifier().length();
										cerr << '\n' << '\n' << '\t';
										int cinc = 0;
										cerr << save[0].get_qualifier() << ' ' << save[1].get_qualifier() << ' ';
										for(int i = 2; i < save.size() && save[i].get_id() != token::NEW_LINE; i++) {
											if(save[i].get_qualifier() == "__VA_ARGS__")
												cerr << "... ";
											else if(save[i].get_qualifier() == "...")
												cerr << "__VA_ARGS__ ";
											else
												cerr << save[i].get_qualifier() << ' ';
											if(cinc > 0);
											else {
												if(save[i].get_qualifier() == "__VA_ARGS__")
													_caret += 4;
												else if(save[i].get_qualifier() == "...")
													_caret += 12;
												else
													_caret += save[i].get_qualifier().length() + 1;
											}
											if(save[i].get_id() == token::COMMA && save[i + 1].get_id() == token::CLOSE_PARENTHESIS) cinc = 1;
										}
										cerr << '\n' << '\n' << '\t';
										caret(_caret);
										exit(1);
									}
								}
								//get rest of macro parameters for variable arguments
								string code = symbol_table[j].first.second;
								for(int i = 0; i < tparameters.size(); i++) {
									if(tparameters[i] == _va_args_macro) {
										string params;
										int index = i;
										while(i < parameters.size()) {
											params.insert(params.length(),parameters[i] + _comma);
											i++;
										}
										params.erase(params.length() - 1);
										parameters.erase(parameters.begin() + index,parameters.end());
										parameters.insert(parameters.begin() + index,params);
										break;
									}
									else {
									}
								}
								vector<token> v = lex(code);
								for(int i = 0; i < parameters.size(); i++) {
									for(int j = 0; j < v.size(); j++) {
										if(v[j].get_qualifier() == _hash && j == v.size() - 1) {
											msg(_file_name, _line_number, "Error: expected an initial token to concatenate.");
											int _caret = save[0].get_qualifier().length() + save[1].get_qualifier().length();
											cerr << '\n' << '\n' << '\t';
											int cinc = 0;
											cerr << save[0].get_qualifier() << ' ' << save[1].get_qualifier() << ' ';
											for(int i = 2; i < save.size() && save[i].get_id() != token::NEW_LINE; i++) {
												if(save[i].get_qualifier() == "__VA_ARGS__")
													cerr << "... ";
												else if(save[i].get_qualifier() == "...")
													cerr << "__VA_ARGS__ ";
												else
													cerr << save[i].get_qualifier() << ' ';
												if(cinc > 0);
												else {
													if(save[i].get_qualifier() == "__VA_ARGS__")
														_caret += 4;
													else if(save[i].get_qualifier() == "...")
														_caret += 12;
													else
														_caret += save[i].get_qualifier().length() + 1;
												}
											}
											cerr << '\n' << '\n' << '\t';
											caret(_caret);
											exit(1);
										}
										if(v[j].get_qualifier() == _hash && v[j + 1].get_qualifier() == tparameters[i]) {
											string save = parameters[i];
											vector<token> vec = lex(save);
											for(int i = 0; i < vec.size(); i++)
												replace_macro(vec, i, symbol_table, _file_name, _line_number);
											save.clear();
											for(int i = 0; i < vec.size(); i++) {
												if(vec[i].wide() == true)
													save.insert(save.length(), "L" + vec[i].get_qualifier() + _space);
												else {
													if(vec[i].get_id() == token::IDENTIFIER && vec[i + 1].get_id() == token::IDENTIFIER)
														save.insert(save.length(), vec[i].get_qualifier() + _space);
													else
														save.insert(save.length(), vec[i].get_qualifier());
												}
											}
											trim(save);
											for(int i = 0; i < save.length(); i++) {
												if(save[i] == '\"') {
													save.erase(i, 1);
													save.insert(i, "\\\"");
													i++;
												}
											}
											v.erase(v.begin() + j, v.begin() + j + 2);
											v.insert(v.begin() + j, token(_double_quote + save + _double_quote));
										}
										else if(v[j].get_id() == token::HASH) {
											string save = v[j + 1].get_qualifier();
											if(v[j + 1].wide()) save.insert(0, "L");
											if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) save += "d";
											else if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) save += "u";
											else if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) save += "f";
											else if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) save += "l";
											else if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) save += "ull";
											else if(v[j + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) save += "ll";
											for(int i = 0; i < save.length(); i++) {
												if(save[i] == '\"') {
													save.erase(i, 1);
													save.insert(i, "\\\"");
													i++;
												}
											}
											v.erase(v.begin() + j, v.begin() + j + 2);
											v.insert(v.begin() + j, token(_double_quote + save + _double_quote));
										}
										else if(v[j].get_qualifier() == tparameters[i]) {
											v.erase(v.begin() + j,v.begin() + j + 1);
											v.insert(v.begin() + j,parameters[i]);
										}
									}
								}
								for(int i = 0; i < v.size(); i++) {
									if((i == 0 || i == v.size() - 1) && v[i].get_id() == token::PASTE) {
										msg(_file_name, _line_number, "Error: expected an initial token to concatenate.");
										int _caret = save[0].get_qualifier().length() + save[1].get_qualifier().length();
										cerr << '\n' << '\n' << '\t';
										int cinc = 0;
										cerr << save[0].get_qualifier() << ' ' << save[1].get_qualifier() << ' ';
										for(int i = 2; i < save.size() && save[i].get_id() != token::NEW_LINE; i++) {
											if(save[i].get_qualifier() == "__VA_ARGS__")
												cerr << "... ";
											else if(save[i].get_qualifier() == "...")
												cerr << "__VA_ARGS__ ";
											else
												cerr << save[i].get_qualifier() << ' ';
											if(cinc > 0);
											else {
												if(save[i].get_qualifier() == "__VA_ARGS__")
													_caret += 4;
												else if(save[i].get_qualifier() == "...")
													_caret += 12;
												else
													_caret += save[i].get_qualifier().length() + 1;
											}
											if(save[i].get_id() == token::PASTE) cinc = 1;
										}
										cerr << '\n' << '\n' << '\t';
										_caret -= 1;
										caret(_caret);
										exit(1);
									}
									if(i != 0 && i != v.size() - 1 && v[i].get_id() == token::PASTE) {
										string sprev = v[i - 1].get_qualifier();
										if(v[i - 1].wide()) sprev.insert(0, "L");
										if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) sprev += "l";
										else if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) sprev += "u";
										else if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) sprev += "ll";
										else if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) sprev += "ull";
										else if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) sprev += "d";
										else if(v[i - 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) sprev += "f";
										vector<token> prev = lex(sprev);
										string snext = v[i + 1].get_qualifier();
										if(v[i + 1].wide()) snext.insert(0, "L");
										if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) snext += "l";
										else if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) snext += "u";
										else if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) snext += "ll";
										else if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) snext += "ull";
										else if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) snext += "d";
										else if(v[i + 1].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) snext += "f";
										vector<token> next = lex(snext);
										for(int i = 0; i < prev.size(); i++)
											replace_macro(prev,i,symbol_table,_file_name,_line_number);
										for(int i = 0; i < next.size(); i++)
											replace_macro(next,i,symbol_table,_file_name,_line_number);
										string ins;
										for(int i = 0; i < prev.size(); i++) {
											if(prev[i].wide() == true)
												ins.insert(ins.length(), "L" + prev[i].get_qualifier());
											else {
												ins += prev[i].get_qualifier();
												if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) ins += "l";
												else if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) ins += "u";
												else if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) ins += "ll";
												else if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) ins += "ull";
												else if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) ins += "d";
												else if(prev[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) ins += "f";
											}
										}
										for(int i = 0; i < next.size(); i++) {
											if(next[i].wide() == true)
												ins.insert(ins.length(), "L" + next[i].get_qualifier());
											else {
												ins += next[i].get_qualifier();
												if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) ins += "l";
												else if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) ins += "u";
												else if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) ins += "ll";
												else if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) ins += "ull";
												else if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) ins += "d";
												else if(next[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) ins += "f";
											}
										}
										vector<token> l = lex(ins);
										v.erase(v.begin() + i - 1,v.begin() + i + 2);
										v.insert(v.begin() + i - 1,l.begin(),l.end());
										i--;
									}
								}
								code.clear();
								for(int i = 0; i < v.size(); i++) {
									if(v[i].wide() == true)
										code.insert(code.length(), "L" + v[i].get_qualifier() + _space);
									else {
										code += v[i].get_qualifier();
										if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) code += "ull";
										else if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) code += "ll";
										else if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) code += "l";
										else if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) code += "d";
										else if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) code += "f";
										else if(v[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) code += "u";
										code += _space;
									}
								}
								trim(code);
								vector<token> _lexed = lex(code);
								source.erase(source.begin() + index,source.begin() + k + 1);
								source.insert(source.begin() + index,_lexed.begin(),_lexed.end());
								i--;
							}
							else {
							}
						}
						else {
						}
						break;
					}
				}
			}
		}

		vector <cpp_token> replace_all_defines(vector <token> & source, string _file) {
			vector<pair<pair<string,string>,string>> symbol_table;
			symbol_table.push_back(make_pair(make_pair(_file_macro,""),""));
			symbol_table.push_back(make_pair(make_pair(_line_macro,""),""));
			symbol_table.push_back(make_pair(make_pair(_stdc_macro,"1"),""));
			symbol_table.push_back(make_pair(make_pair(_time_macro,""),""));
			symbol_table.push_back(make_pair(make_pair(_date_macro,""),""));
			symbol_table.push_back(make_pair(make_pair(_timestamp_macro,""),""));
			vector<token> copy = source;
			time_t t = time(0);
			struct tm* now = localtime(&t);
			int hour = now->tm_hour;
			int min = now->tm_min;
			int sec = now->tm_sec;
			int mday = now->tm_mday;
			int mon = now->tm_mon;
			int year = now->tm_year;
			time_t rawtime;
			tm * timeinfo;
			time(&rawtime);
			timeinfo=localtime(&rawtime);
			int wday=timeinfo->tm_wday;
			int _line_number = 1;
			string _file_name = _file;
#if defined _WIN32 || defined __MINGW32__
			string inc_dir = "C:\\include\\";
#else
			string inc_dir = "/home/include/";
#endif
			vector<string> onces;
			vector<cpp_token> ret;
			int rindex = 0;
			source.push_back(token("\n"));
			for(int i = 0; i < source.size(); i ++) {
				if(source[i].get_id() == token::NEW_LINE) {
					vector<cpp_token> ins;
					for(int j = rindex; j < i; j++) {
						if(source[j].get_id() == token::NEW_LINE) {
							ins.push_back(cpp_token(_file_name, _line_number, source[j]));
							_line_number++;
						}
						else
							ins.push_back(cpp_token(_file_name, _line_number, source[j]));
					}
					ins.push_back(cpp_token(_file_name, _line_number, token("\n")));
					ret.insert(ret.end(), ins.begin(), ins.end());
					_line_number++;
					rindex = i + 1;
				}
				else if(source[i].get_id() == token::HLINE) {
					int savefl = i;
					for(int i = savefl + 1; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
						string s = source[i].get_qualifier();
						if(s == _date_macro || s == _time_macro || s == _timestamp_macro || s == _stdc_macro || s == _herror ||
								s == _hdefine || s == _hundef || s == _hif || s == _helif || s == _helse || s == _hendif || s == _hinclude || s == _hifdef ||
								s == _hifndef || s == _hpragma || s == _pragma_operator || s == _defined) {
							msg(_file_name, _line_number, "Error: cannot redefine '" + s + "' statement inside '#line' preprocessor directive.");
							cerr << '\n' << '\n' << '\t';
							for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hline.length() + 1);
							exit(1);
							break;
						}
					}
					i = savefl;
					int index = i;
					while(source[i].get_id() != token::NEW_LINE && i != source.size()) {
						i++;
					}
					for(int j = index; source[j].get_id() != token::NEW_LINE; j++) {
						replace_macro(source, j, symbol_table, _file_name,_line_number);
					}
					string expr;
					int j = index + 1;
					while((source[j].get_id() == token::INTEGER || source[j].get_id() == token::PLUS || source[j].get_id() == token::MINUS || source[j].get_id() == token::STAR ||
								source[j].get_id() == token::DIVIDE || source[j].get_id() == token::MODULUS || source[j].get_id() == token::OPEN_PARENTHESIS || source[j].get_id() == token::CLOSE_PARENTHESIS) && j != source.size()) {
						expr += source[j].get_qualifier();
						j++;
					}
					__karma::__util::expr e(_line_number, _file_name, "#line");
					e.set_expr(expr);
					string eval = e.use_notation();
					if(e.get_error_counter() > 0) {
						msg(_file_name,_line_number, string("Error: ill-formed expression in preprocessor directive '#line' with the expansion being:\t'" + expr + ".'\nLine given here for reference:"));
						cerr << '\n' << '\n' << '\t';
						for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
							cerr << source[i].get_qualifier() << ' ';
						}
						cerr << '\n' << '\n' << '\t';
						caret(6);
						_line_number = -1;
						exit(1);
					}
					else {
						_line_number = _stoi(eval) - 1;
					}
					if(source[j].get_id() != token::NEW_LINE && source[j].get_id() != token::QUOTE) {
						msg(_file_name, _line_number, "Error: expected a 'NEW_LINE' character or a file-name-specifier(no wide-character constants) after the integral line-number-specifier.");
						int _caret = 0;
						cerr << '\n' << '\n' << '\t';
						for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
							cerr << source[i].get_qualifier() << ' ';
							if(i == source.size() - 1);
							else
								_caret += source[i].get_qualifier().length() + 1;
						}
						cerr << '\n' << '\n' << '\t';
						_caret -= 2;
						caret(_caret);
						exit(1);
					}
					else {
						if(source[j].get_id() == token::NEW_LINE)
							while(source[index].get_id() != token::NEW_LINE)
								source.erase(source.begin() + index, source.begin() + index + 1);
						else {
							string vec = source[j].get_qualifier();
							int k = j + 1;
							while(source[k].get_id() != token::NEW_LINE && k != source.size()) {
								if(source[k].get_id() == token::QUOTE) {
									vec.erase(vec.length() - 1, vec.length());
									vec += source[k].get_qualifier().substr(1, source[k].get_qualifier().length());
								}
								else {
									msg(_file_name, _line_number, "Error: junk token '" + source[k].get_qualifier() + ",' encountered after the '#line' preprocessor directive.");
									int _caret = 0;
									cerr << '\n' << '\n' << '\t';
									for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
										cerr << source[i].get_qualifier() << ' ';
										if(i == source.size() - 1);
										else
											_caret += source[i].get_qualifier().length() + 1;
									}
									cerr << '\n' << '\n' << '\t';
									_caret -= 2;
									caret(_caret);
									exit(1);
								}
								k++;
							}
							_file_name = vec.substr(1, vec.length() - 2);
							while(source[index].get_id() != token::NEW_LINE)
								source.erase(source.begin() + index, source.begin() + index + 1);
						}
					}
					i = index - 1;
				}
				else if(source[i].get_id() == token::HWARNING) {
					string view;
					int index = i;
					i++;
					while(source[i].get_id() != token::NEW_LINE && i != source.size()) {
						view += source[i].get_qualifier() + _space;
						i++;
					}
					source.erase(source.begin() + index, source.begin() + i);
					warn(_file_name, _line_number, "Warning: '#warning' preprocessor directive encountered with the following message:\n\n\t#warning " + view + "\n");
					i = index - 1;
				}
				else if(source[i].get_id() == token::HASH) {
					warn(_file_name,_line_number,"Warning: stray '#' in the program; ignoring.\n");
					source.erase(source.begin() + i,source.begin() + i + 1);
					i--;
				}
				else if(source[i].get_id() == token::IDENTIFIER && source[i].get_qualifier() == _pragma_operator) {
					int index = i;
					if(source[i + 1].get_id() == token::OPEN_PARENTHESIS &&(source[i + 2].get_id() == token::QUOTE || source[i + 2].get_id() == token::WQUOTE) &&
							source[i + 3].get_id() == token::CLOSE_PARENTHESIS) {
						string quotes = source[i + 2].get_qualifier();
						quotes.erase(0, 1);
						quotes.erase(quotes.length() - 1, quotes.length());
						trim(quotes);
						if(quotes == "once")
							onces.push_back(_file_name);
						else {
							warn(_file_name, _line_number, "Warning: Unknown statement: '" + source[i + 2].get_qualifier() + ",' encountered after the '_Pragma' preprocessor operator; ignoring.\nLine given here for reference: ");
							cerr << '\n' << '\n' << '\t';
							for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(10);
							cerr << '\n';
						}
						source.erase(source.begin() + i, source.begin() + i + 4);
					}
					else {
						warn(_file_name, _line_number, "Warning: incorrect format for the '_Pragma' preprocessor operator; ignoring.\nLine given here for reference:");
						cerr << '\n' << '\n' << '\t';
						for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
							cerr << source[i].get_qualifier() << ' ';
						}
						cerr << '\n' << '\n' << '\t';
						caret(1);
						cerr << '\n';
						source.erase(source.begin() + i, source.begin() + i + 1);
						i--;
					}
				}
				else if(source[i].get_id() == token::PASTE) {
					warn(_file_name,_line_number,"Warning: preprocessor concatenation operator '##' can only be used within the '#define' preprocessor declaration; ignoring.\nLine given here for reference:");
					cerr << '\n' << '\n' << '\t';
					for(int j = i; j < source.size() && source[j].get_id() != token::NEW_LINE; j++) {
						cerr << source[j].get_qualifier() << ' ';
					}
					cerr << '\n' << '\n' << '\t';
					caret(1);
					cerr << '\n';
					source.erase(source.begin() + i,source.begin() + i + 1);
					i--;
				}
				else if(source[i].get_id() == token::HPRAGMA) {
					int savefl = i;
					for(int i = savefl + 1; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
						string s = source[i].get_qualifier();
						if(s == _line_macro || s == _file_macro || s == _date_macro || s == _time_macro || s == _timestamp_macro || s == _stdc_macro || s == _herror ||
								s == _hdefine || s == _hundef || s == _hif || s == _helif || s == _helse || s == _hendif || s == _hinclude || s == _hifdef ||
								s == _hifndef || s == _hpragma || s == _pragma_operator || s == _defined) {
							msg(_file_name, _line_number, "Error: cannot redefine '" + s + "' statement inside '#pragma' preprocessor directive.");
							cerr << '\n' << '\n' << '\t';
							for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hpragma.length() + 1);
							exit(1);
							break;
						}
					}
					int index = i;
					source.erase(source.begin() + i, source.begin() + i + 1);
					if(source[i].get_qualifier() == "once") {
						bool o = true;
						source.erase(source.begin() + i, source.begin() + i + 1);
						while(source[i].get_id() != token::NEW_LINE) {
							if(o) {
								warn(_file_name, _line_number, "Warning: unexpected token '" + source[i].get_qualifier() + ",' after the '#pragma' directive.\nLine given here for reference:");
								cerr << '\n' << '\n' << '\t' << "#pragma ";
								for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
									cerr << source[i].get_qualifier() << ' ';
								}
								cerr << '\n' << '\n' << '\t';
								caret(_hpragma.length() + 1);
								cerr << '\n';
								source.erase(source.begin() + i, source.begin() + i + 1);
								o = false;
							}
						}
						i--;
						onces.push_back(_file_name);
					}
					else {
						vector<token> save;
						save.insert(save.begin(), source.begin() + index, source.end());
						bool o = true;
						while(source[i].get_id() != token::NEW_LINE) {
							warn(_file_name, _line_number, "Warning: unexpected token '" + source[i].get_qualifier() + ",' after the '#pragma' directive.\nLine given here for reference:\n");
							cerr << '\n' << '\n' << '\t' << "#pragma ";
							if(o) {
								for(int i = 0; i < save.size() && save[i].get_id() != token::NEW_LINE; i++) {
									cerr << save[i].get_qualifier() << ' ';
								}
								cerr << '\n' << '\n' << '\t';
								caret(_hpragma.length() + 1);
								cerr << '\n';
								o = false;
							}
							source.erase(source.begin() + i, source.begin() + i + 1);
						}
					}
				}
				else if(source[i].get_id() == token::HINCLUDE) {
					int savefl = i;
					for(int i = savefl + 1; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
						string s = source[i].get_qualifier();
						if(s == _line_macro || s == _file_macro || s == _date_macro || s == _time_macro || s == _timestamp_macro || s == _stdc_macro || s == _herror ||
								s == _hdefine || s == _hundef || s == _hif || s == _helif || s == _helse || s == _hendif || s == _hinclude || s == _hifdef ||
								s == _hifndef || s == _hpragma || s == _pragma_operator || s == _defined) {
							msg(_file_name, _line_number, "Error: cannot redefine '" + s + "' statement inside '#include' preprocessor directive.");
							cerr << '\n' << '\n' << '\t';
							for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hinclude.length() + 1);
							exit(1);
							break;
						}
					}
					int index = i;
					i++;
					string ident;
					bool local = false;
					bool system = false;
					while(source[i].get_id() != token::NEW_LINE && i != source.size()) {
						if(source[i].wide()) ident += "L";
						ident += source[i].get_qualifier();
						if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) ident += "l";
						else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) ident += "u";
						else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) ident += "f";
						else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) ident += "d";
						else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) ident += "ll";
						else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) ident += "ull";
						ident += _space;
						i++;
					}
					vector<token> v = lex(ident);
					vector<token> save;
					if(v.size() == 0) {
						msg(_file_name, _line_number, "Error: expected a file to include.\nLine given here for reference:");
						cerr << '\n' << '\n' << '\t';
						for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
							cerr << source[i].get_qualifier() << ' ';
							save.push_back(source[i]);
						}
						cerr << '\n' << '\n' << '\t';
						caret(_hinclude.length() - 1);
						exit(1);
					}
					ident.clear();
					//for some conditional inclusion, they need to replace parameters
					for(int i = 0; i < v.size(); i++)
						replace_macro(v, i, symbol_table, _file_name, _line_number);
					string vec = v[0].get_qualifier();
					for(int i = 1; i < v.size(); i++) {
						if(vec[0] != '<' && v[i].get_id() == token::QUOTE) {
							vec.erase(vec.length() - 1, vec.length());
							vec += v[i].get_qualifier().substr(1,v[i].get_qualifier().length());
						}
						else if(vec[0] == '<') {
							vec += v[i].get_qualifier();
						}
						else {
							msg(_file_name, _line_number, "Error: incorrect format for '#include' preprocessor directive.");
							cerr << '\n' << '\n' << '\t';
							for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hinclude.length() + 1);
							exit(1);
						}
					}
					ident = vec;
					if(string(1,ident[0]) == _double_quote && string(1,ident[ident.length() - 1]) == _double_quote)
						local = true;
					else if(string(1, ident [0]) == _less_than && string(1,ident[ident.length() - 1]) == _greater_than)
						system = true;
					if(ident.length() < 3 ||(!local && !system)) {
						msg(_file_name, _line_number, "Error: invalid '#include' preprocessor directive format.");
						cerr << '\n' << '\n' << '\t';
						for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
							cerr << source[i].get_qualifier() << ' ';
						}
						cerr << '\n' << '\n' << '\t';
						caret(_hinclude.length() + 1);
						exit(1);
					}
					ident.erase(0,1);
					ident.erase(ident.length() - 1,ident.length());
					for(int i = 0; i < ident.length(); i++) {
						if(ident[i] == '\\') {
							warn(_file_name, _line_number, "Warning: a back-slash escape character encountered in the '#include' preprocessor directive.\nLine given here for reference:");
							cerr << '\n' << '\n' << '\t';
							for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hinclude.length() + 1);
							cerr << '\n';
						}
					}
					string init = _hline + _space + "1" + _space + _double_quote + ident + _double_quote + _new_line;
					bool search = false;
					for(int i = 0; i < onces.size(); i++) {
						if(onces[i] == ident) {
							search = true;
							break;
						}
					}
					if(search) {
						source.erase(source.begin() + index, source.begin() + index + 1);
						while(source[index].get_id() != token::NEW_LINE)
							source.erase(source.begin() + index, source.begin() + index + 1);
						i--;
					}
					else if(system) {
						string full_path = inc_dir + ident;
						file f(full_path);
						if(!f.exists()) {
							msg(_file_name,_line_number,"Error: could not find system header '" + ident + ",' in predefined search path '" + inc_dir + ".'\nLine given here for reference:");
							cerr << '\n' << '\n' << '\t';
							for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hinclude.length() + 1);
							exit(1);
						}
						else {
							string fle = f.get_text();
							string fl = init + fle + "\n" + _hline + _space + itos(_line_number) + _space + _double_quote + _file_name + _double_quote;
							source.erase(source.begin() + index,source.begin() + i);
							vector<token> _lexed = lex(remove_line_continuations(fl));
							source.insert(source.begin() + index,_lexed.begin(),_lexed.end());
						}
					}
					else if(local) {
						file f;
						string curr;
#if defined WIN32 || MINGW32
						curr = f.get_current_directory() + "\\";
#else
						curr = f.get_current_directory() + "/";
#endif
						string full_path = curr + ident;
						file lheader(full_path);
						if(!lheader.exists()) {
							msg(_file_name,_line_number,"Error: could not find local header '" + ident + ",' in predefined search path '" + curr + ".'");
							cerr << '\n' << '\n' << '\t';
							for(int i = index; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hinclude.length() + 1);
							exit(1);
						}
						else {
							string fle = lheader.get_text();
							string fl = init + fle + "\n" + _hline + _space + itos(_line_number) + _space + _double_quote + _file_name + _double_quote;
							source.erase(source.begin() + index, source.begin() + i);
							vector<token> _lexed = lex(remove_line_continuations(fl));
							source.insert(source.begin() + index, _lexed.begin(), _lexed.end());
						}
					}
					i = index - 1;
				}
				else if(source[i].get_id() == token::HERROR) {
					string view;
					i++;
					while(source[i].get_id() != token::NEW_LINE && i != source.size()) {
						view += source[i].get_qualifier() + _space;
						i++;
					}
					msg(_file_name,_line_number,"Error: '#error' preprocessor directive encountered with the following message:\n\n\t#error " + view);
					i--;
					exit(1);
				}
				else if(source[i].get_id() == token::HDEFINE) {
					int savefl = i;
					i++;
					while(source[i].get_id() != token::NEW_LINE && i != source.size()) {
						string s = source[i].get_qualifier();
						if(s == _hdefine || s == _hundef || s == _hif || s == _helif || s == _helse || s == _hendif || s == _hinclude || s == _hifdef ||
								s == _hifndef || s == _hpragma) {
							msg(_file_name, _line_number, "Error: cannot redefine '" + s + "' statement inside '#define' preprocessor directive.");
							cerr << '\n' << '\n' << '\t';
							for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hdefine.length() + 1);
							exit(1);
							break;
						}
						i++;
					}
					i = savefl;
					//for symbol_table:
					//1. identifier
					//2. macro code
					//3. parameters(if any): __VA_ARGS__ for variable arguments, - for zero parameters but parenthesis present
					if(i + 1 == source.size() || source[i + 1].get_id() != token::IDENTIFIER) {
						string s = source[i + 1].get_qualifier();
						if(s == _line_macro || s == _file_macro || s == _date_macro || s == _time_macro || s == _timestamp_macro || s == _stdc_macro || s == _herror ||
								s == _hdefine || s == _hundef || s == _hif || s == _helif || s == _helse || s == _hendif || s == _hinclude || s == _hifdef ||
								s == _hifndef || s == _hpragma || s == _pragma_operator || s == _defined) {
							msg(_file_name, _line_number, "Error: cannot redefine '" + s + "' statement inside '#define' preprocessor directive.");
							cerr << '\n' << '\n' << '\t';
							for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hdefine.length() + 1);
							exit(1);
							break;
						}
						string ident;
						if(i + 1 >= source.size())
							ident = "EOF";
						else if(source[i + 1].get_qualifier() == _new_line)
							ident = "NEW_LINE";
						else
							ident = source[i + 1].get_qualifier();
						msg(_file_name,_line_number,"Error: expected an identifier after the '#define' preprocessor directive instead of '" + ident + ".'\nLine given here for reference:");
						cerr << '\n' << '\n' << '\t';
						for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
							cerr << source[i].get_qualifier() << ' ';
						}
						cerr << '\n' << '\n' << '\t';
						caret(_hdefine.length() + 1);
						exit(1);
						break;
					}
					int index = i;
					string id = source[i + 1].get_qualifier();
					string s = id;
					if(s == _line_macro || s == _file_macro || s == _date_macro || s == _time_macro || s == _timestamp_macro || s == _stdc_macro || s == _herror ||
							s == _hdefine || s == _hundef || s == _hif || s == _helif || s == _helse || s == _hendif || s == _hinclude || s == _hifdef ||
							s == _hifndef || s == _hpragma || s == _pragma_operator || s == _defined) {
						msg(_file_name, _line_number, "Error: cannot redefine '" + s + "' statement inside '#define' preprocessor directive.");
						cerr << '\n' << '\n' << '\t';
						for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
							cerr << source[i].get_qualifier() << ' ';
						}
						cerr << '\n' << '\n' << '\t';
						caret(_hdefine.length() + 1);
						exit(1);
						break;
					}
					if(id == _va_args_macro) {
						int _caret = 0;
						msg(_file_name, _line_number, "Error: '__VA_ARGS__' is a reserved preprocessor identifier and cannot be used as a macro identifier.\nMacro definition given here for reference:");
						cerr << '\n' << '\n' << '\t';
						bool save = true;
						for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
							cerr << source[i].get_qualifier() << ' ';
							if(source[i].get_qualifier() == _va_args_macro) save = false;
							if(save)
								_caret += source[i].get_qualifier().length() + 1;
						}
						cerr << '\n' << '\n' << '\t';
						caret(_caret);
						exit(1);
					}
					string func = id;
					int k = i + 3;
					bool b = true;
					while(source[k].get_id() != token::CLOSE_PARENTHESIS && k != source.size()) {
						if(source[k].get_id() == token::IDENTIFIER || source[k].get_id() == token::COMMA || source[k].get_id() == token::VA_ARGS);
						else {
							b = false;
							break;
						}
						k++;
					}
					if(b) {
						i += 3;
						id.clear();
						while(source[i].get_id() != token::CLOSE_PARENTHESIS && i != source.size()) {
							if(source[i].get_qualifier() == _va_args_macro) {
								int _caret = 0;
								msg(_file_name, _line_number, "Error: '__VA_ARGS__' is a reserved preprocessor identifier and caanot be used as a macro parameter.\nMacro definition given here for reference:");
								cerr << '\n' << '\n' << '\t';
								bool save = true;
								for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
									cerr << source[i].get_qualifier() << ' ';
									if(source[i].get_qualifier() == _va_args_macro) save = false;
									if(save)
										_caret += source[i].get_qualifier().length() + 1;
								}
								cerr << '\n' << '\n' << '\t';
								caret(_caret);
								exit(1);
							}
							id += source[i].get_qualifier();
							i++;
						}
						string code;
						i++;
						while(source[i].get_id() != token::NEW_LINE) {
							if(source[i].get_id() == token::WQUOTE) {
								string q = source[i].get_qualifier();
								source.erase(source.begin() + i, source.begin() + i + 1);
								source.insert(source.begin() + i, token("L" + q));
							}
							else if(source[i].get_id() == token::WCHARACTER) {
								string q = source[i].get_qualifier();
								source.erase(source.begin() + i, source.begin() + i + 1);
								source.insert(source.begin() + i, token("L" + q));
							}
							code += source[i].get_qualifier();
							if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) code += "l";
							else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) code += "u";
							else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) code += "f";
							else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) code += "d";
							else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) code += "ll";
							else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) code += "ull";
							code += _space;
							i++;
						}
						id = replace_all(id,_variable_arguments,_va_args_macro);
						if(id.length() == 0) id = "-";
						symbol_table.push_back(make_pair(make_pair(func,code),id));
						source.erase(source.begin() + index,source.begin() + i);
						i = index - 1;
					}
					else {
						string code;
						i += 2;
						while(source[i].get_id() != token::NEW_LINE) {
							if(source[i].get_id() == token::WQUOTE) {
								string q = source[i].get_qualifier();
								source.erase(source.begin() + i, source.begin() + i + 1);
								source.insert(source.begin() + i, token("L" + q));
							}
							else if(source[i].get_id() == token::WCHARACTER) {
								string q = source[i].get_qualifier();
								source.erase(source.begin() + i, source.begin() + i + 1);
								source.insert(source.begin() + i, token("L" + q));
							}
							code += source[i].get_qualifier();
							if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) code += "l";
							else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) code += "u";
							else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) code += "f";
							else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) code += "d";
							else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) code += "ll";
							else if(source[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) code += "ull";
							code += _space;
							i++;
						}
						symbol_table.push_back(make_pair(make_pair(id,code),""));
						source.erase(source.begin() + index,source.begin() + i);
						i = index - 1;
					}
				}
				else if(source[i].get_id() == token::HIFDEF || source[i].get_id() == token::HIFNDEF || source[i].get_id() == token::HIF) {
					int balanced = 1;
					vector<token> checking;
					checking.push_back(source[i]);
					int k = i + 1;
					int tl = _line_number;
					while(balanced != 0 && k != source.size()) {
						if(source[k].get_id() == token::NEW_LINE)
							tl++;
						if(source[k].get_id() == token::HIFDEF || source[k].get_id() == token::HIFNDEF || source[k].get_id() == token::HIF)
							balanced++;
						else if(source[k].get_id() == token::HENDIF) {
							bool b = true;
							int temp = k + 1;
							vector<token> save;
							bool o = true;
							for(int i = k; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) save.push_back(source[i]);
							if(b)
								while(source[temp].get_id() != token::NEW_LINE && temp != source.size()) {
									if(o) {
										warn(_file_name, tl, "Warning: unexpected token '" + save[1].get_qualifier() + "' found after '#endif' preprocessor directive; ignoring.\nLine given here for reference:");
										cerr << '\n' << '\n' << '\t';
										for(int i = 0; i < save.size(); i++) {
											cerr << save[i].get_qualifier() << ' ';
										}
										cerr << '\n' << '\n' << '\t';
										caret(_hendif.length() + 1);
										cerr << '\n';
										o = false;
									}
									source.erase(source.begin() + temp, source.begin() + temp + 1);
								}
							balanced--;
						}
						else if(source[k].get_id() == token::HELSE) {
							vector<token> save;
							for(int i = k; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) save.push_back(source[i]);
							bool b = true;
							int temp = k + 1;
							bool o = true;
							if(b)
								while(source[temp].get_id() != token::NEW_LINE && temp != source.size()) {
									if(o) {
										warn(_file_name, tl, "Warning: unexpected token '" + source[temp].get_qualifier() + "' found after '#else' preprocessor directive; ignoring.\nLine given here for reference:");
										cerr << '\n' << '\n' << '\t';
										for(int i = 0; i < save.size(); i++) {
											cerr << save[i].get_qualifier() << ' ';
										}
										cerr << '\n' << '\n' << '\t';
										caret(_helse.length() + 1);
										cerr << '\n';
										o = false;
									}
									source.erase(source.begin() + temp, source.begin() + temp + 1);
								}
						}
						checking.push_back(source[k]);
						k++;
					}
					if(balanced > 0) {
						msg(_file_name, _line_number, "Error: expected a '#endif' preprocessor directive at the end of the input.");
						exit(1);
					}
					else if(balanced < 0) {
						msg(_file_name, _line_number, "Error: an unmatched '#endif' preprocessor directive with no introductory preprocessor conditional");
						exit(1);
					}
					else {}
					source.erase(source.begin() + i,source.begin() + k);
					vector<vector<token>> parts;
					int index = 0;
					balanced = 1;
					int kk = 0;
					//handle first iteration, loop the rest
					if(checking[kk].get_id() == token::HIFDEF || checking[kk].get_id() == token::HIFNDEF) {
						string orig = checking[kk].get_qualifier();
						if(kk + 1 == checking.size() || checking[kk + 1].get_id() != token::IDENTIFIER) {
							int savefl = kk;
							string ident;
							if(kk + 1 >= checking.size())
								ident = "EOF";
							else if(checking[kk + 1].get_qualifier() == _new_line)
								ident = "NEW_LINE";
							else
								ident = checking[kk + 1].get_qualifier();
							msg(_file_name, _line_number, "Error: expected an identifier after the '" + checking[kk].get_qualifier() + "' preprocessor directive instead of '" + ident + ".'");
							cerr << '\n' << '\n' << '\t';
							for(int i = savefl; i < checking.size() && checking[i].get_id() != token::NEW_LINE; i++) {
								cerr << checking[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							if(orig == _hifndef) caret(_hifndef.length() + 1);
							else if(orig == _hifdef) caret(_hifdef.length() + 1);
							exit(1);
							break;
						}
						else {
							string directive = checking[kk].get_qualifier();
							bool o = true;
							int ii = kk;
							ii += 2;
							while(checking[ii].get_id() != token::NEW_LINE && ii != checking.size()) {
								if(o) {
									warn(_file_name, _line_number, "Warning: unexpected token '" + checking[i].get_qualifier() + "' after the '" + directive + "' preprocessor directive; ignoring.\nLine given here for reference:");
									int savefl = i;
									cerr << '\n' << '\n' << '\t';
									if(directive == _hifdef) cerr << "#ifdef ";
									else if(directive == _hifndef) cerr << "#ifndef ";
									for(int i = savefl; i < checking.size() && checking[i].get_id() != token::NEW_LINE; i++) {
										cerr << checking[i].get_qualifier() << ' ';
									}
									cerr << '\n' << '\n' << '\t';
									if(orig == _hifdef) caret(_hifdef.length() + 1);
									else if(orig == _hifndef) caret(_hifndef.length() + 1);
									cerr << '\n';
									o = false;
								}
								ii++;
							}
							ii--;
						}
					}
					if(checking[kk].get_id() == token::HIFDEF || checking[kk].get_id() == token::HIFNDEF || checking[kk].get_id() == token::HIF) {
						int inner_balanced = 1;
						kk++;
						while(inner_balanced != 0) {
							if(checking[kk].get_id() == token::HIF || checking[kk].get_id() == token::HIFDEF || checking[kk].get_id() == token::HIFNDEF)
								inner_balanced++;
							else if(checking[kk].get_id() == token::HENDIF)
								inner_balanced--;
							kk++;
						}
					}
					for(int i = 1; i < checking.size() && balanced != 0; i++) {
						if(checking[i].get_id() == token::HENDIF || checking[i].get_id() == token::HELSE) {
							string orig = checking[i].get_qualifier();
							int savefl = i;
							for(int i = savefl + 1; i < checking.size() && checking[i].get_id() != token::NEW_LINE; i++) {
								string s = checking[i].get_qualifier();
								if(s == _line_macro || s == _file_macro || s == _date_macro || s == _time_macro || s == _timestamp_macro || s == _stdc_macro || s == _herror ||
										s == _hdefine || s == _hundef || s == _hif || s == _helif || s == _helse || s == _hendif || s == _hinclude || s == _hifdef ||
										s == _hifndef || s == _hpragma || s == _pragma_operator || s == _defined) {
									msg(_file_name, _line_number, "Error: cannot redefine '" + s + "' statement inside preprocessor conditional endings.");
									cerr << '\n' << '\n' << '\t';
									for(int i = savefl; i < checking.size() && checking[i].get_id() != token::NEW_LINE; i++) {
										cerr << checking[i].get_qualifier() << ' ';
									}
									cerr << '\n' << '\n' << '\t';
									if(orig == _hendif) caret(_hendif.length() + 1);
									else if(orig == _helse) caret(_helse.length() + 1);
									exit(1);
									break;
								}
							}
							vector<token> save;
							bool o = true;
							for(int k = i; k < checking.size() && checking[k].get_id() != token::NEW_LINE; k++) save.push_back(checking[k]);
							int ii = i;
							ii++;
							string directive = checking[ii - 1].get_qualifier();
							while(checking[ii].get_id() != token::NEW_LINE && ii != checking.size()) {
								if(o) {
									warn(_file_name, _line_number, "Warning: unexpected token '" + checking[ii].get_qualifier() + "' after the '" + directive + "' preprocessor directive; ignoring.\nLine given here for reference:");
									cerr << '\n' << '\n' << '\t';
									for(int i = 0; i < save.size(); i++) {
										cerr << save[i].get_qualifier() << ' ';
									}
									cerr << '\n' << '\n' << '\t';
									if(orig == _hendif) caret(_hendif.length() + 1);
									else if(orig == _helse) caret(_helse.length() + 1);
									cerr << '\n';
									o = false;
								}
								ii++;
							}
							ii--;
						}
						if(checking[i].get_id() == token::HIFDEF || checking[i].get_id() == token::HIFNDEF) {
							string orig = checking[i].get_qualifier();
							if(i + 1 == checking.size() || checking[i + 1].get_id() != token::IDENTIFIER) {
								int savefl = i;
								string ident;
								if(i + 1 >= checking.size())
									ident = "EOF";
								else if(checking[i + 1].get_qualifier() == _new_line)
									ident = "NEW_LINE";
								else
									ident = checking[i + 1].get_qualifier();
								msg(_file_name,_line_number,"Error: expected an identifier after the '" + checking[i].get_qualifier() + "' preprocessor directive instead of '" + ident + ".'");
								cerr << '\n' << '\n' << '\t';
								for(int i = savefl; i < checking.size() && checking[i].get_id() != token::NEW_LINE; i++) {
									cerr << checking[i].get_qualifier() << ' ';
								}
								cerr << '\n' << '\n' << '\t';
								if(orig == _hifndef) caret(_hifndef.length() + 1);
								else if(orig == _hifdef) caret(_hifdef.length() + 1);
								exit(1);
								break;
							}
							else {
								string directive = checking[i].get_qualifier();
								bool o = true;
								int ii = i;
								ii += 2;
								while(checking[ii].get_id() != token::NEW_LINE && ii != checking.size()) {
									if(o) {
										warn(_file_name, _line_number, "Warning: unexpected token '" + checking[i].get_qualifier() + "' after the '" + directive + "' preprocessor directive; ignoring.\nLine given here for reference:");
										int savefl = i;
										cerr << '\n' << '\n' << '\t';
										for(int i = savefl; i < checking.size() && checking[i].get_id() != token::NEW_LINE; i++) {
											cerr << checking[i].get_qualifier() << ' ';
										}
										cerr << '\n' << '\n' << '\t';
										if(orig == _hifndef) caret(_hifndef.length() + 1);
										else if(orig == _hifdef) caret(_hifdef.length() + 1);
										cerr << '\n';
										o = false;
									}
									ii++;
								}
								ii--;
							}
						}
						if(checking[i].get_id() == token::HIFDEF || checking[i].get_id() == token::HIFNDEF || checking[i].get_id() == token::HIF) {
							int inner_balanced = 1;
							i++;
							while(inner_balanced != 0) {
								if(checking[i].get_id() == token::HIF || checking[i].get_id() == token::HIFDEF || checking[i].get_id() == token::HIFNDEF)
									inner_balanced++;
								else if(checking[i].get_id() == token::HENDIF)
									inner_balanced--;
								i++;
							}
						}
						else if(checking[i].get_id() == token::HELIF || checking[i].get_id() == token::HELSE) {
							vector<token> v;
							for(int j = index; j < i; j++) {
								v.push_back(checking[j]);
							}
							parts.push_back(v);
							index = i;
						}
						else if(checking[i].get_id() == token::HENDIF) {
							vector<token> v;
							for(int j = index; j < i; j++) {
								v.push_back(checking[j]);
							}
							balanced--;
							if(balanced != 0)
								v.push_back(token(_hendif));
							parts.push_back(v);
							index = i;
						}
					}
					if(balanced > 0) {
						msg(_file_name, _line_number, "Error: expected a '#endif' preprocessor directive at the end of the input.");
						exit(1);
					}
					else if(balanced < 0) {
						msg(_file_name, _line_number, "Error: an unmatched '#endif' preprocessor directive with no introductory preprocessor conditional");
						exit(1);
					}
					else {}
					int iindex = -1;
					int store = i;
					vector<token> initial;
					vector<token> after;
					int l = _line_number;
					int lt = _line_number;
					for(int i = 0; i < parts.size(); i++) {
						vector<token> current = parts[i];
						if(current[0].get_id() == token::HELSE && i != parts.size() - 1) {
							vector<token> next = parts[i + 1];
							for(int i = 0; i < next.size(); i++) {
								if(next[i].get_id() == token::NEW_LINE)
									lt++;
								else {
								}
							}
						}
						else {
							for(int i = 0; i < current.size(); i++)
								if(current[i].get_id() == token::NEW_LINE)
									lt++;
						}
					}
					for(int i = 0; i < parts.size(); i++) {
						vector<token> t = parts[i];
						vector<token> cond;
						for(int i = 0; t[i].get_id() != token::NEW_LINE && i < t.size(); i++) {
							if(t[i].get_id() == token::WCHARACTER || t[i].get_id() == token::WQUOTE)
								cond.push_back("L" + t[i].get_qualifier());
							else
								cond.push_back(t[i]);
						}
						if(cond[0].get_id() == token::HIFDEF || cond[0].get_id() == token::HIFNDEF) {
							string check = cond[1].get_qualifier();
							int id = cond[1].get_id();
							bool b = false;
							if(id == token::INTEGER || id == token::QUOTE) b = true;
							else {
								for(int i = 0; i < symbol_table.size(); i++) {
									if(check == symbol_table[i].first.first) {
										b = true;
										break;
									}
									else {}
								}
							}
							if(cond[0].get_id() == token::HIFDEF) {
								if(b) {
									iindex = i;
									break;
								}
								else {}
							}
							else if(cond[0].get_id() == token::HIFNDEF) {
								if(!b) {
									iindex = i;
									break;
								}
								else {}
							}
						}
						else if(cond[0].get_id() == token::HIF || cond[0].get_id() == token::HELIF) {
							string ident;
							for(int i = 1; i < cond.size(); i++) {
								if(cond[i].wide()) ident += "L";
								ident += cond[i].get_qualifier();
								if(cond[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_L) ident += "l";
								else if(cond[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_U) ident += "u";
								else if(cond[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) ident += "f";
								else if(cond[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D) ident += "d";
								else if(cond[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_LL) ident += "ll";
								else if(cond[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_ULL) ident += "ull";
								ident += _space;
							}
							bool b = evaluate_preprocessor_conditional(ident,symbol_table,l,_file_name,cond[0].get_qualifier());
							if(b) {
								iindex = i;
								break;
							}
						}
						else if(cond[0].get_id() == token::HELSE) {
							iindex = i;
							break;
						}
						for(int i = 0; i < t.size(); i++) {
							if(t[i].get_id() == token::NEW_LINE) {
								initial.push_back(token(_new_line));
								l++;
							}
						}
					}
					source.insert(source.begin() + i,initial.begin(),initial.end());
					if(iindex == -1);
					else {
						vector<token> v = parts[iindex];
						for(;v[0].get_id() != token::NEW_LINE;)
							v.erase(v.begin(),v.begin() + 1);
						source.insert(source.begin() + i + initial.size(),v.begin(),v.end());
						for(int i = iindex + 1; i < parts.size(); i++) {
							vector<token> v = parts[i];
							for(int i = 0; i < v.size(); i++) {
								if(v[i].get_id() == token::NEW_LINE) {
									after.push_back(token(_new_line));
								}
							}
						}
						source.insert(source.begin() + i + v.size() + initial.size(),after.begin(),after.end());
					}
					i--;
				}
				else if(source[i].get_id() == token::HUNDEF) {
					int savefl = i;
					for(int i = savefl + 1; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
						string s = source[i].get_qualifier();
						if(s == _line_macro || s == _file_macro || s == _date_macro || s == _time_macro || s == _timestamp_macro || s == _stdc_macro || s == _herror ||
								s == _hdefine || s == _hundef || s == _hif || s == _helif || s == _helse || s == _hendif || s == _hinclude || s == _hifdef ||
								s == _hifndef || s == _hpragma || s == _pragma_operator || s == _defined) {
							msg(_file_name, _line_number, "Error: cannot redefine '" + s + "' statement inside '#undef' preprocessor directive.");
							cerr << '\n' << '\n' << '\t';
							for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hundef.length() + 1);
							exit(1);
							break;
						}
					}
					string s = source[i + 1].get_qualifier();
					if(i + 1 == source.size() || source[i + 1].get_id() != token::IDENTIFIER) {
						string ident;
						if(i + 1 >= source.size())
							ident = "EOF";
						else if(source[i + 1].get_qualifier() == _new_line)
							ident = "NEW_LINE";
						else
							ident = source[i + 1].get_qualifier();
						msg(_file_name,_line_number,"Error: expected an identifier after the '#undef' preprocessor directive instead of '" + source[i + 1].get_qualifier() + ".'");
						cerr << '\n' << '\n' << '\t';
						for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
							cerr << source[i].get_qualifier() << ' ';
						}
						cerr << '\n' << '\n' << '\t';
						caret(_hundef.length() + 1);
						exit(1);
						break;
					}
					else if(source[i + 1].get_id() == token::IDENTIFIER &&(s == _pragma_operator || s == _defined || s == _line_macro || s == _file_macro || s == _stdc_macro || s == _timestamp_macro || s == _stdc_macro || s == _date_macro || s == _time_macro)) {
						warn(_file_name,_line_number,"Warning: cannot undefine the predefined preprocessor macro: '" + s + ",' in the '#undef' preprocessor directive; ignoring.\nLine given here for reference:");
						cerr << '\n' << '\n' << '\t';
						for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
							cerr << source[i].get_qualifier() << ' ';
						}
						cerr << '\n' << '\n' << '\t';
						caret(_hundef.length() + 1);
						cerr << '\n';
						bool o = true;
						i += 2;
						while(source[i].get_id() != token::NEW_LINE && i != source.size()) {
							if(o) {
								warn(_file_name, _line_number, "Warning: unexpected token '" + source[i].get_qualifier() + "' after '#undef' preprocessor directive; ignoring.\nLine given here for reference:");
								cerr << '\n' << '\n' << '\t';
								for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
									cerr << source[i].get_qualifier() << ' ';
								}
								cerr << '\n' << '\n' << '\t';
								caret(_hundef.length() + 1);
								o = false;
								cerr << '\n';
							}
							i++;
						}
						i--;
					}
					else {
						token er = source[i + 1];
						string now = source[i + 1].get_qualifier();
						int index = -1;
						for(int i = 0; i < symbol_table.size(); i++) {
							string now = symbol_table[i].first.first;
							if(now == er.get_qualifier()) {
								index = i;
								break;
							}
						}
						if(index != -1)
							symbol_table.erase(symbol_table.begin() + index, symbol_table.begin() + index + 1);
						else {}
						if(source[savefl + 2].get_id() != token::NEW_LINE) {
							warn(_file_name, _line_number, "Warning: Junk characters after the '#undef' preprocessor directive; ignoring.\nLine given here for reference:");
							cerr << '\n' << '\n' << '\t';
							for(int i = savefl; i < source.size() && source[i].get_id() != token::NEW_LINE; i++) {
								cerr << source[i].get_qualifier() << ' ';
							}
							cerr << '\n' << '\n' << '\t';
							caret(_hundef.length() + 1 + source[savefl + 1].get_qualifier().length() + 1);
							cerr << '\n';
						}
						while(source[i].get_id() != token::NEW_LINE && i < source.size()) {
							source.erase(source.begin() + i, source.begin() + i + 1);
						}
						i--;
					}
				}
				else {
					int index = i;
					replace_macro(source,i,symbol_table,_file_name,_line_number);
				}
			}
			return ret;
		}

		string evaluate_basic_bool_expr(string res) {
			vector<token> result = lex(res);
			for(int i = 0; i < result.size(); i++) {
				if(result[i].get_id() == token::MINUS) {
					string number = "-" + result[i + 1].get_qualifier();
					result.erase(result.begin() + i,result.begin() + i + 2);
					result.insert(result.begin() + i,number);
				}
			}
			for(;result.size() > 1;) {
				string first = result[0].get_qualifier();
				string third = result[2].get_qualifier();
				if(_stoi(first) > 0 && _stoi(third) > 0) {
					result.erase(result.begin(),result.begin() + 3);
					result.insert(result.begin(),token("1"));
				}
				else if(_stoi(first) <= 0 && _stoi(third) <= 0) {
					result.erase(result.begin(),result.begin() + 3);
					result.insert(result.begin(),token("0"));
				}
				else {
					if(result[1].get_id() == token::OR) {
						result.erase(result.begin(),result.begin() + 3);
						result.insert(result.begin(),token("1"));
					}
					else if(result[1].get_id() == token::AND) {
						result.erase(result.begin(),result.begin() + 3);
						result.insert(result.begin(),token("0"));
					}
				}
			}
			return result[0].get_qualifier();
		}
	
		bool evaluate_preprocessor_conditional(string code, vector <pair<pair<string,string>,string>> symbol_table, int & line, string file, string dir) {
			if(code.length() == 0) {
				int savefl = 0;
				msg(file,line,"Error: expected expression after conditional preprocessor directive '" + dir + ".'\nLine given here for reference:");
				cerr << '\n' << '\n' << '\t';
				cerr << dir << ' ';
				cerr << '\n' << '\n' << '\t';
				caret(dir.length() - 1);
				cerr << '\n';
				line = -1;
				return false;
			}
			//save original code for diagnostics
			string ccode = code;
			for(int i = 0; i < code.length(); i++) {
				if(code[i] == '-') {
					code.erase(i, 1);
					code.insert(i, "-1*");
				}
			}
			vector<token> _lexed = lex(code);
			int pbal = 0;
			//make sure parenthesis are balanced
			for(int i = 0; i < _lexed.size(); i++) {
				if(_lexed[i].get_id() == token::OPEN_PARENTHESIS) pbal++;
				else if(_lexed[i].get_id() == token::CLOSE_PARENTHESIS) pbal--;
				if(_lexed[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D ||
						_lexed[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) {
					msg(file,line,string("Error: no 'double' or 'float' constants allowed.\nRegion given here for reference:"));
					cerr << '\n' << '\n' << '\t';
					cerr << dir << ' ' << code;
					cerr << '\n' << '\n' << '\t';
					caret(dir.length() + 1);
					line = -1;
					exit(1);
					return false;
				}
			}
			if(pbal > 0) {
				msg(file,line,string("Error: expected a ')' after the input conditional of:\t'" + ccode + ",' of the preprocessor directive '" + dir + ".'\nLine given here for reference:"));
				cerr << '\n' << '\n' << '\t';
				cerr << dir << ' ' << code;
				cerr << '\n' << '\n' << '\t';
				caret(dir.length() + 1 + code.length() - 2);
				cerr << '\n';
				line = -1;
				return false;
			}
			else if(pbal < 0) {
				msg(file,line,string("Error: expected a '(' before the input conditional of:\t'" + ccode + ",' of the preprocessor directive '" + dir + ".'\nLine given here for reference:"));
				cerr << '\n' << '\n' << '\t';
				cerr << dir << ' ' << code;
				cerr << '\n' << '\n' << '\t';
				caret(dir.length() + 1);
				cerr << '\n';
				line = -1;
				return false;
			}
			vector<string> errors;
			//handle 'defined' statements
			for(int i = 0; i < _lexed.size(); i++) {
				if(_lexed[i].get_id() == token::IDENTIFIER && _lexed[i].get_qualifier() == _defined) {
					if(_lexed[i + 1].get_id() == token::IDENTIFIER) {
						bool def = false;
						string s = _lexed[i + 1].get_qualifier();
						for(int i = 0; i < symbol_table.size(); i++) {
							if(symbol_table[i].first.first == s) {
								def = true;
								break;
							}
						}
						_lexed.erase(_lexed.begin() + i,_lexed.begin() + i + 2);
						if(def)
							_lexed.insert(_lexed.begin() + i,token("1"));
						else
							_lexed.insert(_lexed.begin() + i,token("0"));
						i--;
					}
					else if(_lexed[i + 1].get_id() == token::OPEN_PARENTHESIS && _lexed[i + 2].get_id() == token::IDENTIFIER && _lexed[i + 3].get_id() == token::CLOSE_PARENTHESIS) {
						bool def = false;
						string s = _lexed[i + 2].get_qualifier();
						for(int i = 0; i < symbol_table.size(); i++) {
							if(symbol_table[i].first.first == s) {
								def = true;
								break;
							}
						}
						_lexed.erase(_lexed.begin() + i,_lexed.begin() + i + 4);
						if(def)
							_lexed.insert(_lexed.begin() + i,token("1"));
						else
							_lexed.insert(_lexed.begin() + i,token("0"));
						i--;
					}
					else {
						msg(file,line,"Error: a defined check in the conditional preprocessor directive '" + dir + ",' must either be in the form defined(id) or defined id.\nLine given here fro reference:");
						cerr << '\n' << '\n' << '\t';
						cerr << dir << ' ' << code;
						cerr << '\n' << '\n' << '\t';
						caret(dir.length() + 1);
						exit(1);
					}
				}
			}
			//replace macros for proper conditional evaluation
			for(int i = 0; i < _lexed.size(); i++) {
				string s = _lexed[i].get_qualifier();
				if(s == _herror || s == _hif || s == _helif || s == _helse || s == _hendif || s == _hifndef || s == _hifdef || s == _hdefine || s == _hundef || s == _hpragma || s == _hinclude) {
					errors.push_back("Error, did not expect the nested preprocessor directive '" + s + "' in the conditional, preprocessor directive '" + dir + ".'\nLine given here for reference:");
					break;
				}
				else if(_lexed[i].get_qualifier() == _line_macro) {
					_lexed.erase(_lexed.begin() + i,_lexed.begin() + i + 1);
					_lexed.insert(_lexed.begin() + i,token(itos(line)));
				}
				else {
					replace_macro(_lexed,i,symbol_table,file,line);
				}
				if(_lexed[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_D ||
						_lexed[i].get_c_suffix_token_kind() == c_suffix_token_kind::SUFFIX_TOKEN_F) {
					msg(file,line,string("Error: no 'double' or 'float' constants allowed.\nRegion given here for reference:"));
					cerr << '\n' << '\n' << '\t';
					cerr << dir << ' ' << code;
					cerr << '\n' << '\n' << '\t';
					caret(dir.length() + 1);
					line = -1;
					exit(1);
					return false;
				}
			}
			if(errors.size() == 0);
			else {
				for(int i = 0; i < errors.size(); i++) {
					msg(file, line, errors[i]);
					cerr << '\n' << '\n' << '\t';
					cerr << dir << ' ' << code;
					cerr << '\n' << '\n' << '\t';
					caret(dir.length() + 1);
					cerr << '\n';
				}
				line = -1;
				return false;
			}
			for(int i = 0; i < _lexed.size(); i++)
				if(_lexed[i].get_id() == token::IDENTIFIER &&(_lexed[i].get_qualifier() == _line_macro)) {
					_lexed.erase(_lexed.begin() + i,_lexed.begin() + i + 1);
					_lexed.insert(_lexed.begin() + i,token(itos(line)));
				}
			//convert any characters to integers before evaluation
			for(int i = 0; i < _lexed.size(); i++) {
				if(_lexed[i].get_id() == token::FLOATING_POINT || _lexed[i].get_id() == token::QUOTE || _lexed[i].get_id() == token::WQUOTE) {
					msg(file, line, "Error: only integral types accepted in the conditional preprocessor directive '" + dir + ".'");
					cerr << '\n' << '\n' << '\t';
					cerr << dir << ' ' << code;
					cerr << '\n' << '\n' << '\t';
					caret(dir.length() + 1);
					exit(1);
				}
				else if(_lexed[i].get_id() == token::CHARACTER || _lexed[i].get_id() == token::WCHARACTER) {
					string ident = _lexed[i].get_qualifier();
					ident.erase(0, 1);
					ident.erase(ident.length() - 1, ident.length());
					std::stringstream ss;
					if(ident.substr(0, 2) == "\\x") {
						int r;
						ident.erase(0, 2);
						ss << std::hex << ident.c_str();
						ss >> r;
						_lexed.erase(_lexed.begin() + i, _lexed.begin() + i + 1);
						_lexed.insert(_lexed.begin() + i, itos(r));
					}
					else if(ident[0] == '\\' && isdigit(ident[1])) {
						int r;
						ident.erase(0, 1);
						ss << std::oct << ident.c_str();
						ss >> r;
						_lexed.erase(_lexed.begin() + i, _lexed.begin() + i + 1);
						_lexed.insert(_lexed.begin() + i, itos(r));
					}
					else {
						int r = ident[ident.length() - 1];
						_lexed.erase(_lexed.begin() + i, _lexed.begin() + i + 1);
						_lexed.insert(_lexed.begin() + i, itos(r));
					}
				}
			}
			for(int i = 0; i < _lexed.size(); i++) {
				if(_lexed[i].get_id() == token::IDENTIFIER) {
					if(_lexed[i + 1].get_id() == token::OPEN_PARENTHESIS) {
						int index = i;
						i += 2;
						int balanced = 1;
						while(balanced != 0 && i != _lexed.size()) {
							if(_lexed[i].get_id() == token::OPEN_PARENTHESIS)
								balanced++;
							else if(_lexed[i].get_id() == token::CLOSE_PARENTHESIS)
								balanced--;
							i++;
						}
						_lexed.erase(_lexed.begin() + index,_lexed.begin() + i);
						_lexed.insert(_lexed.begin() + index,token("0"));
						i = index - 1;
					}
					else {
						_lexed.erase(_lexed.begin() + i,_lexed.begin() + i + 1);
						_lexed.insert(_lexed.begin() + i,token("0"));
					}
				}
			}
			code.clear();
			for(int i = 0; i < _lexed.size(); i++)
				code.insert(code.length(),_lexed[i].get_qualifier() + _space);
			code.insert(0,_open_parenthesis);
			code.insert(code.length(),_close_parenthesis);
			for(int i = 0; i < code.length(); i++) {
				if(string(1,code[i]) == _open_parenthesis) {
					int index = i;
					string ident = _open_parenthesis;
					int balanced = 1;
					i++;
					while(balanced != 0) {
						if(string(1,code[i]) == _open_parenthesis) {
							break;
						}
						else if(string(1,code[i]) == _close_parenthesis)
							balanced--;
						ident.insert(ident.length(),string(1,code[i]));
						i++;
					}
					if(string(1,code[i]) == _open_parenthesis) i--;
					else {
						vector<token> _lex = lex(ident);
						for(int i = 0; i < _lex.size(); i++) {
							if(_lex[i].get_id() == token::NOT) {
								int counter = 0;
								int iindex = i;
								while(_lex[i].get_id() == token::NOT) {
									i++;
									counter++;
								}
								_lex.erase(_lex.begin() + iindex,_lex.begin() + iindex + counter);
								if(counter % 2 == 0);
								else _lex.insert(_lex.begin() + iindex,token(_not));
							}
						}
						for(int i = 0; i < _lex.size(); i++) {
							if(_lex[i].get_id() == token::NOT && _lex[i + 1].get_qualifier() == "-") {
								_lex.erase(_lex.begin() + i,_lex.begin() + i + 3);
								_lex.insert(_lex.begin() + i,token(itos(1)));
								i = -1;
							}
							if(_lex[i].get_id() == token::NOT && _stoi(_lex[i + 1].get_qualifier()) <= 0) {
								_lex.erase(_lex.begin() + i, _lex.begin() + i + 2);
								_lex.insert(_lex.begin() + i, token(itos(1)));
								i = -1;
							}
							else if(_lex[i].get_id() == token::NOT && _stoi(_lex[i + 1].get_qualifier()) > 0) {
								_lex.erase(_lex.begin() + i,_lex.begin() + i + 2);
								_lex.insert(_lex.begin() + i,token(itos(0)));
								i = -1;
							}
						}
						ident.clear();
						for(int i = 0; i < _lex.size(); i++)
							ident.insert(ident.length(),_lex[i].get_qualifier() + _space);
						vector<string> parts;
						vector<string> seps;
						int iindex = 0;
						for(int i = 0; i < ident.length(); i++) {
							string two = ident.substr(i,2);
							string one = string(1,ident[i]);
							if(two == _greater_than_or_equal_to || two == _less_than_or_equal_to || two == _equality || two == _inequality || two == _and || two == _or) {
								parts.push_back(ident.substr(iindex,i - iindex));
								trim(two);
								seps.push_back(two);
								i++;
								iindex = i + 2;
							}
							else if(one == _greater_than || one == _less_than || one == _question_mark || one == _colon) {
								parts.push_back(ident.substr(iindex,i - iindex));
								trim(one);
								seps.push_back(one);
								i++;
								iindex = i + 1;
							}
						}
						parts.push_back(ident.substr(iindex,ident.length()));
						for(int i = 0; i < parts.size(); i++) {
							string now = parts[i];
							int par = 0;
							for(int i = 0; i < now.length(); i++) {
								if(now[i] == '(')
									par++;
								else if(now[i] == ')')
									par--;
							}
							if(par == 0) {
								now.insert(now.length(),_plus + itos(0));
								expr e(line,file,dir);
								e.set_expr(now);
								string eval = e.use_notation();
								if(e.get_error_counter() > 0) {
									msg(file,line,string("Error: ill-formed expression in preprocessor conditional '" + dir + ".' with the conditional being:\t'" + ccode + ".'"));
									cerr << '\n' << '\n' << '\t';
									cerr << dir << ' ' << code;
									cerr << '\n' << '\n' << '\t';
									caret(dir.length() + 1);
									line = -1;
									exit(1);
								}
								parts.erase(parts.begin() + i,parts.begin() + i + 1);
								parts.insert(parts.begin() + i,eval);
							}
							else if(par > 0) {
								for(int i = 0; i < par; i++)
									now.insert(now.length(),_close_parenthesis);
								now.insert(now.length(),_plus + itos(0));
								expr e(line,file,dir);
								e.set_expr(now);
								string eval = e.use_notation();
								if(e.get_error_counter() > 0) {
									msg(file,line,string("Error: ill-formed expression in preprocessor conditional '" + dir + ".' with the conditional being:\n\t'" + ccode + ".'"));
									line = -1;
									exit(1);
								}
								parts.erase(parts.begin() + i,parts.begin() + i + 1);
								parts.insert(parts.begin() + i,eval);
							}
							else {
								for(int i = 0; i > par; i--)
									now.insert(0,_open_parenthesis);
								now.insert(now.length(),_plus + itos(0));
								expr e(line,file,dir);
								e.set_expr(now);
								string eval = e.use_notation();
								if(e.get_error_counter() > 0) {
									msg(file,line,string("Error: ill-formed expression in preprocessor conditional '" + dir + ".' with the conditional being:\n\t'" + ccode + ".'"));
									line = -1;
									exit(1);
								}
								parts.erase(parts.begin() + i,parts.begin() + i + 1);
								parts.insert(parts.begin() + i,eval);
							}
						}
						string result;
						string fnal;
						int counter = 0;
size_check:
						if(parts.size() == 1) {
							result = parts[0];
							fnal = result;
						}
						else {
							for(int i = 0; i < parts.size(); i++) {
								if(seps[0] == _question_mark &&(seps.size() == 1 || seps[1] != _colon)) {
									msg(file,line,"Error: expected a colon to match the ternary '?' operator.");
									cerr << '\n' << '\n' << '\t';
									cerr << dir << ' ' << code;
									cerr << '\n' << '\n' << '\t';
									caret(dir.length() + 1);
									exit(1);
								}
								else if(seps[0] == _colon) {
									msg(file,line,"Error: expected a question mark to match the ':' ternary operator");
									cerr << '\n' << '\n' << '\t';
									cerr << dir << ' ' << code;
									cerr << '\n' << '\n' << '\t';
									caret(dir.length() + 1);
									exit(1);
								}
								if(seps[0] == _greater_than_or_equal_to) {
									string one = parts[i];
									string two = parts[i + 1];
									if(_stoi(one) >= _stoi(two))
										result += itos(1);
									else
										result += itos(0);
									parts.erase(parts.begin() + i,parts.begin() + i + 2);
									i = -1;
									seps.erase(seps.begin(),seps.begin() + 1);
								}
								else if(seps[0] == _less_than_or_equal_to) {
									string one = parts[i];
									string two = parts[i + 1];
									if(_stoi(one) <= _stoi(two))
										result += itos(1);
									else
										result += itos(0);
									parts.erase(parts.begin() + i,parts.begin() + i + 2);
									i = -1;
									seps.erase(seps.begin(),seps.begin() + 1);
								}
								else if(seps[0] == _equality) {
									string one = parts[i];
									string two = parts[i + 1];
									if(_stoi(one) == _stoi(two))
										result += itos(1);
									else
										result += itos(0);
									parts.erase(parts.begin() + i,parts.begin() + i + 2);
									i = -1;
									seps.erase(seps.begin(),seps.begin() + 1);
								}
								else if(seps[0] == _inequality) {
									string one = parts[i];
									string two = parts[i + 1];
									if(_stoi(one) != _stoi(two))
										result += itos(1);
									else
										result += itos(0);
									parts.erase(parts.begin() + i,parts.begin() + i + 2);
									i = -1;
									seps.erase(seps.begin(),seps.begin() + 1);
								}
								else if(seps[0] == _greater_than) {
									string one = parts[i];
									string two = parts[i + 1];
									if(_stoi(one) > _stoi(two))
										result += itos(1);
									else
										result += itos(0);
									parts.erase(parts.begin() + i,parts.begin() + i + 2);
									i = -1;
									seps.erase(seps.begin(),seps.begin() + 1);
								}
								else if(seps[0] == _less_than) {
									string one = parts[i];
									string two = parts[i + 1];
									if(_stoi(one) < _stoi(two))
										result += itos(1);
									else
										result += itos(0);
									parts.erase(parts.begin() + i,parts.begin() + i + 2);
									i = -1;
									seps.erase(seps.begin(),seps.begin() + 1);
								}
								else if(seps[0] == _and || seps[0] == _or) {
									string one = parts[i];
									string temp;
									if(_stoi(one) > 0)
										temp += itos(1);
									else
										temp += itos(0);
									if(isdigit(result[result.length() - 1]))
										result += seps[0];
									else {
										result += temp + seps[0];
										parts.erase(parts.begin() + i,parts.begin() + i + 1);
									}
									seps.erase(seps.begin(),seps.begin() + 1);
									i = -1;
								}
								else if(seps[0] == _question_mark && seps[1] == _colon) {
									result += parts[0];
									string res = evaluate_basic_bool_expr(result);
									result.clear();
									string temp;
									if(_stoi(res) > 0)
										temp = parts[1];
									else
										temp = parts[2];
									parts.erase(parts.begin() + i,parts.begin() + i + 3);
									parts.insert(parts.begin() + i,temp);
									seps.erase(seps.begin(),seps.begin() + 2);
									i = -1;
									//if this is the only part, rerun.
									goto size_check;
								}
							}
							if(parts.size() > 0 && result.length() == 0) {
								result = parts[0];
							}
							else if(parts.size() > 0 && result.length() != 0) {
								result += parts[0];
							}
							else {}
							fnal = evaluate_basic_bool_expr(result);
						}
						code.erase(index,i - index);
						code.insert(index,fnal);
						i = -1;
					}
				}
			}
			if(_stoi(code) > 0)
				return true;
			else
				return false;
		}

		vector <cpp_token> preprocess(string source, string _file_name) {
			vector<token> v = lex(remove_line_continuations(source));
			vector<cpp_token> vcpp = replace_all_defines(v, _file_name);
			for(int i = 0; i < vcpp.size(); i++) {
				if(vcpp[i].get_id() == token::HELSE || vcpp[i].get_id() == token::HELIF || vcpp[i].get_id() == token::HENDIF) {
					msg(vcpp[i].get_file_name(), vcpp[i].get_line_number(), "Error: cannot start a preprocessor conditional with: '" + vcpp[i].get_qualifier() + ".'\nRegion given here for reference:");
					cerr << '\n' << '\n' << '\t' << vcpp[i].get_qualifier();
					cerr << '\n' << '\n' << '\t';
					caret(1);
					exit(1);
					break;
				}
				if((vcpp[i].wide() == true && vcpp[i].get_id() == token::WQUOTE) &&(vcpp[i + 1].get_id() == token::QUOTE || vcpp[i + 1].get_id() == token::WQUOTE)) {
					string fname = vcpp[i].get_file_name();
					int lnum = vcpp[i].get_line_number();
					string now = vcpp[i].get_qualifier();
					string next = vcpp[i + 1].get_qualifier();
					now.erase(now.length() - 1, now.length());
					next.erase(0, 1);
					now += next;
					if(i + 2 >= vcpp.size()) {
						vcpp.erase(vcpp.begin() + i, vcpp.end());
						vcpp.insert(vcpp.begin() + i, cpp_token(fname,lnum, "@wchar_t*" + now));
					}
					else{
						vcpp.erase(vcpp.begin() + i, vcpp.begin() + i + 2);
						vcpp.insert(vcpp.begin() + i, cpp_token(fname, lnum, "@wchar_t*" + now));
					}
					i--;
				}
				else if(vcpp[i].get_id() == token::QUOTE &&(vcpp[i + 1].get_id() == token::WQUOTE || vcpp[i + 1].get_id() == token::QUOTE)) {
					string fname = vcpp[i].get_file_name();
					int lnum = vcpp[i].get_line_number();
					string now = vcpp[i].get_qualifier();
					string next = vcpp[i + 1].get_qualifier();
					now.erase(now.length() - 1, now.length());
					next.erase(0, 1);
					now += next;
					if(i + 2 >= vcpp.size()) {
						vcpp.erase(vcpp.begin() + i, vcpp.end());
						vcpp.insert(vcpp.begin() + i, cpp_token(fname, lnum,now));
					}
					else{
						vcpp.erase(vcpp.begin() + i, vcpp.begin() + i + 2);
						vcpp.insert(vcpp.begin() + i, cpp_token(fname, lnum,now));
					}
					i--;
				}
			}
			for(int i = 0; i < vcpp.size(); i++) {
				if(vcpp[i].get_id() == token::NEW_LINE) {
					vcpp.erase(vcpp.begin() + i, vcpp.begin() + i + 1);
					i--;
				}
			}
			for(int i = 0; i < vcpp.size(); i++) {
				if(vcpp[i].get_id() == token::QUOTE || vcpp[i].get_id() == token::WQUOTE) {
					string comp = vcpp[i].get_qualifier();
					if(comp[0] == comp[comp.length() - 1] && comp[0] == '\"' && comp[comp.length() - 1] == '\"');
					else {
						msg(vcpp[i].get_file_name(), vcpp[i].get_line_number(), "Error: misformed string-literal.\nLine given here for reference:");
						cerr << '\n' << '\n' << '\t';
						int l = vcpp[i].get_line_number();
						int _caret = 0;
						for(int j = i; j < i + 1; j++) {
							cerr << vcpp[j].get_qualifier() << ' ';
							_caret += vcpp[j].get_qualifier().length() + 1;
						}
						cerr << '\n' << '\n' << '\t';
						_caret -= 2;
						caret(_caret);
						exit(1);
						break;
					}
				}
				else if(vcpp[i].get_id() == token::CHARACTER || vcpp[i].get_id() == token::WCHARACTER) {
					string comp = vcpp[i].get_qualifier();
					if(comp[0] == comp[comp.length() - 1] && comp[0] == '\'' && comp[comp.length() - 1] == '\'');
					else {
						msg(vcpp[i].get_file_name(), vcpp[i].get_line_number(), "Error: misformed raw character.\nLine given here for reference:");
						cerr << '\n' << '\n' << '\t';
						int l = vcpp[i].get_line_number();
						if(vcpp[i].get_id() == token::WCHARACTER) cerr << 'L';
						int _caret = 0;
						for(int j = i; j < i + 1; j++) {
							cerr << vcpp[j].get_qualifier() << ' ';
							_caret += vcpp[j].get_qualifier().length() + 1;
						}
						cerr << '\n' << '\n' << '\t';
						if(vcpp[i].get_id() == token::WCHARACTER) _caret -= 1;
						else _caret -= 2;
						caret(_caret);
						exit(1);
						break;
					}
				}
			}
			return vcpp;
		}
	}
}
