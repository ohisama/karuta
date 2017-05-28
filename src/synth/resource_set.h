// -*- C++ -*-
//
// Manages per table resource objects.

#ifndef _synth_resource_set_h_
#define _synth_resource_set_h_

#include "synth/common.h"

// for IValueType
#include "iroha/i_design.h"
#include "vm/opcode.h"

#include <map>

using std::map;

namespace synth {

class ResourceSet {
public:
  ResourceSet(ITable *tab);
  ~ResourceSet();

  IResource *AssertResource();
  IResource *AssignResource();
  IResource *BranchResource();
  IResource *PseudoCallResource();
  IResource *PrintResource();
  IResource *GetOpResource(vm::OpCode op, IValueType &vt);

  IResource *GetImportedResource(vm::Method *method);
  IResource *GetExternalArrayResource();
  IResource *GetInternalArrayResource(vm::Object *obj);
  IResource *GetChannelResource(vm::Object *ch, bool is_write,
				int data_width);
  IResource *GetSubModuleTaskResource();
  IResource *GetDataFlowInResource();
  IResource *GetMemberSharedReg(sym_t name, bool is_owner,
				bool is_write);
  IResource *GetSharedArray(vm::Object *obj, bool is_owner, bool is_write);
  IResource *GetAxiMasterPort(vm::Object *obj);
  IResource *GetAxiSlavePort(vm::Object *obj);
  IResource *GetMailbox(vm::Object *obj, bool is_owner, bool is_put);
  IResource *GetTaskReturnRegWriter(int width);
  IResource *GetExtIO(const string &name, bool is_output, int width);

private:
  string GetResourceClassName(vm::OpCode op);
  void PopulateResourceDataType(int op, IValueType &vt, IResource *res);

  ITable *tab_;
  IResource *assert_;
  IResource *assign_;
  IResource *br_;
  IResource *print_;
  IResource *mem_if_;
  IResource *pseudo_call_;
  IResource *task_entry_;
  IResource *dataflow_in_;
  IResource *task_return_reg_;
  IResource *task_return_reg_writer_;

  class ResourceEntry {
  public:
    vm::OpCode op;
    IValueType vt;
    IResource *resource;
  };
  vector<ResourceEntry> resources_;

  vector<IResource *> imported_resources_;
  map<vm::Object *, IResource *> array_resources_;
  map<vm::Object *, IResource *> channel_resources_;
  map<sym_t, IResource *> member_shared_reg_;
  map<sym_t, IResource *> member_shared_reg_writer_;
  map<sym_t, IResource *> member_shared_reg_reader_;
  map<vm::Object *, IResource *> shared_array_;
  map<vm::Object *, IResource *> shared_array_writer_;
  map<vm::Object *, IResource *> shared_array_reader_;
  map<vm::Object *, IResource *> axi_master_ports_;
  map<vm::Object *, IResource *> axi_slave_ports_;
  map<vm::Object *, IResource *> mailbox_shared_reg_;
  map<vm::Object *, IResource *> mailbox_putters_;
  map<vm::Object *, IResource *> mailbox_getters_;
  map<string, IResource *> ext_io_;
};

}  // namespace synth

#endif  // _synth_resource_set_h_
