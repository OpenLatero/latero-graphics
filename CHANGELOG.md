# Changelog

## 2010-04-06 — Vincent Levesque

- **mutex:** Cleaned up and moved to root (no longer part of xenomai++).
- **laterophics.h:** Removed `Init()` and `Shutdown()`. These functions were only used to call `srand()`, which should not be messed with in a library. The function `rand()` is used in laterophics but only for graphics so its seed is not critical.
- **laterophics.h:** Cleaned up global mutex, moved it to `mutex.h`. Removed `laterophics.h/cpp`. Moved mainpage doc to `mainpage.h`.

## 2010-04-05 — Vincent Levesque

- Added `make doc` target which compiles the doxygen docs.
- **mutex:** Replaced `Guard` by `boost::scoped_lock`. Mutex is now compatible with Boost's templates.

## 2010-03-17 — Vincent Levesque

- Switched from autotools to CMake.

## 2010-03-01 — Vincent Levesque

- Minor cleanup, mostly in generators.

## 2010-02-28 — Vincent Levesque

- **generator:** Started phasing out `RangeImg`. Clean up.
- **doxygen.cfg:** Added basic support for doxygen. To use it, install packages `doxygen`, `doxygen-gui`, and `graphviz`. Then run `doxygen doxygen.cfg`. The output will be in `docs/html/index.html`.

## 2010-02-26 — Vincent Levesque

- **planar:** Moved all files to laterophics root.
- **testing:** Removed most test applications.
- **generatorwidget, positiongenwidget:** Removed.
- **generatorhandle:** Removed. Copied temporarily to demo.
- **togglegen:** Removed. Use Toggle pattern instead.
- **generator:** Removed support for stored illustrations.

## 2010-02-24 — Vincent Levesque

- **xeno.task:** Modified so that a Task can be stopped even if not started without hanging.

## 2010-02-21 — Vincent Levesque

- **graphics.toggle:** Added pattern to replace `ToggleGen`. Refreshing visuals may be an issue since Patterns do not support caching illustrations as Generators do.

## 2010-02-16 — Vincent Levesque

- **planar.imageeditordialog:** Removed.
- **planar.generator:** Removed obsolete "Vector" type. Only "Canvas" is now recognized.
- **planar.combogen:** Removed. Should be replaced by Canvas.
- **planar.audiogen:** Removed, replaced by `AudioPattern`.
- **graphics.audiopattern:** Created to replace `AudioGen`. Fixed a bug that skipped first play of each sound.

## 2010-02-10 — Vincent Levesque

- **planar.dotgen:** Removed. The Dots pattern has similar functionality, with the exception of support for the specification of dot position by bitmap.

## 2010-02-08 — Vincent Levesque

- **planar.audiogenwidget:** Removed.
- **filter.qilowpass, delay:** Removed `QiLowPass` and `Delay`. Moved `Average` to laterophics. Removed filter.
- **xeno.timingmonitor:** Removed.

## 2010-02-07 — Vincent Levesque

- **planar.virtualsurfacewidget:** Removed, merged with `laterophics.virtualsurfacewidget`.
- **planar:** Removed obsolete planar namespace.
- **laterophics.generator:** Removed, merged with `planar.generator`.

## 2010-02-05 — Vincent Levesque

- **planar.device:** Removed `latero::graphics::planar::Device`, replaced with `latero::Tactograph`.
- **generatorhandle:** Removed.
- **generator, state:** Merged `latero::graphics::State` and `latero::graphics::planar::State`. Started cleaning `latero::graphics::Generator` for merge with `latero::graphics::planar::Generator`.

## 2009-12-30 — Vincent Levesque

- **graphics.group:** Added a new compositing operation named "reactive" that blends between the first two patterns according to the exploration behavior (direction and speed of exploration velocity). This effectively replaces the stand-alone "blend" pattern and generalizes the effect of `BlendTexture` to generic patterns. The format of the `operator` tag in XML files was modified to accommodate sub-parameters (Modulator information).
- **graphics.blend:** Removed, replaced by new compositing operator in Group.

## 2009-12-29 — Vincent Levesque

- **graphics.blend:** Added a Blend pattern that can alternate between two patterns based on exploration velocity, exactly like `BlendTexture`. The pattern is fully implemented but the GUI lacks support for replacing the two underlying patterns. This pattern should most likely be integrated as an additional compositing operator in Group.

## 2009-12-27 — Vincent Levesque

- **gtk:** Removed `ManagedNotebook`, `Notebook`, `Scope`, `ScopeDisplay`, `ScopeTrace`.
- **linear:** Removed the linear component of the library. This is a first step towards refocusing the library on tactile graphics and eliminating components that have become non-essential and difficult to maintain without appropriate hardware.
- **fixed:** Also removed the fixed component of the library, leaving only the 2D component.

