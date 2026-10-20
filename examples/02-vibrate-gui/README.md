# 02-VIBRATE-GUI

This example extends 01-vibrate-engine by adding a GUI with a custom widget for the VibrationGenerator and a latero::graphics::TactileDisplayView that shows the movement of the Latero's pins.

## macOS / Linux / Windows

#### Install OpenLatero libraries

Install the [latero](https://github.com/OpenLatero/latero) and [latero-graphics](https://github.com/OpenLatero/latero-graphics) libraries.

#### Build and run the app

The app can be compiled with the following commands:

```
cmake -B build
cmake --build build
```

and run with this command:

```
build/latero-example-vibrate-gui
```

or in a single command:

```
cmake -B build && cmake --build build && build/latero-example-vibrate-gui
```

## Authors

OpenLatero is maintained by [Vincent Levesque](https://vlevesque.com) and his Haptic User Experience research group at [École de technologie supérieure](https://etsmtl.ca). It was originally developed as part of his PhD thesis at [McGill University](https://mcgill.ca) and prepared for release as an open source project by Jerome Pasquero (<jerome.pasquero@gmail.com>). Please see the git history for a full list of contributors.
