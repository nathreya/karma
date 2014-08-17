#include "stdafx.h"
#include "graphs.h"

namespace __karma {
	namespace __lex {
		namespace __token {
			string const _trigraph_hash = "\?\?=";
			string const _trigraph_escape_character = "\?\?/";
			string const _trigraph_open_bracket = "\?\?(";
			string const _trigraph_close_bracket = "\?\?)";
			string const _trigraph_single_or = "\?\?!";
			string const _trigraph_open_brace = "\?\?<";
			string const _trigraph_close_brace = "\?\?>";
			string const _trigraph_cmpl = "\?\?-";
			string const _digraph_open_bracket = "<:";
			string const _digraph_close_bracket = ":>";
			string const _digraph_open_brace = "<%";
			string const _digraph_close_brace = "%>";
			string const _digraph_hash = "%:";
			string const _digraph_paste = "%:%:";
		}

		string remove_trigraphs(string source) {
			source = replace_all(source,_trigraph_hash,_hash);
			source = replace_all(source,_trigraph_escape_character,_escape_character);
			source = replace_all(source,_trigraph_open_bracket,_open_bracket);
			source = replace_all(source,_trigraph_close_bracket,_close_bracket);
			source = replace_all(source,_trigraph_single_or,_single_or);
			source = replace_all(source,_trigraph_open_brace,_open_brace);
			source = replace_all(source,_trigraph_close_brace,_close_brace);
			source = replace_all(source,_trigraph_cmpl,_cmpl);
			return source;
		}
		string remove_digraphs(string source) {
			source = replace_all(source,_digraph_open_bracket,_open_bracket);
			source = replace_all(source,_digraph_close_bracket,_close_bracket);
			source = replace_all(source,_digraph_open_brace,_open_brace);
			source = replace_all(source,_digraph_close_brace,_close_brace);
			source = replace_all(source,_digraph_paste,_paste);
			source = replace_all(source,_digraph_hash,_hash);
			return source;
		}
		string remove_graphs(string source) {
			source = remove_trigraphs(source);
			source = remove_digraphs(source);
			return source;
		}
	}
}
#undef LZZ_INLINE
