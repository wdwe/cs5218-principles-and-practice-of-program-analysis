#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <stack>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"

//typedef std::set<llvm::Instruction *> instr_set;
typedef std::unordered_map<llvm::Instruction *, std::unordered_set<std::string>> vars_VB_map;
typedef std::unordered_set<std::string> VB_set;
typedef std::unordered_map<const llvm::Instruction *, llvm::Instruction *> instr_map;

struct BlockBusyExpr;

std::string getSimpleNodeLabel(const llvm::BasicBlock *Node);

std::vector<llvm::BasicBlock *> getLeafBlocks(llvm::BasicBlock *BB);

void updateVBExpr(llvm::BasicBlock *BB, vars_VB_map &varsVBMap, VB_set &VBSet);

instr_map getRegMap(llvm::BasicBlock *BB);

void genVB(llvm::Instruction &I, instr_map &regMap, vars_VB_map &varsVBMap, VB_set &VBSet);

std::pair<llvm::Instruction*, std::string> getInstrAndName(llvm::Value *I, instr_map &regMap);

void killVB(llvm::Instruction &I, instr_map &regMap, vars_VB_map &varsVBMap, VB_set &VBSet);

char opCodeToSymbol(const std::string &opName);

void printVB(const std::unordered_map<llvm::BasicBlock *, std::pair<BlockBusyExpr, BlockBusyExpr>> &BBVBMap);

void printStrSet(const std::unordered_set<std::string> &strSet);

struct BlockBusyExpr {
    llvm::BasicBlock *BB{};
    vars_VB_map varsVBMap;
    VB_set VBSet;
};

int main(int argc, char **argv) {

    // Extract Module M from IR (assuming only one Module exists)
    // Modules are the top level container of all other LLVM Intermediate Representation (IR) objects.
    // Read the IR file.
    llvm::SMDiagnostic Err;
    llvm::LLVMContext Context;  //A reference to the core LLVM "engine" that you should pass to the various methods that require a LLVMContext.
    std::unique_ptr<llvm::Module> Mod(parseIRFile(argv[1], Err, Context));
    if (!Mod) {
        fprintf(stderr, "error: failed to load LLVM IR file \"%s\"", argv[1]);
        return EXIT_FAILURE;
    }

    // Extract Function main from Module M
    llvm::Function *F = Mod->getFunction("main");

    // get exit blocks
    std::vector<llvm::BasicBlock *> exitBlocks = getLeafBlocks(&F->getEntryBlock());;

    // initialise dfs with exit blocks
    std::stack<BlockBusyExpr> dfsStack;
    for (auto exitBlock: exitBlocks)
        dfsStack.push({exitBlock, vars_VB_map(), VB_set()});

    // VB pair.first (entry) pair.second (exit)
    std::unordered_map<llvm::BasicBlock *, std::pair<BlockBusyExpr, BlockBusyExpr>> BBVBMap;

    while (!dfsStack.empty()) {
        auto [BB, varsVBMap, VBSet] = dfsStack.top();
        dfsStack.pop();

        // store the new exit VB
        BlockBusyExpr exitVB = {BB, varsVBMap, VBSet};

        updateVBExpr(BB, varsVBMap, VBSet);

        // if visit the block for the first time
        if (BBVBMap.count(BB) == 0)
            // copy over
            BBVBMap[BB] = {{BB, varsVBMap, VBSet}, exitVB};

        // meet operator, use intersection
        auto &[entryBB, entryVarsVBMap, entryVBSet] = BBVBMap[BB].first;
        // store a copy of old VB set for fix point analysis
        VB_set oldEntryVBSet = entryVBSet;

        // intersect
        for (auto it = entryVBSet.begin(); it != entryVBSet.end();) {
            if (VBSet.count(*it) == 0)
                it = entryVBSet.erase(it);
            else
                ++it;
        }

        for (auto &[var, entryExprs]: entryVarsVBMap) {
            std::unordered_set<std::string> exprs = varsVBMap[var];
            for (auto it = entryExprs.begin(); it != entryExprs.end();) {
                if (exprs.count(*it) == 0)
                    it = entryExprs.erase(it);
                else
                    ++it;
            }
        }


        // add the block's predecessor
        for (auto pred: llvm::predecessors(BB))
            if (oldEntryVBSet != entryVBSet || BBVBMap.count(pred) == 0)
                dfsStack.push({pred, entryVarsVBMap, entryVBSet});
    }

    printVB(BBVBMap);

    return 0;
}

void printVB(const std::unordered_map<llvm::BasicBlock *, std::pair<BlockBusyExpr, BlockBusyExpr>> &BBVBMap) {
    for (const auto &[BB, busyExprs]: BBVBMap) {
        llvm::outs() << getSimpleNodeLabel(BB) << "\n";
        llvm::outs() << "\tEntry: ";
        printStrSet(busyExprs.first.VBSet);
        llvm::outs() << "\n";
        llvm::outs() << "\tExit: ";
        printStrSet(busyExprs.second.VBSet);
        llvm::outs() << "\n";
    }
}

