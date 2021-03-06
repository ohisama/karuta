// -*- C++ -*-
#ifndef _synth_method_context_h_
#define _synth_method_context_h_

#include "synth/common.h"

namespace synth {

class StateWrapper {
public:
  StateWrapper();

  int index_;
  IState *state_;
  string callee_func_name_;
  vm::Insn *vm_insn_;
  // for sub obj task call or data flow call.
  string obj_name_;
  bool is_sub_obj_call_;
  bool is_data_flow_call_;
  bool is_ext_stub_call_;
  // vailid if is_ext_stub_call_ is true.
  // Indicate whether this calls flow or method.
  bool is_ext_flow_stub_call_;
  vm::Object *callee_vm_obj_;
};

class MethodContext {
public:
  MethodContext(MethodSynth *synth);
  ~MethodContext();

  // Pseudo insn to represent the entry of this method.
  // This insn holds args/rets information.
  IInsn *method_signature_insn_;
  vector<StateWrapper *> states_;

  StateWrapper *LastState();

private:
  MethodSynth *synth_;
};

}  // namespace synth

#endif  // _synth_method_context_h_
