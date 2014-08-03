#include <iostream>
#include <string>


using namespace std;

void output_header_and_beginning(){
    cout << 
"// Adapted from http://stackoverflow.com/questions/11920577/casting-all-parameters-passed-in-macro-using-va-args\n"
"\n"
"// With adaptations for MSVC from BOOST_PP\n"
"\n"
"\n"
"/* This will let macros expand before concating them */\n"
"#define INTERNAL_MACRO_CPPCOMPONENTS_PRIMITIVE_CAT(x, y) x ## y\n"
"#define INTERNAL_MACRO_CPPCOMPONENTS_CAT(x, y) INTERNAL_MACRO_CPPCOMPONENTS_PRIMITIVE_CAT(x, y)\n"
"\n"
"#define INTERNAL_MACRO_CPPCOMPONENTS_STR(x) #x\n"
;

}

void output_nargs_seq(int n){
    cout << "#define INTERNAL_MACRO_CPPCOMPONENTS_NARGS_SEQ(";
    for(int i = 0; i< n; ++i){
        cout << "_" << (i+1);
        if(i < (n-1)) cout << ",";
    }
    cout << ",N,...) N";


}

void output_nargs_part_seq(int n){
    cout << "INTERNAL_MACRO_CPPCOMPONENTS_NARGS_SEQ(__VA_ARGS__,";
    for(int i = n; i >= 0; --i){
        cout << i;
        if( i > 0) cout << ", ";
    }
    cout << ")";

}

void output_nargs(int n){

    output_nargs_seq(n);
    cout << "\n";
    cout << "#ifdef _MSC_VER\n"
        << "#define INTERNAL_MACRO_CPPCOMPONENTS_NARGS(...) INTERNAL_MACRO_CPPCOMPONENTS_CAT(";
    output_nargs_part_seq(n);
    cout << ",)\n";
    cout << "#else\n"
        << "#define INTERNAL_MACRO_CPPCOMPONENTS_NARGS(...) ";
    output_nargs_part_seq(n);
    cout << "\n#endif\n";


}

void output_apply(std::string s){
    cout << 
        "/* This will call a macro on each argument passed in */\n"
        "#ifdef _MSC_VER\n"
        "#define cppcomponents_" + s + "APPLY(T,macro, ...) INTERNAL_MACRO_CPPCOMPONENTS_CAT(cppcomponents_" + s+ "APPLY_, INTERNAL_MACRO_CPPCOMPONENTS_NARGS(__VA_ARGS__))INTERNAL_MACRO_CPPCOMPONENTS_CAT((T, macro, __VA_ARGS__),)\n"
        "#else\n"
        "#define cppcomponents_" + s + "APPLY(T,macro, ...) INTERNAL_MACRO_CPPCOMPONENTS_CAT(cppcomponents_" + s + "APPLY_, INTERNAL_MACRO_CPPCOMPONENTS_NARGS(__VA_ARGS__))(T, macro, __VA_ARGS__)\n"
        "#endif\n";

}

void output_apply_n(int n){

    cout << "#define INTERNAL_MACRO_CPPCOMPONENTS_APPLY_" << n << "(T,m,";
    for(int i = 0; i < n; ++i){
        cout << "x" << (i+1);
        if(i < (n-1)) cout << ", ";
    }
    cout << ") ";
    for(int i = 0; i < n; ++i){
        cout << "m(T," << (i+1) << ", x" << (i+1) << ")";
        if( i < (n-1) ) cout << ", ";
    }
}
void output_apply_n_semicolon(int n){

	cout << "#define INTERNAL_MACRO_CPPCOMPONENTS_SEMICOLON_APPLY_" << n << "(T,m,";
	for (int i = 0; i < n; ++i){
		cout << "x" << (i + 1);
		if (i < (n - 1)) cout << ", ";
	}
	cout << ") ";
	for (int i = 0; i < n; ++i){
		cout << "m(T," << (i + 1) << ", x" << (i + 1) << ") ;";
	}
}


void output_apply_n_space(int n){

	cout << "#define INTERNAL_MACRO_CPPCOMPONENTS_SPACE_APPLY_" << n << "(T,m,";
	for (int i = 0; i < n; ++i){
		cout << "x" << (i + 1);
		if (i < (n - 1)) cout << ", ";
	}
	cout << ") ";
	for (int i = 0; i < n; ++i){
		cout << "m(T," << (i + 1) << ", x" << (i + 1) << ") ";
	}
}
int main(){

    //output_nargs_seq(40);
    //output_nargs(40);
    //cout << "\n\n";
   // output_apply_n(2);

    const int n = 40;
    output_header_and_beginning();
    cout << "\n\n";
    output_nargs(n);
    cout << "\n\n";
    for(int i = 0; i<n; ++i){
        output_apply_n(i+1);
        cout << "\n";
		output_apply_n_semicolon(i + 1);
		cout << "\n";
		output_apply_n_space(i + 1);
		cout << "\n";
	}
    output_apply("");
	output_apply("SEMICOLON_");
	output_apply("SPACE_");

}