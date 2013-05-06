// Adapted from http://stackoverflow.com/questions/11920577/casting-all-parameters-passed-in-macro-using-va-args
// With adaptations for MSVC from BOOST_PP


/* This will let macros expand before concating them */
#define CROSS_COMPILER_INTERFACE_PRIMITIVE_CAT(x, y) x ## y
#define CROSS_COMPILER_INTERFACE_CAT(x, y) CROSS_COMPILER_INTERFACE_PRIMITIVE_CAT(x, y)



/* This counts the number of args */
#define CROSS_COMPILER_INTERFACE_NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,N,...) N
#ifdef _MSC_VER
#define CROSS_COMPILER_INTERFACE_NARGS(...) CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_NARGS_SEQ(__VA_ARGS__,40, 39, 38, 37, 36, 35, 34, 33, 32, 31,30 , 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,19 ,18 ,17 , 16 , 15 , 14 , 13, 12, 11, 10 , 9, 8, 7, 6, 5, 4, 3, 2, 1),)
#else
#define CROSS_COMPILER_INTERFACE_NARGS(...) CROSS_COMPILER_INTERFACE_NARGS_SEQ(__VA_ARGS__,40, 39, 38, 37, 36, 35, 34, 33, 32, 31,30 , 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,19 ,18 ,17 , 16 , 15 , 14 , 13, 12, 11, 10 , 9, 8, 7, 6, 5, 4, 3, 2, 1)

#endif


/* This will call a macro on each argument passed in */
#ifdef _MSC_VER
#define CROSS_COMPILER_INTERFACE_APPLY(macro, ...) CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_APPLY_, CROSS_COMPILER_INTERFACE_NARGS(__VA_ARGS__))CROSS_COMPILER_INTERFACE_CAT(( macro, __VA_ARGS__),)
#else
#define CROSS_COMPILER_INTERFACE_APPLY(macro, ...) CROSS_COMPILER_INTERFACE_CAT(CROSS_COMPILER_INTERFACE_APPLY_, CROSS_COMPILER_INTERFACE_NARGS(__VA_ARGS__))( macro, __VA_ARGS__)
#endif
#define CROSS_COMPILER_INTERFACE_APPLY_1(m, x1) m(x1)
#define CROSS_COMPILER_INTERFACE_APPLY_2(m, x1, x2) m(x1), m(x2)
#define CROSS_COMPILER_INTERFACE_APPLY_3(m, x1, x2, x3) m(x1), m(x2), m(x3)
#define CROSS_COMPILER_INTERFACE_APPLY_4(m, x1, x2, x3, x4) m(x1), m(x2), m(x3), m(x4)
#define CROSS_COMPILER_INTERFACE_APPLY_5(m, x1, x2, x3, x4, x5) m(x1), m(x2), m(x3), m(x4), m(x5)
#define CROSS_COMPILER_INTERFACE_APPLY_6(m, x1, x2, x3, x4, x5, x6) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6)
#define CROSS_COMPILER_INTERFACE_APPLY_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7)
#define CROSS_COMPILER_INTERFACE_APPLY_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8)
#define CROSS_COMPILER_INTERFACE_APPLY_9(m, x1, x2, x3, x4, x5, x6, x7, x8, x9) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9)
#define CROSS_COMPILER_INTERFACE_APPLY_10(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10)
#define CROSS_COMPILER_INTERFACE_APPLY_11(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11)
#define CROSS_COMPILER_INTERFACE_APPLY_12(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12)
#define CROSS_COMPILER_INTERFACE_APPLY_13(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13)
#define CROSS_COMPILER_INTERFACE_APPLY_14(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14)
#define CROSS_COMPILER_INTERFACE_APPLY_15(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15)
#define CROSS_COMPILER_INTERFACE_APPLY_16(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15), m(x16)
#define CROSS_COMPILER_INTERFACE_APPLY_17(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15), m(x16), m(x17)
#define CROSS_COMPILER_INTERFACE_APPLY_18(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15), m(x16), m(x17), m(x18)
#define CROSS_COMPILER_INTERFACE_APPLY_19(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15), m(x16), m(x17), m(x18), m(x19)
#define CROSS_COMPILER_INTERFACE_APPLY_20(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15), m(x16), m(x17), m(x18), m(x19), m(x20)
#define CROSS_COMPILER_INTERFACE_APPLY_21(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15), m(x16), m(x17), m(x18), m(x19), m(x20), m(x21)
#define CROSS_COMPILER_INTERFACE_APPLY_22(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15), m(x16), m(x17), m(x18), m(x19), m(x20), m(x21), m(x22)
#define CROSS_COMPILER_INTERFACE_APPLY_23(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15), m(x16), m(x17), m(x18), m(x19), m(x20), m(x21), m(x22), m(x23)
#define CROSS_COMPILER_INTERFACE_APPLY_24(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15), m(x16), m(x17), m(x18), m(x19), m(x20), m(x21), m(x22), m(x23), m(x24)
#define CROSS_COMPILER_INTERFACE_APPLY_25(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16, x17, x18, x19, x20, x21, x22, x23, x24, x25) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15), m(x16), m(x17), m(x18), m(x19), m(x20), m(x21), m(x22), m(x23), m(x24), m(x25)


#define CROSS_COMPILER_INTERFACE_STR(x) #x