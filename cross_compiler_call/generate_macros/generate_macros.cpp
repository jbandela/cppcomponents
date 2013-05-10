#include <iostream>

/* This counts the number of args */
#define CROSS_COMPILER_INTERFACE_NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,N,...) N
#ifdef _MSC_VER
#define CROSS_COMPILER_INTERFACE_NARGS(...) CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_NARGS_SEQ(__VA_ARGS__,40, 39, 38, 37, 36, 35, 34, 33, 32, 31,30 , 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,19 ,18 ,17 , 16 , 15 , 14 , 13, 12, 11, 10 , 9, 8, 7, 6, 5, 4, 3, 2, 1),)
#else
#define CROSS_COMPILER_INTERFACE_NARGS(...) CROSS_COMPILER_INTERFACE_NARGS_SEQ(__VA_ARGS__,40, 39, 38, 37, 36, 35, 34, 33, 32, 31,30 , 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,19 ,18 ,17 , 16 , 15 , 14 , 13, 12, 11, 10 , 9, 8, 7, 6, 5, 4, 3, 2, 1)

#endif

using namespace std;

void output_header_and_beginning(){
    cout << 
"// Adapted from http://stackoverflow.com/questions/11920577/casting-all-parameters-passed-in-macro-using-va-args\n"
"\n"
"// With adaptations for MSVC from BOOST_PP\n"
"\n"
"\n"
"/* This will let macros expand before concating them */\n"
"#define CROSS_COMPILER_INTERFACE_PRIMITIVE_CAT(x, y) x ## y\n"
"#define CROSS_COMPILER_INTERFACE_CAT(x, y) CROSS_COMPILER_INTERFACE_PRIMITIVE_CAT(x, y)\n"
"\n"
"#define CROSS_COMPILER_INTERFACE_STR(x) #x\n"
;

}

void output_nargs_seq(int n){
    cout << "#define CROSS_COMPILER_INTERFACE_NARGS_SEQ (";
    for(int i = 0; i< n; ++i){
        cout << "_" << (i+1);
        if(i < (n-1)) cout << ",";
    }
    cout << ",N...) N";


}

void output_nargs_part_seq(int n){
    cout << "CROSS_COMPILER_INTERFACE_NARGS_SEQ(__VA_ARGS__,";
    for(int i = n; i > 0; --i){
        cout << i;
        if( i > 1) cout << ", ";
    }
    cout << ")";

}

void output_nargs(int n){

    output_nargs_seq(n);
    cout << "\n";
    cout << "#ifdef _MSC_VER\n"
        << "#define CROSS_COMPILER_INTERFACE_NARGS(...) CROSS_COMPILER_INTERFACE_CAT(";
    output_nargs_part_seq(n);
    cout << ",)\n";
    cout << "#else\n"
        << "#define CROSS_COMPILER_INTERFACE_NARGS(...) ";
    output_nargs_part_seq(n);
    cout << "\n#endif\n";


}

void output_apply(){
    cout << 
        "/* This will call a macro on each argument passed in */\n"
        "#ifdef _MSC_VER\n"
        "#define CROSS_COMPILER_INTERFACE_APPLY(macro, ...) CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_APPLY_, CROSS_COMPILER_INTERFACE_NARGS(__VA_ARGS__))CROSS_COMPILER_INTERFACE_CAT(( macro, __VA_ARGS__),)\n"
        "#else\n"
        "#define CROSS_COMPILER_INTERFACE_APPLY(macro, ...) CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_APPLY_, CROSS_COMPILER_INTERFACE_NARGS(__VA_ARGS__))( macro, __VA_ARGS__)\n"
        "#endif\n";

}

void output_apply_n(int n){

    cout << "#define CROSS_COMPILER_INTERFACE_APPLY_" << n << "(m,";
    for(int i = 0; i < n; ++i){
        cout << "x" << (i+1);
        if(i < (n-1)) cout << ", ";
    }
    cout << ") ";
    for(int i = 0; i < n; ++i){
        cout << "m(" << (i+1) << ", x" << (i+1) << ")";
        if( i < (n-1) ) cout << ", ";
    }
}

int main(){

    //output_nargs_seq(40);
    //output_nargs(40);
    //cout << "\n\n";
    output_apply_n(2);

    const int n = 40;
    output_header_and_beginning();
    cout << "\n\n";
    output_nargs(n);
    cout << "\n\n";
    for(int i = 0; i<n; ++i){
        output_apply_n(i+1);
        cout << "\n";
    }
    output_apply();

}