## 2009-01-01 — Vincent Levesque

- Many changes were made from January to August 2009 but they were not documented here.

## 2008-11-28 — Vincent Levesque

- **planar.texture:** The amplitude of the vibration is now programmable.

## 2008-11-26 — Vincent Levesque

- **planar.blendtexture:** The `BlendTexture` blends two textures according to a "modulator" that specifies an alpha value as a function of sensor readings or other variables. A new sublibrary was created for modulators under the planar directory. There is currently only one that can toggle between the textures as a function of the direction of exploration.

## 2008-11-24 — Vincent Levesque

- **filter.average:** Modified to use an array instead of a queue. This eliminates dynamic memory allocation in the realtime loop (insert, get).
- **planar.velocityestimator:** Added a crude velocity estimator (based on Braille work). Velocity information is now gathered in the tactile engine and propagated to generators, positiongens and textures.

## 2008-11-23 — Vincent Levesque

- **planar.texture:** Added previews to individual textures.
- **planar.combotexture:** Added support for alpha blending. The user correctly sets the list of textures (a, b, k such that result = k\*a + (1-k)\*b). The tabs are updated to reflect the role of each texture in the operation.

## 2008-11-19 — Vincent Levesque

- **planar, linear:** The linear and planar namespaces now have a typedef that maps a "Device" to the latero devices, allowing use of "Device" instead of `latero::TactileDisplay1dof`.
- **planar.texture:** Textures are now aware of the device. This enables use of the workspace size for drawing and GUI controls (e.g. setting the seed point as "center").
- **planar.texture:** All textures now have the ability to vibrate, without using the grating's vibration.
- **planar.oscillator:** Optimized to avoid recomputing the same values 64 times per cycle.

## 2008-11-18 — Vincent Levesque

- Cleaned up `xml.h`. Two objects (`XMLOutputNode` and `XMLInputNode`) are now used as convenience classes to simplify reading and writing XML files. The result is cleaner code when operating with XML and better support for relative path names.

## 2008-11-17 — Vincent Levesque

- **planar.grating:** Gratings can now be inverted.
- **planar.texture:** Textures can now be inverted. Useful for operations like multiplication of textures. Illustrations are not currently drawn correctly.

## 2008-11-13 — Vincent Levesque

- **VirtualSurfaceArea:** Added a context menu to save the image shown.

## 2008-11-12 — Vincent Levesque

- **planar.texture:** Every texture now has an amplitude.
- **planar.ComboTexture:** `ComboGratingTexture` was renamed `ComboTexture`. Now supports three operations: `op_multiply`, `op_add`, `op_max`.
- **planar.texture:** Textures now have a default icon which can be overridden in XML files.

## 2008-11-10 — Vincent Levesque

- **planar.Grating:** Grating can now be made to vibrate. This makes it possible to turn a grating into a modulated vibration at the click of a button.
- **planar.AxialGratingTexture:** The grating now saturates in the middle to prevent very high spatial frequencies.

## 2008-11-07 — Vincent Levesque

- **gtk:** `ScaleEntryFrame` and friends were replaced by `HNumWidget` and `VNumWidget`. This numeric widget supports all previous modes and also allows alternating between different units.
- **planar.GratingTexture:** Velocity can now be specified either in units/s or Hz.

## 2008-11-04 — Vincent Levesque

- **planar.texture:** Textures are now selected from a set of predefined XML files.

## 2008-11-02 — Vincent Levesque

- **all:** Files (xml, images, etc.) are now saved with `0666` permissions so that non-root users can work on them.
- **planar.generator:** The `Generator` class now has the functionalities of `GeneratorFactory`. The XML file format for generators was modified to match that of the new XML texture files.

## 2008-09-22 — Vincent Levesque

- **planar.vectorgen:** Added moving vibration bands to Circle and Line.

## 2008-09-20 — Vincent Levesque

- **planar.VibroGen:** `VibroGen` was removed. The same functionality can now be obtained from a `TextureGen` with a `VibroTexture`.

## 2008-09-18 — Vincent Levesque

- **planar.WaveGen:** `WaveGen` was renamed `TextureGen`.
- **planar.VibroGen:** Vibration is now just another texture. Integration is not complete yet.

## 2008-09-17 — Vincent Levesque

- **planar.VibroGen:** `VibroGen` is now a `PositionGen`.
- **planar:** Planar generators now use `BiasedImg` rather than `RangeImg`. This changes the meaning of a deflection from -1=left, +1=right to 0=right, 1=left, greatly simplifying most operations.

