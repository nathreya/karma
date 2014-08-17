#include "stdafx.h"
#include "expr.h"

namespace __karma {
	namespace __util {

		int expr::get_error_counter() { return ecounter; }

		expr::expr(int & l, string f, string d)
			: line(l), file(f), dir(d), ecounter(0) {}

		void expr::set_expr(string e) {
			pExpresion =
#ifdef WIN32
				_strdup(e.c_str());
#else
			strdup(e.c_str());
#endif
		}

		string expr::use_notation() {
			int culmination;
			nextToken();
			if(!*token) {
				serror(2);
				return string("");
			}
			recursive_3(culmination);
			if(*token) {serror(0);	return ""; }
			long cul = culmination;
			return itos((int) cul);
		}

		void expr::recursive_3(int & culmination) {
			register char curr;
			int bucket;
			recursive_2(culmination);
			if((curr = *token) == '|') {
				nextToken();
				recursive_2(bucket);
				switch(curr) {
					case '|':
						culmination = culmination | bucket;
						break;
				}
			}
		}

		void expr::recursive_2(int & culmination) {
			register char curr;
			int bucket;
			recursive_1(culmination);
			if((curr = *token) == '^') {
				nextToken();
				recursive_1(bucket);
				switch(curr) {
					case '^':
						culmination = culmination ^ bucket;
						break;
				}
			}
		}

		void expr::recursive_1(int & culmination) {
			register char curr;
			int bucket;
			recursive0(culmination);
			if((curr = *token) == '&') {
				nextToken();
				recursive0(bucket);
				switch(curr) {
					case '&':
						culmination = culmination & bucket;
						break;
				}
			}
		}

		void expr::recursive0(int & culmination) {
			register char curr;
			int bucket;
			recursive1(culmination);
			while((curr = *token) == '>' || curr == '<') {
				char prev = curr;
				nextToken();
				if(((curr = *token) == '>' && prev == '>') ||(curr == '<' && prev == '<')) {
					nextToken();
					recursive1(bucket);
					switch(curr) {
						case '>':
							culmination = culmination >> bucket;
							break;
						case '<':
							culmination = culmination << bucket;
							break;
					}
				}
			}
		}

		void expr::recursive1(int & culmination) {
			register char curr;
			int bucket;
			recursive2(culmination);
			while((curr = *token) == '+' || curr == '-') {
				nextToken();
				recursive2(bucket);
				switch(curr) {
					case '-':
						culmination = culmination - bucket;
						break;
					case '+':
						culmination = culmination + bucket;
						break;
				}
			}
		}

		void expr::recursive2(int & culmination) {
			register char curr;
			int bucket;
			recursive3(culmination);
			while((curr = *token) == '*' || curr == '/' || curr == '%') {
				nextToken();
				recursive3(bucket);
				switch(curr) {
					case '*':
						culmination = culmination * bucket;
						break;
					case '/':
						culmination = culmination / bucket;
						break;
					case '%':
						culmination =(int) culmination %(int) bucket;
						break;
				}
			}
		}

		void expr::recursive3(int & culmination) {
			register char curr;
			curr = 0;
			if((tokenType == DELIMITER) && *token == '~' || *token == '~') {
				curr = *token;
				nextToken();
			}
			recursive4(culmination);
			if(curr == '~') culmination = ~culmination;
		}

		void expr::recursive4(int & culmination) {
			register char  curr;
			curr = 0;
			if((tokenType == DELIMITER) && *token == '+' || *token == '-') {
				curr = *token;
				nextToken();
			}
			recursive5(culmination);
			if(curr == '-') culmination = -culmination;
		}

		void expr::recursive5(int & culmination) {
			if((*token == '(')) {
				nextToken();
				recursive_3(culmination);
				if(*token != ')') {
					serror(1);
					return;
				}
				nextToken();
			}
			else bomb(culmination);
		}

		void expr::bomb(int & culmination) {
			switch(tokenType) {
				case NUMBER:
					culmination = atof(token);
					nextToken();
					return;
				default:
					serror(0);
			}
		}

		void expr::serror(int error) {
			string e[]=  {
				"Error: incorrect syntax of conditional in the preprocessor directive '" + dir + ".'",
				"Error: parenthesis not balanced in the conditional preprocessor directive '" + dir + ".'",
				"Error: no expression present in the conditional preprocessor directive '" + dir + ".'",
			};
			ecounter++;
			pExpresion = "";
			return;
		}

		void expr::nextToken() {
			register char *bucket;
			tokenType = 0;
			bucket = token;
			*bucket = '\0';
			if(!*pExpresion) return;
			while(isspace(*pExpresion)) ++pExpresion;
			if(strchr("+-~*/%^&|()<>", *pExpresion)) {
				tokenType = DELIMITER;
				*bucket++ = *pExpresion++;
			}
			else if(isdigit(*pExpresion)) {
				while(!isdelim(*pExpresion)) *bucket++ = *pExpresion++;
				tokenType = NUMBER;
			}
			*bucket = '\0';
		}

		int expr::isdelim(char c) {
			if(strchr(" +-~/*%^&|()<>", c) || c == 9 || c == '\r' || c == 0)
				return 1;
			return 0;
		}

		expr::~ expr() {}
	}
}
