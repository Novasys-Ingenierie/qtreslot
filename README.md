QtReslot is a Clang plugin that converts string-based signal/slot connections
to the new Qt5 syntax, enabling compile-time connection checking. It is most
useful for large, old projects where such changes can quickly require a huge
amount of time.

## Installation

QtReslot currently requires Clang/LLVM 3.8 but should run with more recent
versions, provided some manual modifications are applied to the Makefile.

Building and installing is simply done by running make. Think of the -j
option to reduce build time. The resulting qtreslot.so file has no
requirements concerning file system paths.

Contributions for a more modern build system are welcome.

## Usage

Add these lines to your Qt project files :

```
QMAKE_CXX = clang++-3.8
QMAKE_LINK = clang++-3.8

QMAKE_CXXFLAGS += -Xclang -load
QMAKE_CXXFLAGS += -Xclang /path/to/qtreslot.so
QMAKE_CXXFLAGS += -Xclang -add-plugin
QMAKE_CXXFLAGS += -Xclang rewrite_signals_and_slots
```

Ideally, these lines should be added to a Qt project include (.pri) file.

You may then rebuild your entire project. During the build, the plugin
creates .qtreslot files for sources where a conversion was made. Once the
build is done, you can use the qtreslot\_rename.sh script from the source
root directory to replace your original source files with the generated files.

## Disclaimer

Like most tools, QtReslot isn't a silver bullet. It won't magically solve
all problems. The [Open Questions](https://wiki.qt.io/New_Signal_Slot_Syntax)
mentioned on the Qt wiki must be addressed manually, in particular overloading.
The good news is that it's easy to rely on the compiler to locate the problems
and then compare the two versions of a signal/slot occurrence and revert to
the old version or decide to make the new one work.
