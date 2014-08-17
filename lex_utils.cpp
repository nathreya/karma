#include "stdafx.h"
#include "lex_utils.h"

namespace __karma {
	namespace __util {
		vector <string>& split(string toParse, char delim, vector <string>& elements) {
			stringstream ss(toParse);
			string item;
			while (std::getline(ss, item, delim))
				elements.push_back(item);

			return elements;
		}

		vector <string> split(string toParse, char delim) {
			vector<string> elements;
			return split(toParse, delim, elements);
		}

		string clean_space(string str) {
#if defined _WIN32 && !defined MINGW32
			char* s = _strdup(str.c_str());
#else
			char* s = strdup(str.c_str());
#endif
			int i = 0, j = 0,
				first = 0,
				last = 0;
			for (i = 0; s[i] != '\0'; i++) {
				if (s[i] != '\n' && s[i] != 32 && s[i] != 9 && s[i] != 10) {
					first = i;
					break;
				}
			}
			for (i = strlen(s) - 1; i > first; i--) {
				if (s[i] != '\n' && s[i] != 32 && s[i] != 9 && s[i] != 10) {
					last = i;
					break;
				}
			}
			for (i = 0; first <= last; i++) {
				s[i] = s[first];
				first++;
			}
			s[i] = '\0';
			for (i = 0; i < strlen(s) + 1; i++) {
				if (s[i] != '\n' && s[i] >= 9 && s[i] <= 10) {
					s[i] = 32;
				}
			}
			i = 0;
			while (s[i] != '\0') {
				if (s[i] != '\n' && s[i] == 32 && s[i + 1] == 32) {
					for (j = i; s[j] != '\0'; j++) {
						s[j] = s[j + 1];
					}
				}
				else {
					i++;
				}
			}
			string ret(s);
			return ret;
		}

		void leading_trim(string& str, locale const& loc) {
			string::size_type pos = 0;
			while (pos < str.size() && isspace(str[pos], loc)) pos++;
			str.erase(0, pos);
		}

		void trailing_trim(string& str, locale const& loc) {
			string::size_type pos = str.size();
			while (pos > 0 && isspace(str[pos - 1], loc)) pos--;
			str.erase(pos);
		}

		void trim(string& str, locale const& loc) {
			leading_trim(str, loc);
			trailing_trim(str, loc);
		}

		int _stoi(string str) {
			istringstream convert(str);
			int ret;
			convert >> ret;
			return ret;
		}

		string itos(int value) {
			string s;
			stringstream ss;
			ss << value;
			s = ss.str();
			return s;
		}

		string replace_all(string toReplace, string find, string replace) {
			int pos;
			do {
				pos = toReplace.find(find);
				if (pos != -1)  toReplace.replace(pos, find.length(), replace);
			} while (pos != -1);
			return toReplace;
		}

		bool ends_with(string& toSearch, string& toCheck) {
			if (toSearch.length() >= toCheck.length()) {
				return(0 == toSearch.compare(toSearch.length() - toCheck.length(), toCheck.length(), toCheck));
			}
			else {
				return false;
			}
		}
#if defined _WIN32
		void msg(string file, int line, string msg) {
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			cerr << file << "(" << itos(line) << ") ";
			vector<string> message = split(msg, ' ');
			for (int i = 0; i < message.size(); i++) {
				if (i == 0) {
					SetConsoleTextAttribute(hOut, 0x0C);
					cerr << message[i] << ' ';
					SetConsoleTextAttribute(hOut, 07);
				}
				else cerr << message[i] << ' ';
			}
		}

		void note(string file, int line, string msg) {
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			cerr << file << "(" << itos(line) << ") ";
			vector<string> message = split(msg, ' ');
			for (int i = 0; i < message.size(); i++) {
				if (i == 0) {
					SetConsoleTextAttribute(hOut, 01);
					cerr << message[i] << ' ';
					SetConsoleTextAttribute(hOut, 07);
				}
				else cerr << message[i] << ' ';
			}
		}

		void warn(string file, int line, string msg) {
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			cerr << file << "(" << itos(line) << ") ";
			vector<string> message = split(msg, ' ');
			for (int i = 0; i < message.size(); i++) {
				if (i == 0) {
					SetConsoleTextAttribute(hOut, 0x0d);
					cerr << message[i] << ' ';
					SetConsoleTextAttribute(hOut, 07);
				}
				else cerr << message[i] << ' ';
			}
		}

		void caret(int pos) {
			HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			for (int i = 0; i < pos; i++) {
				cerr << ' ';
				if (i == pos - 1) {
					SetConsoleTextAttribute(hOut, 0x0A);
					cerr << '^';
					SetConsoleTextAttribute(hOut, 07);
					return;
				}
			}
		}
#elif defined __MINGW32__
		void msg(string file, int line, string msg) {
			cerr << file << "(" << __itos(line) << ") ";
			vector<string> message = __split(msg, ' ');
			for(int i = 0; i < message.size(); i++) {
				if(i == 0) {
					cout << message[i] << ' ';
				}
				else cerr << message[i] << ' ';
			}
		}

		void note(string file, int line, string msg) {
			cerr << file << "(" << __itos(line) << ") ";
			vector<string> message = __split(msg, ' ');
			for (int i = 0; i < message.size(); i++) {
				if (i == 0) {
					cout << message[i] << ' ';
				}
				else cerr << message[i] << ' ';
			}
		}

		void warn(string file, int line, string msg) {
			cerr << file << "(" << __itos(line) << ") ";
			vector<string> message = __split(msg, ' ');
			for(int i = 0; i < message.size(); i++) {
				if(i == 0) {
					cout << message[i] << ' ';
				}
				else cerr << message[i] << ' ';
			}
		}

		void caret(int pos) {
			for(int i = 0; i < pos; i++) {
				cerr << ' ';
				if(i == pos - 1) {
					cout << '^';
					return;
				}
			}
		}
#else
		void msg(string file, int line, string msg) {
			cerr << file << "(" << itos(line) << ") ";
			vector<string> message = split(msg, ' ');
			for(int i = 0; i < message.size(); i++) {
				if(i == 0) {
					cerr << "\033[31m" << message[i] << ' ';
					cerr << "\033[0m";
				}
				else cerr << message[i] << ' ';
			}
		}

		void note(string file, int line, string msg) {
			cerr << file << "(" << itos(line) << ") ";
			vector<string> message = split(msg, ' ');
			for(int i = 0; i < message.size(); i++) {
				if(i == 0) {
					cerr << "\033[35m" << message[i] << ' ';
					cerr << "\033[0m";
				}
				else cerr << message[i] << ' ';
			}
		}

		void warn(string file, int line, string msg) {
			cerr << file << "(" << itos(line) << ") ";
			vector<string> message = split(msg, ' ');
			for(int i = 0; i < message.size(); i++) {
				if(i == 0) {
					cerr << "\033[35m" <<  message[i] << ' ';
					cerr << "\033[0m";
				}
				else cerr << message[i] << ' ';
			}
		}

		void caret(int pos) {
			for(int i = 0; i < pos; i++) {
				cerr << ' ';
				if(i == pos - 1) {
					cerr << "\033[32m" << '^' << "\033[0m";
					return;
				}
			}
		}
#endif
	}
}
