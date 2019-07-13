// -*- C++ -*-
#ifndef _synth_object_synth_h_
#define _synth_object_synth_h_

#include "synth/common.h"

#include <set>

namespace synth {

class ObjectSynth {
public:
  ObjectSynth(vm::Object *obj,
	      DesignSynth *design_synth,
	      bool is_root, const string &name);
  virtual ~ObjectSynth();

  static bool HasSynthesizable(vm::Object *obj);

  void AddTaskEntryName(const string &task_entry);
  bool Scan(bool *ok);
  bool Synth();
  void ResolveTableCallsAll();
  void DeterminePrimaryThread();
  const string &GetName() const;

  vm::VM *GetVM() const;
  vm::Object *GetObject() const;
  IModule *GetIModule();
  ThreadSynth *GetThreadByName(const string &name);
  DesignSynth *GetDesignSynth();
  const vector<ThreadSynth *> &GetAllThreads() const;

private:
  void CollectThreads(IModule *mod);
  void CollectSubModuleCalls();
  void ResolveTableCall(const TableCall &call);
  void CollectExtEntries(vector<string> *entries);

  vm::Object *obj_;
  string obj_name_;
  DesignSynth *design_synth_;
  IModule *mod_;
  bool is_root_;
  vector<ThreadSynth *> threads_;
  std::set<ThreadSynth *> scanned_threads_;
  std::set<string> task_entry_names_;
};

}  // namespace synth

#endif  // _synth_object_synth_h_