void printStrSet(const std::unordered_set<std::string> &strSet) {
    llvm::outs() << "{";
    for (auto it = strSet.cbegin(); it != strSet.cend(); ++it) {
        if (it != strSet.cbegin()) llvm::outs() << ", ";
        llvm::outs() << "\"" << *it << "\"";
    }
    llvm::outs() << "}";
}


void updateVBExpr(llvm::BasicBlock *BB, vars_VB_map &varsVBMap, VB_set &VBSet) {
    // forward pass to obtain register to var mapping
    instr_map regMap = getRegMap(BB);

    // backward pass to perform analysis
    for (auto it = BB->rbegin(); it != BB->rend(); ++it) {
        llvm::Instruction &I = *it;
        genVB(I, regMap, varsVBMap, VBSet);
        killVB(I, regMap, varsVBMap, VBSet);
    }
}


instr_map getRegMap(llvm::BasicBlock *BB) {
    instr_map regMap;
    for (const auto &I: *BB) {
        if (llvm::isa<llvm::LoadInst>(I)) {
            auto *instr = llvm::dyn_cast<llvm::LoadInst>(&I);
            llvm::Value *arg = instr->getOperand(0);
            auto *argInstr = llvm::dyn_cast<llvm::Instruction>(arg);
            regMap[&I] = argInstr;
        }
    }
    return regMap;
}


void genVB(llvm::Instruction &I, instr_map &regMap, vars_VB_map &varsVBMap, VB_set &VBSet) {
    if (llvm::isa<llvm::BinaryOperator>(I)) {
        char opSymbol = opCodeToSymbol(I.getOpcodeName());
        if (opSymbol != '\0') {
            llvm::Value *left = I.getOperand(0), *right = I.getOperand(1);
            auto [leftVar, leftName] = getInstrAndName(left, regMap);
            auto [rightVar, rightName] = getInstrAndName(right, regMap);
            std::string exprStr = leftName + " " + opSymbol + " " + rightName;
            VBSet.insert(exprStr);
            if (leftVar)
                varsVBMap[leftVar].insert(exprStr);
            if (rightVar)
                varsVBMap[rightVar].insert(exprStr);
        }
    }
}

std::pair<llvm::Instruction*, std::string> getInstrAndName(llvm::Value *I, instr_map &regMap) {
    std::string name;
    if (llvm::isa<llvm::Constant>(I)) {
        auto *valInstr = llvm::dyn_cast<llvm::ConstantInt>(I);
        name = valInstr->getValue().toString(10, true);
        return {nullptr, name};
    }
    auto *instr = llvm::dyn_cast<llvm::Instruction>(I);
    llvm::Instruction *varInstr = regMap[instr];
    name = varInstr->getName().str();
    return {varInstr, name };
}

void killVB(llvm::Instruction &I, instr_map &regMap, vars_VB_map &varsVBMap, VB_set &VBSet) {
    if (llvm::isa<llvm::StoreInst>(I)) {
        // if current instruction is storing value to variable
        llvm::Value *val = I.getOperand(0), *var = I.getOperand(1);
        auto *valReg = llvm::dyn_cast<llvm::Instruction>(val);
        auto *varInstr = llvm::dyn_cast<llvm::Instruction>(var);
        llvm::Instruction *valInstr = regMap[valReg];
        if (varInstr != valInstr) {
            // if it is not a = a
            for (const auto &expr: varsVBMap[varInstr]) {
                VBSet.erase(expr);
            }
            varsVBMap[varInstr].clear();
        }
    } else if (llvm::isa<llvm::AllocaInst>(I)) {
        for (const auto &expr: varsVBMap[&I]) {
            VBSet.erase(expr);
        }
        varsVBMap[&I].clear();
    }
}


char opCodeToSymbol(const std::string &opName) {
    if (opName.find("add") != std::string::npos) {
        return '+';
    }

    if (opName.find("sub") != std::string::npos) {
        return '-';
    }

    if (opName.find("mul") != std::string::npos) {
        return '*';
    }

    if (opName.find("div") != std::string::npos) {
        return '/';
    }

    if (opName.find("rem") != std::string::npos) {
        return '%';
    }

    return '\0';
}


std::vector<llvm::BasicBlock *> getLeafBlocks(llvm::BasicBlock *BB) {
    std::unordered_set<llvm::BasicBlock *> seen;
    std::vector<llvm::BasicBlock *> leaves;
    std::stack<llvm::BasicBlock *> dfsStack;
    dfsStack.push(BB);
    while (!dfsStack.empty()) {
        llvm::BasicBlock *block = dfsStack.top();
        dfsStack.pop();
        if (seen.count(block) > 0)
            continue;
        seen.insert(block);
        auto sucs = llvm::successors(block);
        if (sucs.begin() == sucs.end()) {
            // if this node is a leaf node
            leaves.push_back(block);
        } else {
            for (auto suc: sucs) {
                dfsStack.push(suc);
            }
        }
    }
    return leaves;
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