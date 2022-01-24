# GenICam Image Processing

Repository to interface with a GenICam complient camera.

---

## Contents

[GenICam Image Processing](#genicam-image-processing)

- [1. Equipment](#1-equipment)
- [2. Installation](#2-installation)
- [3. Operation](#3-operation)

---

## 1. Equipment

This software was developed for (and tested with) the JAI GO 5000c-PGE camera. It is connected to the camera using a TP-Link PoE switch with Gigabit ehternet ports. It was also tested on a linux machine and Raspberry Pi capable of the GigE link speed. No warrenty or guarantee of the software working is given.

---

## 2. Installation

### mvIMPACT

This software was written using the mvIMPACT_Acquire C++ SDK and its supplied GenTL.
Install the mvIMPACT software for your device using the downloads page [here](http://static.matrix-vision.com/mvIMPACT_Acquire/2.45.0/) and download for your CPU architecture along with the install script.

---

### C++

To compile this project it is necessary to have a C++ compiler with at least C++11.

The supplied `CMakeLists` can be used for compilation of the source files and results in an executable `main` file. Call the executable within the `trigger.sh` script with required arguments.

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Note that the executable is now within the build subdirectory.

---

## 3. Operation

Running the scripts for triggering of the camera is simple using the command line interface.

```bash
imageDirectory="data/test"
numImgs=1000
acquisitionMode="Continuous"  # or "SingleFrame"
pixelFormat="RGBx888Packed"  # or "Mono8"
./main $numImages $imageDirectory $acquisitionMode $pixelFormat
```

A simple bash script is also provided for scheduling the operation of the cameras for more regular intervals. This is `triggering.sh` and settings can be configured within it and passed as arguments to the `main` binary. Other parameters of the camera can also be changed through this JSON file. ***(NOT YET IMPLEMENTED)***

## [License](LICENSE)

<details>
<summary>MIT License</summary>

Copyright (c) 2021 Ryan Smith

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
</details>

---
