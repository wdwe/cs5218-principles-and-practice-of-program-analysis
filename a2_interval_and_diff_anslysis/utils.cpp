//
// Created by wayne on 3/23/24.
//
#include "utils.h"
#include <iterator>

Interval extractInterval(llvm::Value *V, instr_interv_map& map) {
    if (llvm::isa<llvm::Constant>(V)) {
        auto *constInstr = llvm::dyn_cast<llvm::ConstantInt>(V);
        long long val = constInstr->getSExtValue();
        return Interval(val);
    }
    auto *I = llvm::dyn_cast<llvm::Instruction>(V);
    return map[I];
}

void removeReg(instr_interv_map &intervMap) {
    // remove register's interval
    for (auto it = intervMap.begin(); it != intervMap.end();) {
        if (!llvm::isa<llvm::AllocaInst>(it->first))
            intervMap.erase(it++);
        else
            ++it;
    }
}

Interval binaryOperation(const Interval& left, const Interval& right, const llvm::Instruction &I) {
    std::string opName = I.getOpcodeName();
    if (opName.find("add") != std::string::npos) {
        return left + right;
    }

    if (opName.find("sub") != std::string::npos) {
        return left - right;
    }

    if (opName.find("mul") != std::string::npos) {
        return left * right;
    }

    if (opName.find("div") != std::string::npos) {
        return left / right;
    }

    if (opName.find("rem") != std::string::npos) {
        return left % right;
    }

    printInstruction(I);
    llvm::outs() << "undefined binary operation: " << opName << "\n";

    return {Abst::undef(), Abst::undef()};
}


void printBBDifference(std::unordered_map<std::string, instr_interv_map> &map) {
    // print interval first
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        if (it != map.cbegin())
            llvm::outs() << "\n";
        auto blockName = it->first;
        llvm::outs() << blockName << ":\n";
        auto &varInterv = it->second;
        for (auto it2 = varInterv.cbegin(); it2 != varInterv.cend(); ++it2) {
            std::string var1 = it2->first->getName();
            if (var1 == "retval")
                continue;
            for (auto it3 = std::next(it2); it3 != varInterv.cend(); ++it3) {
                std::string var2 = it3->first->getName();
                if (var2 == "retval")
                    continue;
                Abst diff = it2->second.sep(it3->second);
                llvm::outs() << "sep(" << var1 << ", " << var2 << ") = " << diff << "\n";
            }
        }
    }
}

void printBBInterval(std::unordered_map<std::string, instr_interv_map> &map) {
    // print interval first
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        if (it != map.cbegin())
            llvm::outs() << "\n";
        auto blockName = it->first;
        llvm::outs() << blockName << ":\n";
        printInterval(it->second);
    }
}

void printInterval(const instr_interv_map &map) {
    for (const auto &[k, v]: map) {
        std::string varName = k->getName();
        if (varName != "retval")
            llvm::outs() << varName << ": " << v << "\n";
    }
}

// get BasicBlock Label
std::string getSimpleNodeLabel(const llvm::BasicBlock *Node) {
    if (!Node->getName().empty())
        return Node->getName().str();
    std::string Str;
    llvm::raw_string_ostream OS(Str);
    Node->printAsOperand(OS, false);
    return OS.str();
}

void printInstruction(const llvm::Instruction &I) {
    std::string str;
    llvm::raw_string_ostream ss(str);
    ss << I;
    llvm::outs() << ss.str() << "\n";
}