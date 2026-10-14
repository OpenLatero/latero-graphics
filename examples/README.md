# latero-graphics examples

Example programs using the latero-graphics library. Each example is a standalone CMake project that
links against the **installed** libraries, so install latero and latero-graphics first (see the
[main README](../README.md)), then build an example from its own folder.

- [01-vibrate-engine](01-vibrate-engine): replicates latero's 01-vibrate-loop using a Generator and the
  TactileEngine.
- [02-vibrate-gui](02-vibrate-gui): extends 01-vibrate-engine with a GUI.

The latero library's own example, 01-vibrate-loop, is in
[latero/examples](https://github.com/OpenLatero/latero/tree/master/examples).

## License

Unlike the rest of this repository (MIT), the code in this `examples` folder is released into the public
domain under the [Unlicense](LICENSE), so it can be freely copied into your own projects.
