# filetool

A tool for split/join file.

## History

- v1.0.0 @ 2018.04.26 - First release

## Platforms

- Windows
- Linux
- macOS

## Building

### Dependencies

- cmake

### Compiling

- make 64-bit version
~~~
mkdir build
cd build
cmake ..
make
~~~

- make 32-bit version
~~~
mkdir build
cd build
cmake -DBUILD64=OFF ..
make
~~~

### Installing

~~~
make install
~~~

## Usage

~~~
filetool [option...] [option]...
~~~

## Options

See `filetool --help` messages.
