#include "synth/method_expander.h"

#include "iroha/iroha.h"
#include "iroha/i_design.h"
#include "synth/method_context.h"
#include "synth/resource_set.h"
#include "synth/object_synth.h"
#include "synth/thread_synth.h"
#include "synth/tool.h"

namespace synth {

MethodExpander::MethodExpander(MethodContext *root, ThreadSynth *thread_synth,
			       vector<SubObjCall> *sub_obj_calls)
  : root_method_(root), thread_(thread_synth),
    tab_(thread_synth->GetITable()),
    sub_obj_calls_(sub_obj_calls)
{
}

bool MethodExpander::Expand() {
  for (IRegister *reg : tab_->registers_) {
    thread_->AddName(reg->GetName());
  }
  CalleeInfo ci = ExpandMethod(root_method_);
  tab_->SetInitialState(ci.initial);
  return true;
}

CalleeInfo MethodExpander::ExpandMethod(MethodContext *method) {
  map<IState *, IState *> st_copy_map;
  map<IRegister *, IRegister *> reg_copy_map;
  // Copy registers.
  for (auto &sw : method->states_) {
    IState *ost = sw->state_;
    IState *nst = new IState(ost->GetTable());
    st_copy_map[ost] = nst;
    for (IInsn *insn : ost->insns_) {
      BuildInsnRegCopy(insn, reg_copy_map);
    }
    BuildInsnRegCopy(method->method_insn_, reg_copy_map);
  }
  // Copy states.
  for (StateWrapper *sw : method->states_) {
    IState *st = sw->state_;
    IState *nst = st_copy_map[st];
    tab_->states_.push_back(nst);
    CopyState(st, st_copy_map, reg_copy_map, nst);
  }
  CollectSubObjCalls(method, st_copy_map);
  ExpandCalleeStates(method, st_copy_map, reg_copy_map);
  // Add registers.
  for (auto it : reg_copy_map) {
    IRegister *nreg = it.second;
    if (!nreg->IsConst()) {
      tab_->registers_.push_back(nreg);
    }
  }
  CalleeInfo p;
  p.initial = st_copy_map[method->states_[0]->state_];
  p.final = st_copy_map[(*(method->states_.rbegin()))->state_];
  for (IRegister *reg : method->method_insn_->inputs_) {
    p.args.push_back(reg_copy_map[reg]);
  }
  for (IRegister *reg : method->method_insn_->outputs_) {
    p.rets.push_back(reg_copy_map[reg]);
  }
  return p;
}

void MethodExpander::CollectSubObjCalls(MethodContext *method,
					map<IState *, IState *> &st_map) {
  // sub obj call resource.
  IResource *pseudo = thread_->GetResourceSet()->PseudoCallResource();
  for (StateWrapper *sw : method->states_) {
    if (!sw->is_sub_obj_call_) {
      continue;
    }
    SubObjCall call;
    IState *st = st_map[sw->state_];
    call.call_insn = DesignUtil::FindInsnByResource(st, pseudo);
    call.call_state = st;
    call.callee_obj = sw->callee_vm_obj_;
    call.callee_func = sw->callee_func_name_;
    sub_obj_calls_->push_back(call);
  }
}

void MethodExpander::ExpandCalleeStates(MethodContext *method,
					map<IState *, IState *> &st_map,
					map<IRegister *, IRegister *> &reg_map) {
  // call resource.
  IResource *pseudo = thread_->GetResourceSet()->PseudoCallResource();
  for (StateWrapper *sw : method->states_) {
    if (sw->callee_func_name_.empty()) {
      continue;
    }
    if (sw->is_sub_obj_call_) {
      continue;
    }
    MethodContext *callee =
      thread_->GetMethodContext(sw->callee_vm_obj_,
				sw->callee_func_name_);
    CalleeInfo ci = ExpandMethod(callee);
    IState *rs = Tool::GetNextState(sw->state_);
    Tool::SetNextState(st_map[sw->state_], ci.initial);
    Tool::SetNextState(ci.final, st_map[rs]);
    IInsn *call_insn = DesignUtil::FindInsnByResource(sw->state_, pseudo);
    CHECK(call_insn->inputs_.size() == ci.args.size());
    IResource *assign = thread_->GetResourceSet()->AssignResource();
    // Set up call arguments.
    for (size_t i = 0; i < call_insn->inputs_.size(); ++i) {
      IInsn *assign_insn = new IInsn(assign);
      assign_insn->inputs_.push_back(reg_map[call_insn->inputs_[i]]);
      assign_insn->outputs_.push_back(ci.args[i]);
      st_map[sw->state_]->insns_.push_back(assign_insn);
    }
    // Get return values.
    for (size_t i = 0; i < call_insn->outputs_.size(); ++i) {
      IInsn *assign_insn = new IInsn(assign);
      assign_insn->inputs_.push_back(ci.rets[i]);
      assign_insn->outputs_.push_back(reg_map[call_insn->outputs_[i]]);
      st_map[rs]->insns_.push_back(assign_insn);
    }
  }
}

void MethodExpander::CopyState(IState *ost, map<IState *, IState *> &st_map,
			       map<IRegister *, IRegister *> &reg_map,
			       IState *nst) {
  for (IInsn *insn : ost->insns_) {
    nst->insns_.push_back(CopyInsn(insn, st_map, reg_map));
  }
}

IInsn *MethodExpander::CopyInsn(IInsn *oinsn,
				map<IState *, IState *> &st_map,
				map<IRegister *, IRegister *> &reg_map) {
  IInsn *insn = new IInsn(oinsn->GetResource());
  insn->SetOperand(oinsn->GetOperand());
  for (IRegister *reg : oinsn->inputs_) {
    insn->inputs_.push_back(reg_map[reg]);
  }
  for (IRegister *reg : oinsn->outputs_) {
    insn->outputs_.push_back(reg_map[reg]);
  }
  for (IState *st : oinsn->target_states_) {
    insn->target_states_.push_back(st_map[st]);
  }
  return insn;
}

void MethodExpander::BuildInsnRegCopy(IInsn *insn,
				      map<IRegister *, IRegister *> &reg_map) {
  for (IRegister *reg : insn->inputs_) {
    BuildRegCopy(reg, reg_map);
  }
  for (IRegister *reg : insn->outputs_) {
    BuildRegCopy(reg, reg_map);
  }
}

void MethodExpander::BuildRegCopy(IRegister *reg,
				  map<IRegister *, IRegister *> &reg_map) {
  if (reg->IsConst()) {
    reg_map[reg] = reg;
    return;
  }
  if (reg_map.find(reg) != reg_map.end()) {
    return;
  }
  IRegister *nreg = thread_->AllocRegister(reg->GetName());
  if (reg->HasInitialValue()) {
    iroha::Numeric v = reg->GetInitialValue();
    nreg->SetInitialValue(v);
  }
  nreg->SetConst(reg->IsConst());
  nreg->SetStateLocal(reg->IsStateLocal());
  nreg->value_type_ = reg->value_type_;
  reg_map[reg] = nreg;
}

}  // namespace synth
