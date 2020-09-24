# senselosc
*2020, LFSaw ([interaction](http://tai-studio.org) | [music](http://lfsaw.de))*

An [OSC](http://opensoundcontrol.org/) interface for the [sensel morph](https://sensel.com/pages/the-sensel-morph)


Exposes contact information for up to 16 regions of the sensel morph using the [API](http://guide.sensel.com/api/)

## Requirements

+ a sensel morph
+ system-wide installed [sensel API](https://github.com/sensel/sensel-api) (in `/usr/local`) to compile.

## Usage

cli program in 

```sh
build/apps/senselosc
```

## OSC interface

4 message types

```
/morph index id state x y force area orientation  major_axis  minor_axis
/morphDelta index id delta_x delta_y delta_force delta_area
/morphBB index id min_x min_y max_x max_y
/morphPeak index id peak_x peak_y peak_force
```

with 
```txt
index       [int]   device index (currently always 0)
id          [int]   contact id (0..15)

state       [int]   one of invalid(0), start(1), move(2), end(3) 
x           [float] x-coordinate in [mm]
y           [float] y-coordinate in [mm]
force       [float] sum of pressure applied [g] 
area        [int]   covered area [sensels]
orientation [float] orientation of bounding elipsis [deg] (0..360)
major_axis  [float] major axis length of bounding elipsis [mm]
minor_axis  [float] minor_axis length of bounding elipsis [mm]

delta_x     [float] x displacement [mm]
delta_y     [float] y displacement [mm]
delta_force [float] change of force [g]
delta_area  [int]   change of covered area [sensels]

min_x       [float] upper-left x-coordinate of bounding-box [mm] 
min_y       [float] upper-left y-coordinate of bounding-box [mm] 
max_x       [float] lower-right x-coordinate of bounding-box [mm] 
max_y       [float] lower-right y-coordinate of bounding-box [mm] 

peak_x      [float] x-coordinate of pressure peak [mm]
peak_y      [float] y-coordinate of pressure peak [mm]
peak_force  [float] force at pressur peak [g]
```

## Compilation

The requirements are:

* CMake >= 3.14.
* A C++17 compatible compiler
* Git
* Doxygen (optional)

To configure:

```bash
cmake -S . -B build
```
To build:

```bash
cmake --build build
```

To test (`--target` can be written as `-t` in CMake 3.15+):

```bash
cmake --build build --target test
```

To build docs (requires Doxygen, output in `build/docs/html`):

```bash
cmake --build build --target docs
```

To use an IDE, such as Xcode:

```bash
cmake -S . -B xbuild -GXcode
cmake --open xbuild
```

## Thanks

Thanks go to Ross Bencina for his awesome [oscpack library](https://github.com/RossBencina/oscpack), and Henry Schreiner et. al. for a comprehensive and wonderful [guide on how to structure a cmake-based build system](https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html).

Also of course to the sensel team for their decision to not only create a powerful [API](http://guide.sensel.com/api/) but also make it open source. 