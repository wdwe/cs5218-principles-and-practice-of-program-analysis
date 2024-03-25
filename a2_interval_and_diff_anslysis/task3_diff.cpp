#include <set>
#include <map>
#include <unordered_map>
#include <stack>


#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h"

#include "interval.h"
#include "utils.h"

typedef std::stack<std::pair<llvm::BasicBlock*, instr_interv_map>> dfs_stack;

std::string getSimpleNodeLabel(const llvm::BasicBlock *Node);
void processBlock(llvm::BasicBlock *BB, instr_interv_map &intervMap);


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

    /*
     * Since there is loop we set Abst::MIN to -200 and Abst::MAX to 200 for guaranteed termination
     * values less than Abst::min are treated as -inf, more than Abst::max as inf
     */
    Abst::MIN = -200;
    Abst::MAX = 200;

    std::unordered_map<std::string, instr_interv_map> BBInterv;

    // stack for DFS
    dfs_stack dfsStack;


    dfsStack.emplace(&F->getEntryBlock(), instr_interv_map());
    while (!dfsStack.empty()) {
        auto [BB, intervMap] = dfsStack.top();
        dfsStack.pop();
        processBlock(BB, intervMap);

        std::string blockName = getSimpleNodeLabel(BB);
        // save a copy
        auto oldIntervals = BBInterv[blockName];
        auto &allIntervals = BBInterv[blockName];
        for (auto &[k, v]: intervMap) {
            allIntervals[k] = allIntervals[k].unionWith(v);
        }

        // only add the suc if fixpoint is not reached
        if (oldIntervals != allIntervals) {
            for (auto suc: llvm::successors(BB))
                dfsStack.emplace(suc, allIntervals);
        }
    }
    printBBDifference(BBInterv);

    return 0;
}

void processBlock(llvm::BasicBlock *BB, instr_interv_map &intervMap) {
    for (auto &I: *BB) {
        if (llvm::isa<llvm::AllocaInst>(I)) {
            intervMap[&I] = Interval(Abst::negInf(), Abst::inf());
        }
        else if (llvm::isa<llvm::LoadInst>(I)) {
            auto *loadInst = llvm::dyn_cast<llvm::LoadInst>(&I);
            auto *argInstr = llvm::dyn_cast<llvm::Instruction>(loadInst->getOperand(0));
            intervMap[&I] = extractInterval(argInstr, intervMap);
        }
        else if (llvm::isa<llvm::StoreInst>(I)) {
            llvm::Value *val = I.getOperand(0), *var = I.getOperand(1);
            auto* varInstr = llvm::dyn_cast<llvm::Instruction>(var);
            intervMap[varInstr] = extractInterval(val, intervMap);
        }
        else if (llvm::isa<llvm::BinaryOperator>(I)) {
            // if it is a binary operator
            // the result value and two operands must be registers
            llvm::Value *left = I.getOperand(0), *right = I.getOperand(1);
            Interval leftInterv = extractInterval(left, intervMap);
            Interval rightInterv = extractInterval(right, intervMap);
            intervMap[&I] = binaryOperation(leftInterv, rightInterv, I);
        }
    }

    // remove register's interval
    removeReg(intervMap);

}
