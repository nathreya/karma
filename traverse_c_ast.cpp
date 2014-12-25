#include "stdafx.h"
#include "traverse_c_ast.h"

namespace __karma {
	namespace __ast {
		vector<shared_ptr<c_expression>> query_identifier(shared_ptr<c_c_declaration> decl) {
			//user-defined for custom lookup
			vector<shared_ptr<c_expression>> ret;
			ret.push_back(nullptr);
			return ret;
		}
	}
}