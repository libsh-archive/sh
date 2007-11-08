#ifndef EXPRPARSER_HPP
#define EXPRPARSER_HPP

#include <string>
#include <iostream>
#include <sstream>

#include <sh/sh.hpp>

class  ExprParser {
  public:
    // Register a named variable (usually a global or external parameter) for use in expressions
    SH::Variable registerVar(std::string name, SH::Variable var);
    bool hasVar(std::string name); // returns whether a variable is defined
    SH::Variable getVar(std::string name); // returns null variable if not defined 

    // Register a named stream 
    SH::Stream registerStream(std::string name, SH::Stream var);
    bool hasStream(std::string name); // returns whether a stream is defined

    // returns null stream if not defined 
    // (note this also automatically casts a Variable into a one-element
    // stream if the variable is uniform or constant)
    // @todo this should go the other way too (in effect simulating the 
    // var/stream, memory/variant merge that may come in the future)
    SH::Stream getStream(std::string name); 

    void registerFunc(std::string name, SH::Program func);

    // fetches program
    SH::Program& operator[](std::string name);

    void dump(std::ostream& out);

    /* Parses a very simple expression that fits the following grammar 
     * S -> stmtlist
     * stmtlist -> 
     *  | stmt
     *  | stmt stmtlist 
     * stmt -> ;
     *  |      directive ;
     *  |      decl ;
     *  |      expr ; 
     *  |      ASM_OP expr expr ... ;   (ASM_OP is string of some Sh intrinsic)
     *  @todo | print expr;
     * directive -> # string
     * (supported directives:
     *    debug - toggles debug message state)
     * decl -> semantic_type var_name [ = expr ]
     * expr -> expr_0 [ = expr ]
     * expr_0 -> expr_1 [ | expr_0 ]
     * expr_1 -> expr_2 [(+ | -) expr_1 ]
     * expr_2 -> expr_3 [(* | / | %) expr_2 ]
     * expr_3 -> -* expr_4
     * expr_4 -> func_name arglist
     *  |        var_name swiz_opt 
     *  |        semantic_type constlist 
     * (if semantic_type is interval, constlist contains twice as many reals as
     * type size)
     *  |        (expr)
     *  |        positive_real
     * semantic_type -> @ sh_type_name 
     * swizopt ->
     *  | ( int [, ... ] )
     * arglist -> (args) 
     * args ->
     *  | expr 
     *  | expr, args 
     *  constlist -> (consts)
     *  consts ->
     *  | real
     *  | real, consts
     *
     * variable/function/semantic type names can contain any sequence of
     * alphanumeric or '_' characters.
     *
     *
     *  error recovery is to skip the symbol...(probably not so good)
     */
    void parse(std::string stmtlist);

    SH::Variable parse_expr(std::string expr); 

    static SH::Stream varToStream(SH::Variable& var);

  private:
    typedef std::map<std::string, SH::Variable> VarMap;
    VarMap m_vars; // user defined variables

    typedef std::map<std::string, SH::Stream> StreamMap;
    StreamMap m_streams; // user defined streams 

    typedef std::map<std::string, SH::Program> FuncMap;
    FuncMap m_funcs; // user defined functions

    // parsing variables
    int m_cur; // current position, starts at 0
    int m_save;
    int m_end; // last position
    int m_indent; // error/debug reporting indent level

    std::string m_stmtlist; 
    VarMap m_temps;
    void parse_stmtlist();
    void parse_stmt();
    bool parse_asmop(); // returns true if ASM op parsed
    void parse_directive();
    SH::Variable parse_decl();
    SH::Variable parse_expr(); // @todo allow user-defined operators and precedence 
    SH::Variable parse_expr0(); // @todo allow user-defined operators and precedence 
    SH::Variable parse_expr1();
    SH::Variable parse_expr2();
    SH::Variable parse_expr3();
    SH::Variable parse_expr4();
    //SH::Variable parse_func(); // @todo allow user defined functions using functors/func pointers
    SH::Record parse_arglist();
    std::vector<float> parse_constlist();
    SH::Swizzle parse_swiz(int srcSize);

    void eatWs();
    bool eatChar(char expected);
    char peek();
    std::string token(); // string of alphanumerics
    int number(); // positive integer 
    float real(); // real number ([-]digits[.digits]) 
    void save(); // saves and restores m_cur to/from m_save (useful for trial parses)
    void restore(); 
    SH::Variable makeVar(std::string shtype); 


    void debugEnter(std::string node);
    void debugExit();
};

struct ExprParserException: public SH::Exception {
  ExprParserException(std::string msg): SH::Exception(msg) {}
};

#endif
