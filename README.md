# ccompass
An overwhelmingly simple, header-only, C library for working with sky polarization images.

#### What does it do?
1. Compute angle (AoLP) and degree (DoLP) of linear polarization of Stokes vector matrices.
2. Compute colour maps of AoLP and DoLP.

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Installation
This library is *header-only*. There are no source files that need to be built. Simply copy `include/ccompass.h` into your project's include directory. This can be done via `wget`.

`$ wget https://github.com/benjaminpotter/ccompass/releases/download/v0.1/ccompass.h`

If you need a simple implementation, the test suite creates executables that perform all of the basic library functions. This will not work for many use cases. The majority of cases will require the user to integrate the library with another program that provides image loading and parsing.

## Usage
Include the header where ever you need like

```
#include "ccompass.h"
```

The idea behind single-header file libraries is that they're easy to distribute and deploy because all the code is contained in a single file. By default, the .h files in here act as their own header files, i.e. they declare the functions contained in the file but don't actually result in any code getting compiled.

So in addition, you should select exactly one C/C++ source file that actually instantiates the code, preferably a file you're not editing frequently. This file should define a specific macro to actually enable the function definitions. For example, to use `ccompass.h`, you should have exactly one C/C++ file that doesn't include `ccompass.h` regularly, but instead does 

```
#define CCOMPASS_IMPLEMENTATION
#include "ccompass.h"
```

This library follows a similar design pattern as the popular [STB library](https://github.com/nothings/stb). This text is copied from [here](https://github.com/nothings/stb?tab=readme-ov-file#how-do-i-use-these-libraries).

The library does not provide support for loading and saving images. In addition, library functions operate on `struct cc_stokes` objects. These must be created by the user's program before any library functions can be used. This is done because there is no standard way to encode Stokes vectors into an image. The user must provide an implementation that works with their image encoding. For an example, see the [test suite](tests/common.h).

An example of a complete program implementing ccompass is provided in the test suite. These tests use STB to load and save images. They operate on the test image found in `tests/images`. The canonical example is printing an AoLP visualisation in the solar principal plane. This is implemented in [this](tests/test_transform_aolp.c) test.

## Contributing

## License

