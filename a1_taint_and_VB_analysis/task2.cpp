#include <set>
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

typedef std::set<llvm::Instruction*> instr_set;

std::string getSimpleNodeLabel(const llvm::BasicBlock *Node);
instr_set updateTaintedVars(llvm::BasicBlock* BB, instr_set taintedVars);
void printBBVars(const std::map<std::string, instr_set> &BBVars);


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

    // define an exit map recording tainted variables at exits of blocks
    std::map<std::string, instr_set> BBExitVars;

    // stack for DFS
    std::stack<std::pair<llvm::BasicBlock*, instr_set>> dfsStack;

    // initialise dfs with entry block
    dfsStack.emplace(&F->getEntryBlock(), instr_set());
    while (!dfsStack.empty()) {
        auto [BB, entryVars] = dfsStack.top();
        dfsStack.pop();

        instr_set taintedVars = updateTaintedVars(BB, entryVars);

        // since taint analysis is a `may` analysis
        // use union for different branches
        std::string blockName = getSimpleNodeLabel(BB);
        auto &exitVars = BBExitVars[blockName];
        std::copy(taintedVars.cbegin(), taintedVars.cend(), std::inserter(exitVars, exitVars.begin()));

        // add current block's successor to stack
        // IMPORTANT: Assuming there is no loop in the code to be analysed
        for (auto suc: llvm::successors(BB))
            dfsStack.emplace(suc, exitVars);
    }

    printBBVars(BBExitVars);

    return 0;
}

void printBBVars(const std::map<std::string, instr_set> &BBVars) {
    for (const auto &[blockName, varSet] : BBVars) {
        llvm::outs() << blockName << ": {";
        for (auto itVar = varSet.cbegin(); itVar != varSet.cend(); ++itVar) {
            if (itVar != varSet.cbegin())
                llvm::outs() << ", ";
            llvm::outs() << (*itVar)->getName();
        }
        llvm::outs() << "}\n";
    }
}

instr_set updateTaintedVars(llvm::BasicBlock* BB, instr_set taintedVars) {
    for (auto &I: *BB) {
        if (I.getName().str() == "source") {
        // if current instruction is `source` add it to tainted
            taintedVars.insert(&I);
        } else {
            if (llvm::isa<llvm::LoadInst>(I)) {
                // check if current instruction is loading from a tainted variable
                auto *instr = llvm::dyn_cast<llvm::LoadInst>(&I);
                llvm::Value *arg = instr->getOperand(0);
                auto *argInstr = llvm::dyn_cast<llvm::Instruction>(arg);
                if (taintedVars.count(argInstr) > 0) {
                    taintedVars.insert(&I);
                }
            } else if (llvm::isa<llvm::BinaryOperator>(I)) {
                // if it is a binary operator
                // the result value and two operands must be registers
                llvm::Value *left = I.getOperand(0), *right = I.getOperand(1);
                auto *leftInstr = llvm::dyn_cast<llvm::Instruction>(left);
                auto *rightInstr = llvm::dyn_cast<llvm::Instruction>(right);
                if (taintedVars.count(leftInstr) > 0 || taintedVars.count(rightInstr) > 0)
                    taintedVars.insert(&I);
            } else if (llvm::isa<llvm::StoreInst>(I)) {
                // if current instruction is storing value to variable
                llvm::Value *val = I.getOperand(0), *var = I.getOperand(1);
                auto* valInstr = llvm::dyn_cast<llvm::Instruction>(val);
                auto* varInstr = llvm::dyn_cast<llvm::Instruction>(var);
                if (taintedVars.count(valInstr) > 0) {
                    taintedVars.insert(varInstr); // gen
                } else if (varInstr->getName().str() != "source") {
                    // un-tainting, assuming `source` cannot be untainted
                    taintedVars.erase(varInstr); // kill
                }
            }
        }
    }

    // remove register from the variable set
    for (auto iter = taintedVars.begin(); iter != taintedVars.end(); ) {
        if (!llvm::isa<llvm::AllocaInst>(*iter))
            // if the stored expression is not a variable, remove it from the set
            taintedVars.erase(iter++);
        else
            ++iter;
    }

    return taintedVars;
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