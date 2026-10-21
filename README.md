# latero-graphics

Tactile rendering library for the Latero device. See [OpenLatero.org](http://OpenLatero.org) for more information.

Automatically-generated documentation for the latest version on GitHub can be found [here](https://openlatero.github.io/latero-graphics/).

## Installation

Expand the instructions for your platform.

<details>
<summary><b>macOS</b></summary>

_Last tested on macOS 27.0._

#### Install OpenLatero libraries

Install the [latero](https://github.com/OpenLatero/latero) library.

#### Install dependencies

Install the dependencies through [Homebrew](https://brew.sh/):

```
brew install gtkmm4 libxml++@5 ImageMagick pkg-config
```

or through [MacPorts](https://www.macports.org):

```
sudo port install gtkmm4 +quartz libxmlxx5 ImageMagick
```

#### Build and install the library

The library can be compiled and installed with the following commands:

```
cmake --preset default
cmake --build build
sudo cmake --install build
```

or in a single command:

```
cmake --preset default && cmake --build build && sudo cmake --install build
```

The library is installed under `/usr/local`. To install it elsewhere, set the prefix when configuring
(`cmake --preset default -DCMAKE_INSTALL_PREFIX=<dir>`), not with `cmake --install --prefix`: the location of the
stock textures is compiled into the library.

#### Build the documentation

Run this command to build and open the documentation:

```
cmake --build build --target doc && open docs/html/index.html
```

</details>

<details>
<summary><b>Linux</b></summary>

_Last tested on Ubuntu 26.04._

#### Install OpenLatero libraries

Install the [latero](https://github.com/OpenLatero/latero) library.

#### Install dependencies

Install the dependencies with:

```
sudo apt install libgtkmm-4.0-dev libxml++-5.0-dev libmagick++-dev pkg-config
```

#### Build and install the library

The library can be compiled and installed with the following commands:

```
cmake --preset default
cmake --build build
sudo cmake --install build
```

or in a single command:

```
cmake --preset default && cmake --build build && sudo cmake --install build
```

The library is installed under `/usr/local`. To install it elsewhere, set the prefix when configuring
(`cmake --preset default -DCMAKE_INSTALL_PREFIX=<dir>`), not with `cmake --install --prefix`: the location of the
stock textures is compiled into the library.

#### Build the documentation

Run this command to build and open the documentation:

```
cmake --build build --target doc && xdg-open docs/html/index.html
```

</details>

<details>
<summary><b>Windows</b></summary>

_Last tested on Windows 11._

On Windows, the library is built natively with the GCC toolchain of [MSYS2](https://www.msys2.org). Windows 11 is
required: the real-time update loop relies on its high-resolution timers and power-throttling controls.

#### Install OpenLatero libraries

Install the [latero](https://github.com/OpenLatero/latero) library, following its Windows instructions. This also sets
up MSYS2 and the build tools.

#### Install dependencies

Open the **MSYS2 UCRT64** shell and install the dependencies with the following command:

```
pacman -S --needed mingw-w64-ucrt-x86_64-gtkmm-4.0 mingw-w64-ucrt-x86_64-libxml++-5.0 mingw-w64-ucrt-x86_64-imagemagick mingw-w64-ucrt-x86_64-pkgconf
```

Run all the commands below from the same UCRT64 shell.

#### Build and install the library

The library can be compiled and installed into the MSYS2 environment (no `sudo` needed) with the following commands:

```
cmake --preset default
cmake --build build
cmake --install build
```

or in a single command:

```
cmake --preset default && cmake --build build && cmake --install build
```

The install prefix defaults to the MSYS2 environment of the compiler (e.g. `C:/msys64/ucrt64`).

#### Build the documentation

Run this command to build the documentation, then open `docs/html/index.html` in a browser:

```
cmake --build build --target doc
```

</details>

## Sound

Sound is currently disabled, so audio patterns are silent. It relied on the
[audiere](http://audiere.sourceforge.net) library, which is no longer maintained and hard to install on current
systems. The sound code is kept and will be re-enabled once it is ported to a maintained audio library.

## Examples

The [examples](examples) folder contains example programs using the library. Each one is built
separately, against the installed libraries; see [examples/README.md](examples/README.md).

## License

The library is released under the [MIT License](LICENSE). The code in the [examples](examples) folder is
released into the public domain under the [Unlicense](examples/LICENSE).

## Authors

OpenLatero is maintained by [Vincent Levesque](https://vlevesque.com) and his Haptic User Experience research group at [École de technologie supérieure](https://etsmtl.ca). It was originally developped as part of his PhD thesis at [McGill University](https://mcgill.ca) and prepared for release as as open source project by Jerome Pasquero (<jerome.pasquero@gmail.com>). Please see the git history for a full list of contributors.
