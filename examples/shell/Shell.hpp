#ifndef SHELL_HPP
#define SHELL_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <sh/sh.hpp>
#include <sh/ShTransformer.hpp>
#include "ExprParser.hpp"

typedef std::vector<std::string> vecs;

/** 
 * Performs basic debug operations.
 * Possible Commands:
 *
 * p "name"
 *    Starts expr parser to specify a program (use line containing only "p" to end) 
 *
 * c
 *    Starts immediate mode computation
 *
 * dot "name"
 *    Dumps dot graph of "name"
 *
 * incl "func" "ifunc"
 *    Interval inclusion of func
 *
 * aincl "func" "afunc"
 *    Affine inclusion of func
 *
 * conv "func" "cfunc"
 *    IA, AA to float conversion 
 *
 * meta "name" "key" "value"
 *    Sets metadata on a program
 *
 * flag "name" "value"
 *    Sets a flag in the context (either true or false)
 *
 * print ("random_string" |  "var")
 *    Prints stuff (if token matches variable name, prints the variable, else
 *    echoes) 
 *
 * stream "name" "var1" "var2" ... 
 *    Makes a single-element stream using the given float variables as channels. 
 *
 * apply "out_stream" "func" "in_stream" 
 *
 * r "filename" arg1 arg2 ...
 *    Executes commands in filename
 *
 * b "backend"
 *    Executes shSetBackend("backend")
 *
 * dump
 *    Dumps current expression parser state
 *
 *
 * // ...
 * # ... 
 *    comments (note that to disambiguate, # must be followed by a space)
 *
 * q  
 *    Quits
 *
 * anything else (i.e. if line does not start with one of the above)
 *    parses line as a global expression
 *
 * $1 $2 ... $n are replaced with argn in a file
 *
 */
#define EXPECT(N) if(toks.size() < N) { cerr << "Expected " << N << " args " << endl; }

class Shell;
struct CommandProcessor {
  inline virtual ~CommandProcessor() {}
  virtual bool handle(Shell&, const vecs& toks) = 0;
  virtual std::ostream& help(std::ostream& out) = 0;// print out a help message
};
class  Shell {
  public:
    // consume input
    std::istream& start(std::istream& in, const vecs& args, CommandProcessor* comproc, bool prompt = true);

    ExprParser ep;
};

#endif
