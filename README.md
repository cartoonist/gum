GUM - Graph Unified Model
=========================

Sequence Graph Library
----------------------

This library provides fast, efficient, and versatile data structures for
representing sequence graphs and mainly developed for
[PSI](https://github.com/cartoonist/psi) library:

- GUM provides both dynamic and succinct representations for directed and
  bi-directed sequence graphs,
- GUM is written in C++ -- so it's fast!
- GUM is a header-only library, it is easy to be integrated in a project,
- GUM is generic and flexible and is designed to be extended easily using
  template programming,
- GUM provides Python interface so that it can be used interactively in Python
  (incomplete),
- GUM works with different file formats: GFA, vg (libvgio), HashGraph (libbdsg), and
  graph-tool.

This library mainly introduces a generic data structure called `SeqGraph` for
representing sequence graphs and some interface functions for working with this
data structure.

Table of Contents
-----------------
- [Structure](#structure)
- [Dependencies](#dependencies)
  - [Minimum requirements](#minimum-requirements)
  - [Required dependencies](#required-dependencies)
  - [Optional dependencies](#optional-dependencies)
- [Configuration](#configuration)
  - [CMake configuration options](#cmake-configuration-options)
  - [C++ macro identifiers](#c-macro-identifiers)
- [Using GUM](#using-gum)
  - [As a submodule](#as-a-submodule)
  - [As an external dependency](#as-an-external-dependency)
  - [As a part of the source tree](#as-a-part-of-the-source-tree)
- [Installation](#installation)
  - [Using conda](#using-conda)
  - [Install from source code](#install-from-source-code)
- [Development](#development)

Structure
---------

In order to avoid imposing unwanted dependencies, the library is modularised.
It comes with a core and some *optional* extensions:

```text
GUM
├── core (*.hpp except for those below)
│   └── internal    external header-only dependencies comes with the library
├── vg_utils.hpp:   interface functions for loading vg::Graph objects
├── hg_utils.hpp:   interface functions for loading bdsg::HashGraph objects
├── vgio_utils.hpp: IO helper functions for parsing vg files
└── bdsg_utils.hpp: IO helper functions for parsing HashGraph files
```

GUM core library depends on a very few external libraries most of which are
either usually installed on most systems or are already embedded in the library
as internal modules (see [Dependencies](#dependencies)).

In order to use the core library, one only needs to include `graph.hpp` for
working with data structures or algorithms and to include `io_utils.hpp` for IO
operations. The default input file format for graph in the core module is GFA.

*⤷ In case GFA is only format you work with, skip to [the next section](#dependencies).*

Two header file `vg_utils.hpp` and `hg_utils.hpp` are standalone header files
providing some utility functions for loading or extending a `SeqGraph` from a
`vg::Graph` and `bdsg::HashGraph` objects or related ones (e.g. `vg::Nodes`)
respectively. This delegates parsing the input vg or HashGraph files and
consequently linking with required libraries to the library user. Including
these header files does not impose any further dependencies by their own.

Other extensions -- i.e. `vgio_utils.hpp` or `bdsg_utils.hpp` -- requires
`libvgio` and `libbdsg` libraries to be installed respectively. These extensions
provide further io support for parsing vg (protobuf) or `HashGraph` files as
graph input files. GUM relies on these two libraries for dealing with vg file
formats instead of re-implementing them.

Dependencies
------------

### Minimum requirements

GUM uses many features introduced in C++17. In order to work with it, you need a
C++17 compliant compiler first. It has been tested for these compilers:

| Compiler   | Version                  |
| ---------- | ------------------------ |
| GCC        | 8.3, 9.2, 10.2.0, 12.2.0 |
| AppleClang | 14.0.0                   |

### Required dependencies

- Zlib
- CMake >=3.19 (for configuration only)

The core library only depends on zlib which is usually installed on most
systems. If not, both zlib and CMake are available in most package repositories:

```bash
$ sudo apt install zlib1g-dev cmake  # on Debian-based operating systems
```

*⤷ If you are using the core library (i.e. without vg support), proceed to
[Configuration section](#configuration).*

GUM internally utilises three header-only libraries which are embedded in the
library (in `include/gum/internal` directory after configuration). So, there is
no need to install these libraries separately.

- [Parallel Hashmap](https://github.com/greg7mdp/parallel-hashmap)
- [sdsl-lite](https://github.com/cartoonist/sdsl-lite/tree/header-only)
  a fork of [xxsds/sdsl-lite](https://github.com/xxsds/sdsl-lite)
- [GFAKluge](https://github.com/cartoonist/gfakluge) a fork of
  [edawson/GFAKluge](https://github.com/edawson/gfakluge)

### Optional dependencies

The default format support for input graph files is GFA. As we mentioned above,
GUM can also parse Protobuf `vg` and `HashGraph` files which requires `libvgio`
and `libbdsg` respectively.  In order to keep the dependency list as small as
possible, these modules are optional:

- [`libvgio`](https://github.com/vgteam/libvgio/tree/205b81751d46604aa6f9189d26c098b511c5e402) == vgteam/libvgio@205b817
- [`libbdsg`](https://github.com/vgteam/libbdsg/tree/704567184a3cfacfcb1d51e58be771b01c25ab15) == vgteam/libbdsg@7045671

They can be built from source code which are often not straightforward. The good
news is that GUM can build these libraries automatically and install it in the
build directory (i.e. `${PROJECT_BINARY_DIR}/vcpkg_installed`) using
[vcpkg](https://github.com/microsoft/vcpkg) which is explained in the next
section. We go through both approaches below.

#### Using vcpkg

This is the easiest way to handle dependencies. However, it is not enabled by
default. If enabled, GUM will build required dependencies based on selected
modules during [configuration](#configuration). This functionality can be
enabled by setting CMake option `USE_VCPKG` to `on` or `TRUE`; for example by
passing `-DUSE_VCPKG=on` to CMake in configuration phase.

In this mode, GUM automatically downloads vcpkg into the build directory and
bootstraps it. vcpkg is a free C/C++ package manager developed by Microsoft for
fetching and building libraries.  It uses a fork of vcpkg since some ports files
(recipes) for building required packages are not yet merged with the vcpkg main
repository.

All dependencies will be installed in `vcpkg_installed` directory located in the
project build directory; i.e. `/path/to/gum/build` if you created the `build`
directory inside the source directory.

**Note** that vcpkg is not designed to install the dependencies system-wide or
user-wide. Therefore, you should keep build directory for future use in which
installed libraries can be found and loaded at runtime or by the CMake when
building downstream projects.

The installed libraries by vcpkg can be found and used by an external CMake
project by setting `CMAKE_TOOLCHAIN_FILE` to
`<vcpkg-root>/scripts/buildsystems/vcpkg.cmake` where `<vcpkg-root>` is
`/path/to/gum/build/vcpkg` assuming that the build directory is located at
`/path/to/gum/build`. It is done by default if you call `add_subdirectory(gum)`
in your `CMakeLists.txt` file. Check out [the vcpkg
documentation](https://vcpkg.io/en/docs/users/buildsystems/cmake-integration.html)
for further details.

#### Manually build dependencies

You may refer to their GitHub pages for the installation guide. But, here is
probably the easiest way to build and install `libvgio` and `libbdsg` from
source code:

##### Installing `libvgio`

<details>
<summary>Read more</summary>

`libvgio` requires `pkg-config`, Google Protobuf, `jansson`, and `htslib`. They
can be usually found in most operating system repositories. For example, for
Debian-based systems:

```bash
$ sudo apt install pkg-config libprotobuf-dev protobuf-compiler libhts-dev libjansson-dev
```

or using conda:

```bash
$ conda install pkgconfig libprotobuf protobuf htslib jansson
```

or homebrew:

```bash
$ brew install pkg-config protobuf htslib jansson
```

Then, building and installing is quite straightforward:

```bash
$ git clone https://github.com/vgteam/libvgio
$ cd libvgio/build
$ cmake ..
$ make
$ sudo make install
```

</details>

##### Installing `libbdsg`

<details>
<summary>Read more</summary>

Installing `libbdsg` can be tricky, since both this library and `libvgio` might
install different versions of `libhandlegraph`. This makes the linking a hassle.
To prevent such problems, make sure that you use `Makefile` provided by
`libbdsg` to build and install the library (instead of CMake build script).
Consult with [Building](https://github.com/vgteam/libbdsg#building) section of
its README. This requires to install
[a few dependencies](https://github.com/vgteam/libbdsg#dependencies) first which
are mostly header-only.

</details>

Configuration
-------------

The library needs to be configured before use. Please note that
**pre-configured packages** can be found in
[Releases](https://github.com/cartoonist/gum/releases).

In the configuration phase which is done by CMake, one can specifies which
modules should be included in the library. Based on selected modules, it also
checks if dependencies are met.

Additionally, it generates a `config.hpp` file containing some necessary macro
name definitions and version information.

*⤷ If you are relying on default configurations, skip to [the next section](#using-gum).*

### CMake configuration options

- Options for including additional modules:
  - `WITH_VG`: if `on`, `vg_utils.hpp` is included supporting loading vg
    objects. As described [above](#structure), it does not explicitly impose a
    dependency on `libvgio` and it is assumed that the caller handles parsing
    input files (`on` by default). No inclusion order is imposed by the library
    either; i.e. it does not matter if vg namespace is defined before including
    the header file.  This is useful when the library is installed system-wide
    and not all downstream projects are supposed to work with vg.
  - `WITH_VGIO`: if `on`, it includes io supports for vg files by including
    `vgio_utils.hpp` which utilise `libvgio` for parsing vg files (`off` by
    default).
  - `WITH_HG`: if `on`, `hg_utils.hpp` is included only without imposing
    `libbdsg` with no inclusion order obligations (`on` by default).
  - `WITH_BDSG`: if `on`, it includes io supports for vg files with HashGraph
    format by including `bdsg_utils.hpp` which uses `libbdsg` (`off` by
    default).
  - `PROTOBUF_AS_DEFAULT_VG`: if `on`, Protobuf vg file format is considered as
    the default format for 'vg' files rather than `HashGraph` (`on` by default).
- Options for handling dependencies:
  - `USE_VCPKG`: use vcpkg for building dependencies (`off` by default). See
    [Dependencies](#dependencies) for more details.
- Development options:
  - `BUILD_GUM_AUX_TOOLS`: Build some auxiliary tools (`off` by default).
  - `BUILD_TESTING`: Build test scenarios (`off` by default).

### C++ macro identifiers

The `config.hpp` defines following macros names indicating which modules are
present:

- `GUM_HAS_VG`
- `GUM_HAS_HG`
- `GUM_HAS_VGIO`
- `GUM_HAS_BDSG`

Using GUM
---------

*⤷ This section describes how to use GUM in a project for developers. For
installing GUM as a dependency of other tools like PSI, skip to section
"[Installation](#installation)".*

Header-only libraries are usually easy to be integrated in a project by just
copying the header files (i.e. `include/gum` directory) in the source code.
*However*, since GUM has a few external dependencies, it might be easier to use
CMake for integration.

There are different ways to use GUM in a project:

### As a submodule

If you are using CMake for your project, the easiest way would be including GUM
as a git submodule. Then, calling `add_subdirectory(path/to/gum)` in the
corresponding `CMakeLists.txt` file exports `gum::gum` target. The exported
target defines include directories, transitive dependencies, and compiler flags
required for building. Adding target `gum::gum` to `target_include_directories`
or `target_link_libraries` will provide necessary information for compiling and
linking the required libraries.

In addition, if you are using GUM with vg support it is recommended to
[use vcpkg](#using-vcpkg) which automatically builds required dependencies. To
do so, set `USE_VCPKG` to `on` before calling `add_subdirectory`.

[Other CMake options](#cmake-configuration-options) can also be set before
calling `add_subdirectory`.

Example:

``` cmake
cmake_minimum_required(VERSION 3.19)
project(example VERSION 0.0.1 LANGUAGES CXX)

# Add GUM if it is not present
if(NOT TARGET gum::gum)
  set(GUM_SOURCE_DIR path/to/gum)
  execute_process(COMMAND git submodule update --init -- ${GUM_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  #set(USE_VCPKG on)  # uncomment for using vcpkg
  add_subdirectory(${GUM_SOURCE_DIR} EXCLUDE_FROM_ALL)
endif()

# Defining target 'example'
add_executable(example)
target_include_directories(example
  PRIVATE gum::gum)
target_link_libraries(example
  PRIVATE gum::gum)
```

### As an external dependency

Install GUM as an external dependency (see [Installation](#installation)).
In CMake, `find_package(gum)` will import `gum::gum` target and it can be used
just like the previous approach.

If not using CMake, the library and its required parameters for compiling and
linking can be found by using `pkg-config`:

``` shell
$ pkg-config --libs gum
$ pkg-config --cflags gum
```

### As a part of the source tree

Download and extract pre-configured packages in
[Releases](https://github.com/cartoonist/gum/releases). Copy the `include/gum`
directory into the source tree.

The required external libraries such as zlib should be found and linked manually
in the corresponding build system (see [Dependencies](#dependencies)). For the
reference, following flags are required for the _core_ library:

```
-I/path/to/include -I/path/to/include/gum/internal -fopenmp -lpthread -latomic -lz
```

Installation
------------

### Using conda

The library is distributed on bioconda and can be installed by:

```shell
conda install -c bioconda gum
```

Note that since `libvgio` and `libbdsg` are not available on conda, the
installed version only supports parsing GFA files. More specifically, it is
packaged with default CMake options `-DWITH_VG=on -DWITH_HG=on -DWITH_VGIO=off
-DWITH_BDSG=off`.

### Install from source code

The build script configures the library, checks if dependencies are met, and
copies the header files to the destination directory (`/usr/local/include` by
default). So, by default, there is nothing to be compiled. The build script also
installs CMake and `pkg-config` find modules and configuration files in order to
make it easier to include GUM in other projects using CMake or `pkg-config`.

To install GUM core library (recommended):

```bash
$ git clone https://github.com/cartoonist/gum.git
$ mkdir gum/build
$ cd gum/build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
$ sudo make install
```

In case you jumped over some sections above, it should be noted that, some
CMake options might be necessary to be added to the `cmake` command; for example
`-DCMAKE_INSTALL_PREFIX=/path/to/install/prefix` specifies a custom path for
installation (rather than `/usr/local`) or use `WITH_*` options for selecting
optional modules to be included in the installation (see
[Structure](#structure) and
[CMake configuration options](#cmake-configuration-options) for further details).

As explained above, if you pass `-DUSE_VCPKG=on` to the `cmake` command, the
required dependencies will be installed during configuration time. This is only
necessary when options enabling io supports for vg/HashGraph files (i.e.
`-DWITH_VGIO` and `-DWITH_BDSG`) are provided.

Development
-----------

In order to build the tests or auxiliary tools, just turn on `BUILD_TESTING` or
`BUILD_GUM_AUX_TOOLS` options when running `cmake`. Both tests and auxiliary
tools need the full library with all modules included:

```bash
$ cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=on -DBUILD_GUM_AUX_TOOLS=on -DWITH_VGIO=on -DWITH_BDSG=on ..
```
