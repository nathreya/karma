#include "stdafx.h"
#include "lex.h"
#include "lex_utils.h"

namespace __karma {
	namespace __lex {
		string remove_line_continuations(string source) {
			vector<string> tpsplt;
			int index = 0;
			string ret;
			for(int i = 0; i < source.length(); i++) {
				if(source.substr(i,_new_line.length()) == _new_line) {

					string ins = source.substr(index,i - index);
					tpsplt.push_back(ins);
					index = i + 1;
				}
			}
			tpsplt.push_back(source.substr(index,source.length()));
			int counter = 0;
			for(int i = 0; i < tpsplt.size(); i++) {
				string tos = tpsplt[i];
				trim(tos);
				string ending("\\");
				if(ends_with(tos,ending)) {
					int c = 0;
					while(ends_with(tos,ending)) {
						tos = tpsplt[i];
						if(ends_with(tos,ending)) {
							ret.insert(ret.length(),tos.substr(0,tos.length() - 1));
						}
						else {

							ret.insert(ret.length(),tos);
							c++;
						}

						i++;
						counter++;
					}
					for(int i = 0; i < counter; i++)
						ret += _new_line;
					counter = 0;
					i -= c;
				}	

				else if(!ends_with(tos,ending)) {

					ret.insert(ret.length(),tos + _new_line);
				}
			}

			return ret;
		}
		vector <token> lex(string source) {
			vector<string> ret;
			for(int i = 0; i < source.length(); i++) {
				switch(source[i]) {
					case '?':
						ret.push_back("?");
						break;
					case ':':
						ret.push_back(":");
						break;
					case '~':
						ret.push_back("~");
						break;
					case '(':
						ret.push_back("(");
						break;
					case ')':
						ret.push_back(")");
						break;
					case '[':
						ret.push_back("[");
						break;
					case ']':
						ret.push_back("]");
						break;
					case '{':
						ret.push_back("{");
						break;
					case '}':
						ret.push_back("}");
						break;
					case ',':
						ret.push_back(",");
						break;
					case ';':
						ret.push_back(";");
						break;
					case '\n':
						ret.push_back("\n");
						break;
					case '`':
						ret.push_back("`");
						break;
					case '@':
						ret.push_back("@");
						break;
					case '$':
						ret.push_back("$");
						break;
					case '\\':
						ret.push_back("\\");
						break;
					case '\'':
						if(source[i] == '\'') {
							i++;
							string ident = "'";
							while(source[i] != '\'' && source[i] != '\n' && i < source.length()) {
								if(source[i] == '\\' && source[i + 1] == '\\') {
									ident += string(1, source[i]); 
									i++;
								}
								else if(source[i] == '\\' && source[i + 1] == '\'') {
									ident += string(1,source[i]);
									i++;
								}
								ident += string(1,source[i]);
								i++;
							}
							if(source[i] == '\'')
								ident += "\'";
							ret.push_back(ident);
						}
						break;
					case '\"':
						if(source[i] == '\"') {
							i++;
							string ident = "\"";
							while(source[i] != '\"' && source[i] != '\n' && i < source.length()) {
								if(source[i] == '\\' && source[i + 1] == '\\') {
									ident += string(1, source[i]);
									i++;
								}
								else if(source[i] == '\\' && source[i + 1] == '\"') {
									ident += string(1,source[i]);
									i++;
								}
								ident += string(1,source[i]);
								i++;
							}
							if(source[i] == '\"')
								ident += "\"";
							ret.push_back(ident);
						}
						break;
					case '+':
						if(source[i + 1] == '=') {
							ret.push_back("+=");
							i++;
						}
						else if(source[i + 1] == '+') {
							ret.push_back("++");
							i++;
						}
						else
							ret.push_back("+");
						break;
					case '-':
						if(source[i + 1] == '=') {
							ret.push_back("-=");
							i++;
						}
						else if(source[i + 1] == '-') {
							ret.push_back("--");
							i++;
						}
						else if(source[i + 1] == '>') {
							ret.push_back("->");
							i++;
						}
						else
							ret.push_back("-");
						break;
					case '*':
						if(source[i + 1] == '=') {
							ret.push_back("*=");
							i++;
						}
						else
							ret.push_back("*");
						break;
					case '/':
						if(source[i + 1] == '/') {
							while(source[i] != '\n' && i != source.length())
								i++;
							i--;
						}
						else if(source[i + 1] == '*') {
							i += 2;
							int ncounter = 0;
							while(source.substr(i,2) != "*/" && i != source.length()) {
								if(source[i] == '\n')
									ncounter++;
								i++;
							}
							for(int i = 0; i < ncounter; i++)
								ret.push_back("\n");
							i++;
						}
						else if(source[i + 1] == '=') {
							ret.push_back("/=");
							i++;
						}
						else
							ret.push_back("/");
						break;
					case '%':
						if(source[i + 1] == '=') {
							ret.push_back("%=");
							i++;
						}
						else
							ret.push_back("%");
						break;
					case '^':
						if(source[i + 1] == '=') {
							ret.push_back("^=");
							i++;
						}
						else
							ret.push_back("^");
						break;
					case '|':
						if(source[i + 1] == '=') {
							ret.push_back("|=");
							i++;
						}
						else if(source[i + 1] == '|') {
							ret.push_back("||");
							i++;
						}
						else
							ret.push_back("|");
						break;
					case '&':
						if(source[i + 1] == '=') {
							ret.push_back("&=");
							i++;
						}
						else if(source[i + 1] == '&') {
							ret.push_back("&&");
							i++;
						}
						else
							ret.push_back("&");
						break;
					case '=':
						if(source[i + 1] == '=') {
							ret.push_back("==");
							i++;
						}
						else 
							ret.push_back("=");
						break;
					case '!':
						if(source[i + 1] == '=') {
							ret.push_back("!=");
							i++;
						}
						else
							ret.push_back("!");
						break;
					case '>':
						if(source[i + 1] == '>') {
							if(source[i + 2] == '=') {
								ret.push_back(">>=");
								i += 2;
							}
							else {
								ret.push_back(">>");
								i++;
							}
						}
						else if(source[i + 1] == '=') {
							ret.push_back(">=");
							i++;
						}
						else
							ret.push_back(">");
						break;
					case '<':
						if(source[i + 1] == '<') {
							if(source[i + 1] == '<') {
								if(source[i + 2] == '=') {
									ret.push_back("<<=");
									i += 2;
								}
								else {
									ret.push_back("<<");
									break;
								}
							}
						}
						else if(source[i + 1] == '=') {
							ret.push_back("<=");
							i++;
						}
						else if(i != 0 && ret[ret.size() - 1] == _hinclude) {
							ret.push_back("<");
							i++;
							string ident;
							while(source[i] != '>' && source[i] != '\n') {
								ident.insert(ident.length(),string(1,source[i]));
								i++;
							}
							ret.push_back(ident);
							i--;
						}
						else
							ret.push_back("<");
						break;
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						//hexadecimal number
						if(source[i] == '0' &&(source[i + 1] == 'x' || source[i + 1] == 'X')) {
							string ident;
							i += 2;
							string atf = "abcdefABCDEF";
							while(isdigit(source[i]) ||(int) atf.find(source[i]) > 0) {
								ident.insert(ident.length(),string(1,source[i]));
								i++;
							}
							std::stringstream ss;
							ss << std::hex << ident;
							int r;
							ss >> r;
							ret.push_back(itos(r));
							i--;
						}
						else {
							string ident;
							bool encountered = false;
							while(source[i] == '.' || source[i] == 'p' || source[i] == 'P' || source[i] == 'e' || source[i] == 'E' || isdigit(source[i]) || source.substr(i, 3) == "ull"
									|| source.substr(i, 2) == "LL" || source.substr(i, 2) == "ll" ||(int) _suffixes.find(source[i]) >= 0) {
								if(source.substr(i, 3) == "ull" || source.substr(i, 2) == "ll" || source.substr(i, 3) == "LL" ||(int) _suffixes.find(source[i]) >= 0) {
									if(source.substr(i, 3) == "ull") {
										ident.insert(ident.length(), " ull");
										i += 3;
									}
									else if(source.substr(i, 2) == "LL" || source.substr(i, 2) == "ll") {
										ident.insert(ident.length(), " " + source.substr(i, 2));
										i += 2;
									}
									else {
										ident.insert(ident.length(), " " + source.substr(i, 1));
										i++;
									}
									break;
								}
								else if(string(1,source[i]) == _p || string(1,source[i]) == _P || string(1,source[i]) == _e || string(1,source[i]) == _E) {
									if(!encountered) encountered = true;
									else break;
									if(string(1,source[i + 1]) == _plus || string(1,source[i + 1]) == _minus) {

										ident.insert(ident.length(),string(1,source[i]) + string(1,source[i + 1]));
										i++;
									}
									else
										ident.insert(ident.length(),string(1,source[i]));
								}
								else
									ident.insert(ident.length(),string(1,source[i]));
								i++;
							}		
							ret.push_back(ident);
							i--;
						}
						break;
					case '.':
						if(source[i + 1] == '.' && source[i + 2] == '.') {
							ret.push_back("...");
							i += 2;
						}
						else if(isdigit(source[i + 1])) {
							i++;
							string ident;
							bool encountered = false;
							while(source[i] == 'p' || source[i] == 'P' || source[i] == 'e' || source[i] == 'E' || isdigit(source[i]) || source.substr(i, 3) == "ull"
									|| source.substr(i, 2) == "LL" || source.substr(i, 2) == "ll" ||(int) _suffixes.find(source[i]) >= 0) {
								if(source.substr(i, 3) == "ull" || source.substr(i, 2) == "ll" || source.substr(i, 3) == "LL" ||(int) _suffixes.find(source[i]) >= 0) {
									if(source.substr(i, 3) == "ull") {
										ident.insert(ident.length(), " ull");
										i += 3;
									}
									else if(source.substr(i, 2) == "LL" || source.substr(i, 2) == "ll") {
										ident.insert(ident.length(), " " + source.substr(i, 2));
										i += 2;
									}
									else {
										ident.insert(ident.length(), " " + source.substr(i, 1));
										i++;
									}
									break;
								}
								else if(string(1, source[i]) == _p || string(1, source[i]) == _P || string(1, source[i]) == _e || string(1, source[i]) == _E) {
									if(!encountered) encountered = true;
									else break;
									if(string(1, source[i + 1]) == _plus || string(1, source[i + 1]) == _minus) {
										ident.insert(ident.length(), string(1, source[i]) + string(1, source[i + 1]));
										i++;
									}
									else
										ident.insert(ident.length(), string(1, source[i]));
								}
								else
									ident.insert(ident.length(), string(1, source[i]));
								i++;
							}
							ret.push_back(itos(0) + _dot + ident);
							i--;
						}
						else
							ret.push_back(".");
						break;
					case '#': 
						if(source[i + 1] == '#') {
							ret.push_back("##");
							i++;
						}
						else {
							if(source.substr(i,_hdefine.length()) == _hdefine &&(source[i + _hdefine.length()] != '_' && !isalnum(source[i + _hdefine.length()]))) {
								ret.push_back(_hdefine);
								i += _hdefine.length() - 1;
							}
							else if(source.substr(i, _hwarning.length()) == _hwarning &&(source[i + _hwarning.length()] != '_' && !isalnum(source[i + _hwarning.length()]))) {
								ret.push_back(_hwarning);
								i += _hwarning.length() - 1;
							}
							else if(source.substr(i,_hif.length()) == _hif &&(source[i + _hif.length()] != '_' && !isalnum(source[i + _hif.length()]))) {
								ret.push_back(_hif);
								i += _hif.length() - 1;
							}
							else if(source.substr(i,_helif.length()) == _helif &&(source[i + _helif.length()] != '_' && !isalnum(source[i + _helif.length()]))) {
								ret.push_back(_helif);
								i += _helif.length() - 1;
							}
							else if(source.substr(i,_hifdef.length()) == _hifdef &&(source[i + _hifdef.length()] != '_' && !isalnum(source[i + _hifdef.length()]))) {
								ret.push_back(_hifdef);
								i += _hifdef.length() - 1;
							}
							else if(source.substr(i,_hifndef.length()) == _hifndef &&(source[i + _hifndef.length()] != '_' && !isalnum(source[i + _hifndef.length()]))) {
								ret.push_back(_hifndef);
								i += _hifndef.length() - 1;
							}
							else if(source.substr(i,_helse.length()) == _helse &&(source[i + _helse.length()] != '_' && !isalnum(source[i + _helse.length()]))) {
								ret.push_back(_helse);
								i += _helse.length() - 1;
							}
							else if(source.substr(i,_hendif.length()) == _hendif &&(source[i + _hendif.length()] != '_' && !isalnum(source[i + _hendif.length()]))) {
								ret.push_back(_hendif);
								i += _hendif.length() - 1;
							}
							else if(source.substr(i,_hpragma.length()) == _hpragma &&(source[i + _hpragma.length()] != '_' && !isalnum(source[i + _hpragma.length()]))) {
								ret.push_back(_hpragma);
								i += _hpragma.length() - 1;
							}
							else if(source.substr(i,_hline.length()) == _hline &&(source[i + _hline.length()] != '_' && !isalnum(source[i + _hline.length()]))) {
								ret.push_back(_hline);
								i += _hline.length() - 1;
							}
							else if(source.substr(i,_hinclude.length()) == _hinclude &&(source[i + _hinclude.length()] != '_' && !isalnum(source[i + _hinclude.length()]))) {
								ret.push_back(_hinclude);
								i += _hinclude.length() - 1;
							}
							//else if(source.substr(i,_himport.length()) == _himport &&(source[i + _himport.length()] != '_' && !isalnum(source[i + _himport.length()]))) {
							//ret.push_back(_himport);
							//i += _himport.length() - 1;
							//}
							//else if(source.substr(i,_husing.length()) == _husing &&(source[i + _husing.length()] != '_' && !isalnum(source[i + _husing.length()]))) {
							//ret.push_back(_husing);
							//i += _husing.length() - 1;
							//}
							else if(source.substr(i,_herror.length()) == _herror &&(source[i + _herror.length()] != '_' && !isalnum(source[i + _herror.length()]))) {
								ret.push_back(_herror);
								i += _herror.length() - 1;
							}
							else if(source.substr(i,_hundef.length()) == _hundef &&(source[i + _hundef.length()] != '_' && !isalnum(source[i + _hundef.length()]))) {
								ret.push_back(_hundef);
								i += _hundef.length() - 1;
							}
							else {
								ret.push_back(_hash);
								i += _hash.length() - 1;
							}
						}
						break;
					case ' ':
						break;
					case '\t':
						break;
					default:
						//for wide characters
						if(source[i] == 'L' && source[i + 1] == '\'') {
							i += 2;
							string ident = "\'";
							while(source[i] != '\'' && source[i] != '\n' && i != source.length()) {
								if(source[i] == '\\' && source[i + 1] == '\\') {
									ident += string(1, source[i]);
									i++;
								}
								else if(source[i] == '\\' && source[i + 1] == '\'') {
									ident += string(1,source[i]);
									i++;
								}
								ident += string(1,source[i]);
								i++;
							}
							if(source[i] == '\'')
								ident += "\'";
							ret.push_back("@wchar_t " + ident);
						}
						else if(source[i] == 'L' && source[i + 1] == '\"') {
							i += 2;
							string ident = "\"";
							while(source[i] != '\"' && source[i] != '\n' && i != source.length()) {
								if(source[i] == '\\' && source[i + 1] == '\\') {
									ident += string(1, source[i]);
									i++;
								}
								else if(source[i] == '\\' && source[i + 1] == '\"') {
									ident += string(1,source[i]);
									i++;
								}
								ident += string(1,source[i]);
								i++;
							}
							if(source[i] == '\"')
								ident += "\"";
							ret.push_back("@wchar_t*"+ ident);
						}
						string ident;
						while((source[i] == '_' || isalnum(source[i])) && i != source.length()) {
							ident.insert(ident.length(),string(1,source[i]));
							i++;
						}
						trim(ident);
						if(ident.length() != 0) {
							ret.push_back(ident);
							i--;
						}
						else {}
						break;
				}
			}
			vector<token> tret;
			for(int i = 0; i < ret.size(); i++) {
				tret.push_back(token(ret[i]));
			}
			return tret;
		}
	}
}
