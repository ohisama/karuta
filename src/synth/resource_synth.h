// -*- C++ -*-
#ifndef _synth_resource_synth_h_
#define _synth_resource_synth_h_

#include "synth/common.h"

namespace synth {

class ResourceSynth {
public:
  ResourceSynth(ResourceSet *rset, ThreadSynth *thr_synth_);

  void MayAddAxiMasterPort(vm::Object *owner_obj, vm::Object *array_obj);
  void MayAddAxiSlavePort(vm::Object *owner_obj, vm::Object *array_obj);
  IResource *MayAddExtIO(vm::Method *method, bool is_output);
  void MayAddSharedRegExtWriter(vm::Object *mailbox_obj);

private:
  void SetArrayName(vm::Object *owner_obj, vm::Object *array_obj, IResource *res);

  ResourceSet *rset_;
  ThreadSynth *thr_synth_;
};

}  // namespace synth

#endif  // _synth_resource_synth_h_