## 2008-09-16 — Vincent Levesque

- **planar:** Removed any trace of `RestDeflection`. This is now assumed to be 1.0 (right).

## 2008-09-11 — Vincent Levesque

- **planar.wavegen:** Changed the signification of the orientation parameter. Orientation now indicates the direction perpendicular to the ridges and the direction in which the grating should be rendered. The coordinate system was also modified to match that of the vector graphics and seed point specification (Y increases down), consistent with cairo and svg defaults.

## 2008-09-10 — Vincent Levesque

- **planar.wavegen:** Removed support for rest position 0; only rest position 1 remains. This simplifies the code and aligns with the behavior of dots.

## 2008-09-08 — Vincent Levesque

- **planar.texture:** Gratings were refactored. The waveform of gratings is now modifiable — the sinusoid can be skewed and gaps can be added on the sides.
- **gtk.plot:** Corrected orientation (positive y is now up).

## 2008-08-05 — Vincent Levesque

- **planar.wavegen:** The Grating texture generator is now part of a separate class.

## 2008-08-04 — Vincent Levesque

- **planar.dotgen:** Dots are now vectorial objects in `VectorGen`. The old `DotGen` now derives from `VectorGen` and uses it for rendering, handling the conversion from image-based dots to their vectorial representation.

## 2008-07-30 — Vincent Levesque

- **planar.vectorgen:** Added line object.

## 2008-07-29 — Vincent Levesque

- **planar.wavegen:** Fixed visual rendering of axial texture (slow, but working).
- **planar.vectorgen:** Now supporting multiple vector objects.

## 2008-07-28 — Vincent Levesque

- **planar.dotarraygen:** Removed (superseded by dotgen).
- **planar.vector:** Moved `vectorgen` and associated files to the `vector` subdirectory. Extracted a `circle` class from it. Multiple circles can now be shown on the screen.

## 2008-07-18 — Vincent Levesque

- **planar.generator:** Added ability to store and retrieve a precomputed illustration (used in schoolbook).
- **planar.generator:** Added virtual method to handle keypressed (used in `ToggleGen`, schoolbook).
- **planar.togglegen:** New generator that toggles between two generators (can be loaded with xml).

## 2008-06-25 — Vincent Levesque

- **laterophics/virtualsurfacewidget.cpp:** Modified constructor to disable animations. Currently necessary to reduce system load and allow decent interaction with the user interface at update rates in the order of 1 kHz.

## 2008-05-14 — Vincent Levesque

- Now using Audiere for sound instead of libgnome. Works with Ubuntu Hardy Heron.

## 2008-04-22 — Vincent Levesque

- Created a "fixed" sublibrary for generators that don't rely on position.

## 2008-04-13 — Vincent Levesque

- Added typedefs for `shared_ptr`s of generators (e.g. `GeneratorPtr`) to simplify code.
- The `GeneratorSet` code was cleaned and partially moved to the GrandIllusion program.
- Adapted linear generators to use smart pointers.

## 2008-04-12 — Vincent Levesque

- Generators now create their own widgets. This eliminates the need for the `WidgetFactory` and allows generators defined outside of the library to be used by library code that needs to create appropriate widgets.

## 2008-04-11 — Vincent Levesque

- The code in `linear/elements` is now in `pattern/`. This better conveys the idea that these elements are used in both 1D and 2D contexts.

## 2008-04-07 — Vincent Levesque

- **planar.WaveGen:** Added radial gratings and superposition of two gratings (needs cleanup).

## 2008-04-04 — Vincent Levesque

- **planar.WaveGen:** Now supports radial gratings.

## 2008-04-01 — Vincent Levesque

- **planar.WaveGen:** Now time-varying with a wave velocity in mm/s.
- **planar.VibroGen:** Added a stroke parameter that controls the amplitude of the stroke (e.g. 50% or 100%).

## 2008-03-19 — Vincent Levesque

- **planar:** `WaveGen` now generates illustrations as fast for diagonals as other orientations, using only Cairomm. `VibGen` is the only remaining generator that doesn't use Cairomm.

## 2008-02-28 — Vincent Levesque

- **planar:** Masks can now be loaded from a `Gdk::pixbuf` instead of a file. This makes it possible to create masks on the fly using cairomm.

## 2008-02-25 — Vincent Levesque

- **planar:** A new mechanism was implemented to get information about a generator's rest deflection. It is assumed that only renderings with identical rest deflection can be superposed.
- **planar.WaveGen, planar.VibGen:** `WaveGen` and `VibGen` can now be configured to place actuators in the middle or on the right when at rest. This allows them to be combined with `DotGen`. An XML option can switch to "rest on right" mode.
- **planar.ComboGen:** `DotGen` can now be combined with `VibGen` or `WaveGen` provided they are set to "rest on right" mode.

