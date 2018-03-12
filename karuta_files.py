import sys
sys.path.append("iroha/")
import iroha_files
sys.path.append("tests/")
import test_files

VERSION = "0.3.1wip"

EXTRA = ["README.md", "NEWS", "configure", "Makefile", "config.mk", "karuta", "src/karuta.gyp", "iroha/src/iroha.gyp", "src/fe/parser.ypp", "genparser.py", "karuta_files.py", "docs/LRM.md", "examples/top.karuta"]

LIB_FILES = ["lib/karuta.in", "lib/default-isynth.karuta", "lib/karuta_wait.v", "lib/karuta_exit.v", "lib/karuta_server.py", "lib/karuta_wrapper.py", "lib/karuta_examples.py"]

ifiles = iroha_files.GetGypFileList("iroha/src/iroha.gyp")
iextras = iroha_files.GetExtraFileList("iroha/")
kfiles = iroha_files.GetGypFileList("src/karuta.gyp")

tfiles = test_files.GetFiles("tests/")

files = kfiles + EXTRA + LIB_FILES + ifiles + iextras + tfiles

iroha_files.MakeTarBall("karuta-" + VERSION, files)
