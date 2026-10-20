# 01-VIBRATE-ENGINE

This example replicates the latero library's example [01-vibrate-loop](https://github.com/OpenLatero/latero/tree/master/examples/01-vibrate-loop) using latero::graphics::Generator and latero::graphics::TactileEngine.

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
build/latero-example-vibrate-engine
```

or in a single command:

```
cmake -B build && cmake --build build && build/latero-example-vibrate-engine
```

## Authors

OpenLatero is maintained by [Vincent Levesque](https://vlevesque.com) and his Haptic User Experience research group at [École de technologie supérieure](https://etsmtl.ca). It was originally developed as part of his PhD thesis at [McGill University](https://mcgill.ca) and prepared for release as an open source project by Jerome Pasquero (<jerome.pasquero@gmail.com>). Please see the git history for a full list of contributors.
