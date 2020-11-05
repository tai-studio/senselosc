# senselosc
*2020, LFSaw ([interaction](http://tai-studio.org) | [music](http://lfsaw.de))*

An [OSC](http://opensoundcontrol.org/) interface for the [sensel morph](https://sensel.com/pages/the-sensel-morph).
Exposes contact information for up to 16 regions of the sensel morph using the [Sensel API](http://guide.sensel.com/api/).


[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/S6S72858T)

## Requirements

+ a sensel morph
+ system-wide installed [sensel API](https://github.com/sensel/sensel-api) (in `/usr/local`) to compile.
+ installed [boost](https://www.boost.org/)
+ git
+ internet connection (for downloading additional dependency libs ([fmt](https://github.com/fmtlib/fmt)) at configuration time) 

## Usage

cli program in 

```sh
build/apps/senselosc
```

## OSC interface

5 message types

```
/contactAvg index num_contacts x y avg_force avg_dist area x_w y_w total_force avg_wdist
/contact index id state x y force area dist wdist orientation  major_axis  minor_axis
/contactDelta index id delta_x delta_y delta_force delta_area
/contactBB index id min_x min_y max_x max_y
/contactPeak index id peak_x peak_y peak_force
```


### contactAvg

```
/contactAvg index num_contacts x y avg_force avg_dist area x_w y_w total_force avg_wdist
```

Average values for all currently registered contacts. If messages are sent in bundles, this message precedes the messages for the individual contacts.
If no contacts are detected, a message with `num_contacts = 0` is sent once.

```txt
index           [int]   device index (currently always 0)
num_contacts    [int]   number of contacts
x               [float] average x-coordinate over all contacts in [mm]
y               [float] average y-coordinate over all contacts in [mm]
avg_force       [float] average pressure applied [g] 
avg_dist        [float] distance to average position [mm]
area            [int]   covered area [sensels]
w_x             [float] force-weigthed average x-coordinate over all contacts in [mm]
w_y             [float] force-weigthed average y-coordinate over all contacts in [mm]
total_force     [float] sum of pressure applied [g] 
avg_wdist       [float] distance to force-weighted average position [mm]
```

### contact

Basic values for each registered contact.

```
/contact index id state x y force area dist wdist orientation  major_axis  minor_axis
```

```txt
index           [int]   device index (currently always 0)
id              [int]   contact id (0..15)
state           [int]   one of invalid(0), start(1), move(2), end(3) 
x               [float] x-coordinate in [mm]
y               [float] y-coordinate in [mm]
force           [float] sum of pressure applied [g] 
area            [int]   covered area [sensels]
dist            [float] distance to average position [mm]
wdist           [float] distance to weighted average position [mm]
orientation     [float] orientation of bounding elipsis [deg] (0..360)
major_axis      [float] major axis length of bounding elipsis [mm]
minor_axis      [float] minor axis length of bounding elipsis [mm]
```

### contactDelta

Delta-values for each registered contact.

```
/contactDelta index id delta_x delta_y delta_force delta_area
```

```txt
index           [int]   device index (currently always 0)
id              [int]   contact id (0..15)
num_contacts    [int]   number of contacts
delta_x         [float] x displacement [mm]
delta_y         [float] y displacement [mm]
delta_force     [float] change of force [g]
delta_area      [int]   change of covered area [sensels]
```


### contactBB

A bounding box for each registered contact.

```
/contactBB index id min_x min_y max_x max_y
```

```txt
index           [int]   device index (currently always 0)
id              [int]   contact id (0..15)
min_x           [float] upper-left x-coordinate of bounding-box [mm] 
min_y           [float] upper-left y-coordinate of bounding-box [mm] 
max_x           [float] lower-right x-coordinate of bounding-box [mm] 
max_y           [float] lower-right y-coordinate of bounding-box [mm] 
```

### contactPeak

Peak values for each registered contact.

```
/contactPeak index id peak_x peak_y peak_force
```

```txt
index           [int]   device index (currently always 0)
id              [int]   contact id (0..15)
peak_x          [float] x-coordinate of pressure peak [mm]
peak_y          [float] y-coordinate of pressure peak [mm]
peak_force      [float] force at pressur peak [g]
```

## Compilation

The requirements for compilation are

+ CMake >= 3.14
+ A C++17 compatible compiler
+ Git

To configure

```bash
cmake -S . -B build
```
To build

```bash
cmake --build build
```

To test ( `--target` can be written as `-t` in CMake 3.15+)

```bash
cmake --build build --target test
```

To build docs (requires Doxygen, output in `build/docs/html`)

```bash
cmake --build build --target docs
```

To use an IDE, such as Xcode (untested)

```bash
cmake -S . -B xbuild -GXcode
cmake --open xbuild
```

## Thanks

Thanks go to Ross Bencina (@RossBencina) for his awesome [oscpack library](https://github.com/RossBencina/oscpack) (here in an [adapted form](https://github.com/tai-studio/oscpack) with a compatible cmake structure), and Henry Schreiner et. al. for a comprehensive and wonderfully written [guide on how to structure a cmake-based build system](https://cliutils.gitlab.io/modern-cmake/chapters/basics/structure.html).
Also of course to the sensel team for their decision to not only create a powerful [API](http://guide.sensel.com/api/) but also make it open source. 

If you find this useful, you can buy me a [coffee](https://ko-fi.com/lfsaw) if you like, or purchase some of my [albums](http://lfsaw.bandcamp.com).
