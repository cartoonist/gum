GUM - Sequence Graph Library
============================

This library provides fast, efficient, and versatile data structures for
representing sequence graphs:

- GUM provides both dynamic and succinct representations for directed and
  bi-directed sequence graph,
- GUM is written in C++ -- so it's fast,
- GUM is flexible because it is designed to be extended easily using template
  programming,
- GUM provides Python interface so that it can be used interactively in Python,
- GUM works with different file formats: GFA, vg, and graph-tool.

How to Install
--------------

### Dependencies

#### Minimum requirements

GUM uses many features introduced in C++17. In order to work with it, you need a
C++17 compliant compiler first. It has been tested for these compilers:

| Compiler | Version  |
| -------- | -------- |
| GCC      | 8.3, 9.2 |

GUM is a header-only library. However, it depends on a few external libraries to
work:

- Zlib (REQUIRED)
- Google Sparse Hash (REQUIRED) >=2.0.2
- sdsl-lite (REQUIRED) >=2.1.0

These dependencies can usually be installed using the package manager of most
operating systems. GUM also comes with a build system which checks for
dependencies, installs the library, and builds/runs tests which requires to have
CMake >=3.10.

Under Debian-based operating systems, all dependencies can be easily
installed by:

```bash
$ sudo apt install cmake zlib1g-dev libsparsehash-dev libsdsl-dev
```

The default format for input graph files is GFA. GUM uses GFAKluge library to
parse GFA files which is bundled (no need to be installed). If GFAKluge is
already installed on your system, GUM will use the installed version, otherwise
it installs GFAKluge too.

#### Optional dependencies

If you want to use VG as an input graph, you need to have
[`libvgio`](https://github.com/vgteam/libvgio) installed.
You may refer to its GitHub page for the installation guide. But, here is
probably the easiest way to build and install VGio from source code:

##### Installing `libvgio`

`libvgio` requires `pkg-config`, Google Protobuf, and `htslib`. They usually can
be found in most operating system repositories. For example, for Debian-based
systems:

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

### Installation

The build script checks if dependencies are met and copies the header files to
the destination directory (`/usr/local/include` by default). If GFAKluge is not
installed, it will be installed alongside GUM. It also installs CMake and
`pkg-config` find modules and configuration files in order to make it easier to
include GUM in other projects using CMake or `pkg-config`.

To install GUM:

```bash
$ git clone https://github.com/cartoonist/gum.git
$ mkdir gum/build
$ cd gum/build
$ cmake ..
$ make
$ sudo make install
```

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

In order to build the tests, just turn on `BUILD_TESTING` option when running
`cmake`:

```bash
$ cmake -DBUILD_TESTING=ON ..
```
