karma is a set of files that provides tools for C code compilation.
Currently only supports Visual Studio; however, removing the "stdafx.h" headers makes the code cross platform.
The debug configuration throws an exception; however, the release does work.
There may be bugs in the parser; however, considering a wide array of use-cases need to be covered, sometimes, for a specific configuration, it may fail.
The preprocessor has been heavily tested, and works with most cases except those listed in the preprocess.{h,cpp} files listed.

IMPORTANT: 
  Please define these functions: (parser will break without it) 

shared_ptr<c_type_name> parse_type_name(shared_ptr<c_parser> parser); 
shared_ptr<c_expression> parse_compound_literal(shared_ptr<c_parser> parser); 
shared_ptr<c_statement> parse_compound_statement(shared_ptr<c_parser> parser); 
shared_ptr<c_expression> parse_initializer(shared_ptr<c_parser> parser); 
c_type_kind determine_declaration_specifier_list_type_kind(vector<shared_ptr<c_declaration_specifier>> declspec_list); 
vector<shared_ptr<c_expression>> query_identifier(shared_ptr<c_c_declaration> decl); 

In order to run this program's preprocessor, just compile the program in Visual Studio.
By changing the string in the main source file, the source code will be preprocessed, any errors will be emitted, and the individual tokens will be shown.
