#ifndef PRUNER_HPP
#define PRUNER_HPP

#include <map>
#include <sh/sh.hpp>
#include <sh/shutil.hpp>
#include "Scanner.hpp"

/* Greedy prune of sections only (or statements if indicated) based on 
 *  abs(section error) / (# of statements)
 *
 * @todo range - wrap this in a class with the GUI component
 */
void defaultPrune(Scanner& scanner);

#endif
