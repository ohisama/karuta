#include "karuta/env.h"

#include "base/util.h"

#include <set>
#include <string.h>

using std::set;

const char *Env::karuta_dir_;
string Env::output_root_;
string Env::output_marker_;
string Env::module_prefix_;
bool Env::sandbox_mode_;
string Env::argv0_;
string Env::iroha_bin_path_;
vector<string> Env::source_dirs_;
string Env::current_file_;
long Env::duration_ = 1000000;
bool Env::dot_output_;
string Env::flavor_;
bool Env::with_self_shell_;
bool Env::vcd_output_;

const string &Env::GetVersion() {
  static string v(VERSION);
  return v;
}

const char *Env::GetKarutaDir() {
  if (!karuta_dir_) {
    karuta_dir_ = getenv("KARUTA_DIR");
  }
  if (!karuta_dir_) {
#ifdef KARUTA_DIR
    karuta_dir_ = KARUTA_DIR;
#else
    karuta_dir_ = ".";
#endif
  }
  return karuta_dir_;
}

const char *Env::GetIrohaDataDir() {
  return getenv("IROHA_DATA_DIR");
}

void Env::SearchPathList(const char *fn,
			 vector<string> *paths) {
  if (fn[0] != '/' && fn[0] != '.') {
    auto dirs = SearchDirList();
    for (string &d : dirs) {
      paths->push_back(d + fn);
    }
  }
  paths->push_back(string(fn));
}

vector<string> Env::SearchDirList() {
  vector<string> dirs = source_dirs_;
  dirs.push_back(Env::GetKarutaDir() + string("/"));
  const char *iroha_data = GetIrohaDataDir();
  if (iroha_data != nullptr) {
    dirs.push_back(iroha_data + string("/"));
  }
  return dirs;
}

bool Env::GetOutputPath(const string &fn, string *path) {
  if (output_root_.empty()) {
    *path = fn;
    return true;
  }
  if (sandbox_mode_) {
    if (!strncmp(fn.c_str(), "../", 3)) {
      return false;
    }
    if (strstr(fn.c_str(), "/../")) {
      return false;
    }
  }
  *path = output_root_ + string("/") + fn;
  return true;
}

const string &Env::GetOutputMarker() {
  return output_marker_;
}

void Env::SetOutputRootPath(const string &path) {
  output_root_ = path;
  sandbox_mode_ = true;
}

void Env::SetOutputMarker(const string &marker) {
  output_marker_ = marker;
}

void Env::SetModulePrefix(const string &name) {
  module_prefix_ = name;
}

const string & Env::GetModulePrefix() {
  return module_prefix_;
}

void Env::SetArgv0(const string &c) {
  argv0_ = c;
}

const string &Env::GetArgv0() {
  return argv0_;
}

void Env::SetCurrentFile(const string &fn) {
  current_file_ = fn;
  string dn = Util::DirName(fn);
  if (dn.empty()) {
    return;
  }
  for (string &s : source_dirs_) {
    if (s == dn) {
      return;
    }
  }
  source_dirs_.push_back(dn);
}

const string &Env::GetCurrentFile() {
  return current_file_;
}

bool Env::IsSandboxMode() {
  return sandbox_mode_;
}

void Env::SetIrohaBinPath(const string &iroha) {
  iroha_bin_path_ = iroha;
}

const string &Env::GetIrohaBinPath() {
  return iroha_bin_path_;
}

long Env::GetDuration() {
  return duration_;
}

void Env::SetDuration(long duration) {
  duration_ = duration;
}

void Env::EnableDotOutput(bool en) {
  dot_output_ = en;
}

bool Env::DotOutput() {
  return dot_output_;
}

const string &Env::GetFlavor() {
  return flavor_;
}

void Env::SetFlavor(const string &flavor) {
  flavor_ = flavor;
}

bool Env::GetWithSelfShell() {
  return with_self_shell_;
}

void Env::SetWithSelfShell(bool with_self_shell) {
  with_self_shell_ = with_self_shell;
}

void Env::EnableVcdOutput(bool en) {
  vcd_output_ = en;
}

bool Env::GetVcdOutput() {
  return vcd_output_;
}
