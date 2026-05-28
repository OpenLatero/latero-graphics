# latero-graphics

Tactile rendering library for the Latero device. See [OpenLatero.org](http://OpenLatero.org) for more information.

## Installation

### MacOS

Last tested in May 2026 on MacOS Tahoe 26.5 with MacPorts 2.12.5.

#### Install the latero library

Follow [these instructions](https://github.com/OpenLatero/latero) to install the latero library.

#### Install dependencies

Other dependencies can be installed through [MacPorts](https://www.macports.org) with the following command:

```
sudo port install ImageMagick gtkmm3 +quartz glib2 +quartz gsettings-desktop-schemas libxmlxx3 librsvg
```

#### Install the audiere library (optional)

The audiere library adds audio support and is not required for most users. It is old and difficult to install — skip this step unless needed.

There is no port for audiere.

- Download audiere (Unix Source Release 1.9.4): http://audiere.sourceforge.net/download.php
- Add `#include <stdlib.h>` to `audiere-1.9.4/src/utility.h`

```
./configure
make
sudo make install
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

### Cygwin

**Step 1: Install the latero library**

See the INSTALL file in the latero project.

**Step 2: Build and install latero-graphics**

```
cmake .
make
make install
```

If successful, `liblaterographics.a` will be in `/usr/local/lib/` and headers in `/usr/local/include/latero-graphics/`.

Run `latero-demo` to test the library.

## Authors

Vincent Levesque <vleves@cim.mcgill.ca>
