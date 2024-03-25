#include <set>
#include <map>
#include <unordered_map>
#include <stack>
#include <vector>

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


struct CmpBrInterval {
    bool feasible = true; // whether this branch is feasible
    instr_interv_map interv;
};

struct CmpIntervals {
    CmpBrInterval T;
    CmpBrInterval F;
};


typedef std::stack<std::pair<llvm::BasicBlock*, instr_interv_map>> dfs_stack;

std::string getSimpleNodeLabel(const llvm::BasicBlock *Node);
std::unordered_map<llvm::Instruction*, CmpIntervals> processBlock(llvm::BasicBlock *BB, instr_interv_map &intervMap);
std::pair<Interval, Interval> cmpTransfer(const Interval& left, const Interval& right, const std::string& opName);
void updateValueInterval(instr_interv_map &map, llvm::Value *val, const Interval& interv);
CmpBrInterval getCmpInterval(instr_interv_map map, llvm::Value *left, llvm::Value *right, const Interval &leftInterv, const Interval &rightInterv);


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

    // initialise dfs with entry block
    dfsStack.emplace(&F->getEntryBlock(), instr_interv_map());
    while (!dfsStack.empty()) {
        auto [BB, blockInterv] = dfsStack.top();
        dfsStack.pop();
        std::string blockName = getSimpleNodeLabel(BB);

        auto cmpIntervMap = processBlock(BB, blockInterv);


        // check for fix point
        auto oldIntervals = BBInterv[blockName];
        auto &allIntervals = BBInterv[blockName];
        for (auto &[k, v]: blockInterv) {
            allIntervals[k] = allIntervals[k].unionWith(v);
        }


        // if fixpoint reached
        if (oldIntervals == allIntervals)
            continue;

        auto terminator = BB->getTerminator();


        if (llvm::isa<llvm::BranchInst>(terminator) && llvm::dyn_cast<llvm::BranchInst>(terminator)->isConditional()) {
            auto br = llvm::dyn_cast<llvm::BranchInst>(terminator);
            if (br->isConditional()) {
                auto *cmpVal = br->getCondition();
                auto *cmpInstr = llvm::dyn_cast<llvm::Instruction>(cmpVal);
                CmpIntervals cmpInterv = cmpIntervMap[cmpInstr];
                // true branch
                CmpBrInterval TBrInterv = cmpInterv.T;
                auto *TBlock = br->getSuccessor(0);
                if (TBrInterv.feasible) {
                    dfsStack.emplace(TBlock, TBrInterv.interv);
                }

                // false branch
                CmpBrInterval FBrInterv = cmpInterv.F;
                auto *FBlock = br->getSuccessor(1);
                if (FBrInterv.feasible) {
                    dfsStack.emplace(FBlock, FBrInterv.interv);
                }
            }
        } // unconditional jump
        else {
            for (auto suc: llvm::successors(BB))
                dfsStack.emplace(suc, allIntervals);
        }
    }


    printBBInterval(BBInterv);

    return 0;
}


std::unordered_map<llvm::Instruction*, CmpIntervals> processBlock(llvm::BasicBlock *BB, instr_interv_map &intervMap) {
    std::unordered_map<llvm::Instruction*, CmpIntervals> cmpIntervMap;
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
        else if (llvm::isa<llvm::CmpInst>(I)) {
            llvm::Value *left = I.getOperand(0), *right = I.getOperand(1);
            Interval leftInterv = extractInterval(left, intervMap);
            Interval rightInterv = extractInterval(right, intervMap);
            auto *cmpInst = llvm::dyn_cast<llvm::CmpInst>(&I);
            std::string pred = llvm::CmpInst::getPredicateName(cmpInst->getPredicate());
            std::string invPred = llvm::CmpInst::getPredicateName(cmpInst->getInversePredicate());

            CmpIntervals &cmp = cmpIntervMap[&I];
            // for True
            auto [t_left, t_right] = cmpTransfer(leftInterv, rightInterv, pred);
            cmp.T = getCmpInterval(intervMap, left, right, t_left, t_right);
            // for false
            auto [f_left, f_right] = cmpTransfer(leftInterv, rightInterv, invPred);
            cmp.F = getCmpInterval(intervMap, left, right, f_left, f_right);
        }
    }

    removeReg(intervMap);

    return cmpIntervMap;
}




CmpBrInterval getCmpInterval(instr_interv_map map, llvm::Value *left, llvm::Value *right, const Interval &leftInterv, const Interval &rightInterv) {
    CmpBrInterval intervs;
    if (leftInterv.isEmpty()) {
        intervs.feasible = false;
        return intervs;
    }
    intervs.feasible = true;
    updateValueInterval(map, left, leftInterv);
    updateValueInterval(map, right, rightInterv);
    removeReg(map);
    intervs.interv = map;
    return intervs;
}

void updateValueInterval(instr_interv_map &map, llvm::Value *val, const Interval& interv) {
    if (llvm::isa<llvm::Constant>(val))
        return;
    // else it is a load instruction since we only consider (x > y) not (x + 1 > y)
    auto *I = llvm::dyn_cast<llvm::Instruction>(val);
    if (llvm::isa<llvm::LoadInst>(I)) {
        auto *loadInst = llvm::dyn_cast<llvm::LoadInst>(I);
        auto *argInstr = llvm::dyn_cast<llvm::Instruction>(loadInst->getOperand(0));
        map[argInstr] = interv;
    } else {
        llvm::outs() << "As mentioned in discussion only need to support x > y not x + 1 > y\n";
    }
}


std::pair<Interval, Interval> cmpTransfer(const Interval& left, const Interval& right, const std::string& opName) {
    if (opName.find("eq") != std::string::npos)
        return Interval::eqTransfer(left, right);

    if (opName.find("ne") != std::string::npos)
        return Interval::neTransfer(left, right);

    if (opName.find("lt") != std::string::npos)
        return Interval::ltTransfer(left, right);

    if (opName.find("le") != std::string::npos)
        return Interval::leTransfer(left, right);

    if (opName.find("gt") != std::string::npos)
        return Interval::gtTransfer(left, right);

    if (opName.find("ge") != std::string::npos)
        return Interval::geTransfer(left, right);

    llvm::outs() << "error in cmpTransfer, opName: " << opName << "\n";
    return {Interval::empty(), Interval::empty()};
}