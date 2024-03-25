//
// Created by wayne on 3/23/24.
//

#ifndef A2_UTILS_H
#define A2_UTILS_H
#include <stack>
#include <utility>
#include <unordered_map>
#include "abst.h"
#include "interval.h"
#include "llvm/IR/Instructions.h"

typedef std::unordered_map<llvm::Instruction*, Interval> instr_interv_map;


Interval extractInterval(llvm::Value *V, instr_interv_map& map);
void removeReg(instr_interv_map &intervMap);
Interval binaryOperation(const Interval& left, const Interval& right, const llvm::Instruction& I);
void printBBDifference(std::unordered_map<std::string, instr_interv_map> &map);
void printBBInterval(std::unordered_map<std::string, instr_interv_map> &map);
void printInstruction(const llvm::Instruction &I);
void printInterval(const instr_interv_map &map);

#endif //A2_UTILS_H
