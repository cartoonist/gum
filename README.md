GUM - Sequence Graph Library
============================

This library provides fast, efficient, and versatile data structures for
representing sequence graphs and mainly developed for
[PSI](https://github.com/cartoonist/psi) library:

- GUM provides both dynamic and succinct representations for directed and
  bi-directed sequence graphs,
- GUM is written in C++ -- so it's fast,
- GUM is flexible because it is designed to be extended easily using template
  programming,
- GUM provides Python interface so that it can be used interactively in Python
  (incomplete),
- GUM works with different file formats: GFA, vg (libvgio), HashGraph (libbdsg), and
  graph-tool.

How to Install
--------------

### Dependencies

#### Minimum requirements

GUM uses many features introduced in C++17. In order to work with it, you need a
C++17 compliant compiler first. It has been tested for these compilers:

| Compiler | Version          |
| -------- | ---------------- |
| GCC      | 8.3, 9.2, 10.2.0 |

GUM is a header-only library. However, it depends on a few external libraries to
work. Note that there are three groups of dependencies: required, bundled, and optional:

#### Required dependencies

- CMake (REQUIRED) >=3.10
- Zlib (REQUIRED)
- [sdsl-lite](https://github.com/vgteam/sdsl-lite/tree/v2.2-vgteam) (REQUIRED) ==2.2-vgteam

The required libraries need to be installed on the system before building GUM. The first
two can be usually installed by package managers in most operating systems. For example,
for Debian-based operating systems:

```bash
$ sudo apt install cmake zlib1g-dev
```

Although, the sdsl package is also available in some repositories, it is **strongly
recommended** to stick with the designated version above. This library requires _vg-team
fork_ of the sdsl library which needs to be install from the source code.

#### Bundled dependencies

The bundled dependencies can be built and installed by GUM itself:

- [Parallel Hashmap](https://github.com/greg7mdp/parallel-hashmap/tree/1.32) (Bundled) ==1.32
- [GFAKluge](https://github.com/edawson/gfakluge) (Bundled)

To prevent installing multiple versions, GUM does not install bundled dependencies by
default. If you want GUM to handle for example the Parallel Hashmap library, pass
`-DUSE_BUNDLED_PARALLEL_HASHMAP=on` when calling cmake. Then, GUM build system
(requiring CMake >=3.10) checks for its availability, and installs them if not present.
This is also the same for GFAKluge (by `-DUSE_BUNDLED_GFAKLUGE=on`).

#### Optional dependencies

The default format support for input graph files is GFA. However, GUM can also parse
Protobuf `vg` and `HashGraph` files which requires `libvgio` and `libbdsg` respectively.
In order to keep the dependency list as small as possible, these libraries are optional:

- [`libvgio`](https://github.com/vgteam/libvgio/tree/3e184fd29e116221aef6d32f297eab8257b6e2b9) == vgteam/libvgio@3e184fd
- [`libbdsg`](https://github.com/vgteam/libbdsg/tree/v0.3) == v0.3

In order to build the library with their supports, it is required to pass
`-DBUILD_WITH_VG=on` or `-DBUILD_WITH_BDSG=on` to include `vg` or `HashGraph` support
respectively. By default `BUILD_WITH_VG` is `on` and `BUILD_WITH_BDSG` is `off`, that is
`HashGraph` support is excluded by default. These features can be turned `on` or `off`
independently.

In addition, GUM does not have these libraries bundled, so you need to install the
corresponding libraries first in case you enable any of these features. You may refer to
their GitHub pages for the installation guide. But, here is probably the easiest way to
build and install `libvgio` and `libbdsg` from source code:

##### Installing `libvgio`

`libvgio` requires `pkg-config`, Google Protobuf, and `htslib`. They can be usually
found in most operating system repositories. For example, for Debian-based systems:

```bash
$ sudo apt install pkg-config libprotobuf-dev protobuf-compiler libhts-dev
```

Then, building and installing is quite straightforward:

```bash
$ git clone https://github.com/vgteam/libvgio
$ cd libvgio/build
$ cmake ..
$ make
$ sudo make install
```

##### Installing `libbdsg`

Installing `libbdsg` can be tricky, since both this library and `libvgio` (and probably
other vg-related libraries) will install multiple versions of `libhandlegraph`. This
makes the linking a hassle. To prevent such problems, make sure that you use `Makefile`
provided by `libbdsg` to build and install the library (not CMake build script!).
Consult with [Building](https://github.com/vgteam/libbdsg#building) section of its
README. This requires to install [a few
dependencies](https://github.com/vgteam/libbdsg#dependencies) first which are mostly
header-only.


### Installation

The build script checks if dependencies are met and copies the header files to
the destination directory (`/usr/local/include` by default). So, by default, there is
nothing to be compiled expect for any bundled dependency that needs to be compiled. The
build script also installs CMake and `pkg-config` find modules and configuration files
in order to make it easier to include GUM in other projects using CMake or `pkg-config`.

To install GUM (recommended):

```bash
$ git clone https://github.com/cartoonist/gum.git
$ mkdir gum/build
$ cd gum/build
$ cmake -DCMAKE_BUILD_TYPE=Release -DUSE_BUNDLED_PARALLEL_HASHMAP=on -DUSE_BUNDLED_GFAKLUGE=on ..
$ make
$ sudo make install
```

If the dependencies are installed in a custom path, you can give a hint to the build
script to find them when calling `cmake` by passing
`-DCMAKE_PREFIX_PATH=/path/to/custom/prefix`. You can also install GUM in a custom path
(rather than `/usr/local`) by passing `-DCMAKE_INSTALL_PREFIX=/path/to/install/prefix`.

Use GUM
-------

There are two ways to include GUM into your project:

### As a git submodule

If you are using CMake, the library can be added to your project as a git
submodule. Then, calling `add_subdirectory(path/to/gum)` in the corresponding
`CMakeLists.txt` file exports `gum::gum` target. The exported target defines
include directories, transitive dependencies, and compiler flags required for
building the project.


### TODO: As an external dependency

Install the GUM as an external dependency. If you are using CMake `find_package`
will import `gum::gum` target to which can be used to link your project.

If you are not using CMake, the same information can also be retrieved using
`pkg-config`. Or just take a look at `<INSTALL_PREFIX>/lib/pkgconfig/gum.pc`.

Development
-----------

In order to build the tests or auxiliary tools, just turn on `BUILD_TESTING` or
`BUILD_AUX_TOOLS` options when running `cmake`:

```bash
$ cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=on -DBUILD_AUX_TOOLS=on ..
```
