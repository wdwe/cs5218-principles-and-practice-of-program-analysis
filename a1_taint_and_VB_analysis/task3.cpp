#include <set>
#include <map>
#include <vector>
#include <stack>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"

typedef std::map<llvm::Instruction*, std::vector<std::string>> instr_paths_map;

void copyTaintedPaths(instr_paths_map &map, llvm::Instruction *src, llvm::Instruction *dest);
std::string getSimpleNodeLabel(const llvm::BasicBlock *Node);
void updateTaintedVarsPaths(llvm::BasicBlock* BB, instr_paths_map &taintedVarsPaths);
void printVarsPaths(const instr_paths_map &varsPaths);
void printBBVarsPaths(const std::map<std::string, instr_paths_map> &BBVars);


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
    std::map<std::string, instr_paths_map> BBExitVarsPaths;

    // stack for DFS
    std::stack<std::pair<llvm::BasicBlock*, instr_paths_map>> dfsStack;

    // initialise dfs with entry block
    dfsStack.emplace(&F->getEntryBlock(), instr_paths_map());
    while (!dfsStack.empty()) {
        auto [BB, taintedVarsPaths] = dfsStack.top();
        dfsStack.pop();
        updateTaintedVarsPaths(BB, taintedVarsPaths);

        // since taint analysis is a `may` analysis
        // use union for different branches
        std::string blockName = getSimpleNodeLabel(BB);
        auto &exitVarsPaths = BBExitVarsPaths[blockName];
        for (auto &[I, paths]: taintedVarsPaths) {
            std::vector<std::string> &exitPaths = exitVarsPaths[I];
            std::set<std::string> dedup;
            std::copy(exitPaths.cbegin(), exitPaths.cend(), std::inserter(dedup, dedup.begin()));
            std::copy(paths.cbegin(), paths.cend(), std::inserter(dedup, dedup.begin()));
            exitPaths.clear();
            std::move(dedup.begin(), dedup.end(), std::back_inserter(exitPaths));
        }


        // add current block's successor to stack
        // IMPORTANT: assume there is no loops in the code to be analysed
        for (auto suc: llvm::successors(BB))
            dfsStack.emplace(suc, exitVarsPaths);
    }

    printBBVarsPaths(BBExitVarsPaths);

    return 0;
}

void printVarsPaths(const instr_paths_map &varsPaths) {
    for (const auto& [I, paths]: varsPaths) {
        llvm::outs() << "\t" << I->getName() << ": {";
        for (auto itP = paths.cbegin(); itP != paths.cend(); ++itP) {
            if (itP != paths.cbegin())
                llvm::outs() << ", ";
            llvm::outs() << *itP;
        }
        llvm::outs() << "}\n";
    }
}

void printBBVarsPaths(const std::map<std::string, instr_paths_map> &BBVarsPaths) {
    for (const auto &[blockName, varsPaths]: BBVarsPaths) {
        llvm::outs() << blockName << "\n";
        printVarsPaths(varsPaths);
    }

}

void updateTaintedVarsPaths(llvm::BasicBlock* BB, instr_paths_map &taintedVarsPaths) {
    // add current Block to all the paths
    std::string blockName = getSimpleNodeLabel(BB);
    for (auto &[I, paths]: taintedVarsPaths) {
        for (auto &p: paths)
            p.append("->" + blockName);
    }

    // iterate over instruction to update tainted status
    for (auto &I: *BB) {
        if (I.getName().str() == "source") {
            // if current instruction is `source` add it to tainted
            std::vector<std::string> &paths = taintedVarsPaths[&I];
            if (paths.empty())
                // assume there is only one source from the beginning of the function
                // so start the path only if the paths is empty
                paths.emplace_back(blockName);
        } else {
            if (llvm::isa<llvm::LoadInst>(I)) {
                // check if current instruction is loading from a tainted variable
                auto *instr = llvm::dyn_cast<llvm::LoadInst>(&I);
                llvm::Value *arg = instr->getOperand(0);
                auto *argInstr = llvm::dyn_cast<llvm::Instruction>(arg);
                // copy the instruction's paths to current instruction
                if (taintedVarsPaths.count(argInstr) > 0)
                    copyTaintedPaths(taintedVarsPaths, argInstr, &I);
            } else if (llvm::isa<llvm::BinaryOperator>(I)) {
                // if it is a binary operator
                // the result value and two operands must be registers
                llvm::Value *left = I.getOperand(0), *right = I.getOperand(1);
                auto *leftInstr = llvm::dyn_cast<llvm::Instruction>(left);
                auto *rightInstr = llvm::dyn_cast<llvm::Instruction>(right);
                if (taintedVarsPaths.count(leftInstr) > 0)
                    copyTaintedPaths(taintedVarsPaths, leftInstr, &I);
                if (taintedVarsPaths.count(rightInstr) > 0)
                    copyTaintedPaths(taintedVarsPaths, rightInstr, &I);
            } else if (llvm::isa<llvm::StoreInst>(I)) {
                // if current instruction is storing value to variable
                llvm::Value *val = I.getOperand(0), *var = I.getOperand(1);
                auto* valInstr = llvm::dyn_cast<llvm::Instruction>(val);
                auto* varInstr = llvm::dyn_cast<llvm::Instruction>(var);
                if (taintedVarsPaths.count(valInstr) > 0) {
                    copyTaintedPaths(taintedVarsPaths, valInstr, varInstr);; // gen
                } else if (varInstr->getName().str() != "source") {
                    // un-tainting, assuming `source` cannot be untainted
                    taintedVarsPaths.erase(varInstr); // kill
                }
            }
        }
    }

    // remove register from the variable set
    for (auto iter = taintedVarsPaths.begin(); iter != taintedVarsPaths.end(); ) {
        if (!llvm::isa<llvm::AllocaInst>(iter->first))
            // if the stored expression is not a variable, remove it from the set
            taintedVarsPaths.erase(iter++);
        else
            ++iter;
    }

}

void copyTaintedPaths(instr_paths_map &map, llvm::Instruction *src, llvm::Instruction *dest) {
    auto &additionalPaths = map[src];
    std::copy(additionalPaths.cbegin(), additionalPaths.cend(), std::back_inserter(map[dest]));
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