# latero-graphics

Tactile rendering library for the Latero device.

The Latero device is distributed by the not-for-profit organization Tactile Labs Inc. (http://tactilelabs.com).
It was previously named STReSS and was originally designed at the McGill Haptics Laboratory (http://www.cim.mcgill.ca/~haptic/).
Videos of the device in action, as well as a list of related publications, can be found here: http://www.cim.mcgill.ca/~haptic/laterotactile/dev/stress/

Please refer to Vincent Levesque's 2009 Ph.D. thesis "Virtual Display of Tactile Graphics and Braille by Lateral Skin Deformation" for an in-depth report on the design, implementation and experimental evaluation of multiple tactile renders developed specifically for the STReSS device: http://vlevesque.com/go/phd.html

This software is part of a set of libraries and programs, all under the terms of the GNU General Public License or the GNU Lesser General Public License. A partial list of related libraries and programs:

- **latero**: the low level driver for the Latero device
- **latero-gui**: a gui that allows the individual testing of the actuators
- **latero-graphics**: a library for tactile rendering
- **latero-examples**: a set of example programs using the latero or latero-graphics libraries
- **latero-demo**: a program that makes use of the latero-graphics library and showcases the Latero device's functionality
- **latero-graphics-demo**: a program that makes use of the latero-graphics library and showcases the Latero device's functionality when mounted on a 2D slider (Tactograph)
- **latero-graphics-studio**: a program that allows editing of 2D tactile graphics rendered using the latero-graphics library

See [OpenLatero.org](http://OpenLatero.org) for more information.

## Installation

The latero-graphics library links to the latero library, which must be installed first.

### Mac OS X

**Step 1: Install the latero library**

See the INSTALL file in the latero project.

**Step 2: Install dependencies**

```
sudo port install ImageMagick gtkmm3 +quartz glib2 +quartz gsettings-desktop-schemas libxmlxx3 librsvg
```

**Step 3: Install the audiere library (optional)**

The audiere library adds audio support and is not required for most users. It is old and difficult to install — skip this step unless needed.

There is no port for audiere.

- Download audiere (Unix Source Release 1.9.4): http://audiere.sourceforge.net/download.php
- Add `#include <stdlib.h>` to `audiere-1.9.4/src/utility.h`

```
./configure
make
sudo make install
```

**Step 4: Build and install latero-graphics**

```
cmake --preset default
cmake --build build
sudo cmake --install build
```

Or all together:

```
cmake --preset default && cmake --build build && sudo cmake --install build
```

Run `latero-demo` to test the library.

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
