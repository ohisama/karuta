// -*- C++ -*-
#ifndef _synth_object_synth_h_
#define _synth_object_synth_h_

#include "synth/common.h"

#include <set>

namespace synth {

class ObjectSynth {
public:
  ObjectSynth(vm::Object *obj,
	      DesignSynth *design_synth);
  virtual ~ObjectSynth();

  void SetName(const char *name);
  void AddEntryName(const string &task_entry);
  bool Synth();

  vm::VM *GetVM() const;
  vm::Object *GetObject() const;
  ChannelSynth *GetChannelSynth() const;
  IModule *GetIModule();
  ThreadSynth *GetThreadByName(const string &name);

private:
  void CollectThreads(IModule *mod);
  void CollectSubModuleCalls();
  void ResolveSubModuleCalls();

  vm::Object *obj_;
  string obj_name_;
  std::set<string> task_entries_;
  DesignSynth *design_synth_;
  IModule *mod_;
  vector<ThreadSynth *> threads_;
  vector<ObjectSynth *> member_objs_;
};

}  // namespace synth

#endif  // _synth_object_synth_h_