## 2008-02-11 — Vincent Levesque

- **laterophics:** The library now accommodates most features of the new 2D planar carrier (Puck). The main modification is support for rotation of the tactile display, for both tactile rendering and visualization.

## 2007-07-31 — Vincent Levesque

- **planar:** Now using `boost::shared_ptr` to deal with all Generators.

## 2007-07-17 — Vincent Levesque

- **laterophics:** The library now has a global lock. Individual object locks were removed. See `laterophics.h` for convenience macros.

## 2007-07-13 — Vincent Levesque

- **planar:** Began minimizing use of locks within `Render` calls.

## 2007-06-27 — Vincent Levesque

- **Generator:** `CreateFrame(...)` is now called `Render(...)`.
- **planar.FreqGen:** Removed — modulating by frequency doesn't seem to work.
- **Generator:** No longer requires `UpdateRateHz`. Computations using time can now be done more cleanly using `State.time`.

## 2007-06-14 — Vincent Levesque

- **planar.DotArrayGen, planar.WaveGen:** Removed support for units relative to TD spacing. All XML files were modified accordingly.

## 2007-06-08 — Vincent Levesque

- **planar.DotArrayGen:** Removed dependency on mask. All XML files were modified accordingly.

## 2007-05-04 — Vincent Levesque

- **planar.PositionGen:** `GetDeflection` is now called while locked. This dramatically reduces the time taken to create a frame.

## 2007-04-25 — Vincent Levesque

- **planar:** Removed `MaskedGen` (generators now use masks directly).
- **planar:** Added `VirtualSurfaceWidget` that updates itself (needs more work).
- **planar.DotArrayGen:** Can now optionally be rendered like braille dots (needs more work & evaluation).

## 2007-04-24 — Vincent Levesque

- **planar.wavegen:** Both horizontal and vertical gratings are now drawn; other orientations are drawn but very inefficiently.

## 2007-04-23 — Vincent Levesque

- **planar.wavegen:** Halved the pitch for the illustration — seems closer to perception.
- **planar.visualizewidget:** Added widget that displays a low-level representation of a `PositionGen`'s map.
- **planar.dotarraygen:** Now supports visualization.

## 2007-04-19 — Vincent Levesque

- **planar.wavegen:** Added partial support for different orientations.

## 2007-04-18 — Vincent Levesque

- **planar.positiongen:** Added a visualization feature to see the exact mapping (incomplete).
- **linear.DotGen:** Removed "enable" property.
- **linear.PositionGenWidget:** `PositionGraph` can now be removed by calling `ShowPositionGraph(false)`.
- **linear.DotGenWidget:** Adjustments are now using the virtual surface width.

## 2007-04-16 — Vincent Levesque

- Moved `planar.Manager` to studio.
- Added `laterophics.Init()` and `laterophics.Shutdown()` functions so that similar functions of libgnome can be called only once.

## 2007-04-15 — Vincent Levesque

- Integrated xenomai++ into `laterophics/xenomai++`.

## 2007-04-12 — Vincent Levesque

- Added minimal support for sound tags on 2D masks. Sound is generated with gnome-sound.
- Added basic support for zooming in 2D tactile images. It is now possible to increase the scale such that the mask extends beyond the virtual surface.

## 2007-04-09 — Vincent Levesque

- Integrated libgtkvl into `laterophics/gtk`.
- Integrated libfilter into `laterophics/filter`.
- Renamed namespace `plannar` to `planar` (typo). Renamed directories `1dof` and `2dof` to `linear` and `planar` to match namespaces.

## 2007-03-08 — Vincent Levesque

- Minor cleanups in `BrailleGenWidget`. The standard `PositionGenWidget` modifiers (shift, scale, etc.) are no longer shown by default.

## 2007-03-05 — Vincent Levesque

- `DotArrayGen` now uses cairomm to draw its illustration. The code is much cleaner, runs faster, and paves the way to realtime rendering with cairo instead of going through intermediate pixbufs.

## 2007-02-28 — Vincent Levesque

- **BrailleGen and friends:** Implemented a rough symbolic representation for all the different modes. Also cleaned up the terminology used for those modes, both in the code and in the GUI.
- Split up `latero::graphics::linear::Generator` in two: a `Generator` that handles frame requests based on position and a `PositionGen` specialization that maps piezo positions to their deflection.

## 2007-02-27 — Vincent Levesque

- Replaced all instances of the word "voltage" by "deflection" or "frame".
- Began work on symbolic representation of Braille.
