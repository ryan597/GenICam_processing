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

### Aravis

Follow the installation instruction on the [Aravis project page](https://aravisproject.github.io/aravis/building.html).

### CImg

CImg is used for the saving of the images. It is a simple header only library and can be downloaded [here](https://cimg.eu/). You will need to tell meson where to find the header in the meson.build file.

### C++

To compile this project it is necessary to have a C++ compiler with at least C++11.

The supplied `meson.build` can be configured (ie by setting the correct directory to the CImg download folder) and then build with the following commands provided you have both meson and ninja installed.

```bash
mkdir build
meson build
cd build
ninja
```

Note that the executable is now within the build subdirectory.

After compilation on the Raspberry Pi, it is recommended to enable packet socket support to improve performance. This is done with

```bash
sudo setcap cap_net_raw+ep main
```

---

## 3. Operation

A simple bash script is also provided for scheduling the operation of the cameras for more regular intervals. This is `triggering.sh` and settings can be configured within it and passed as arguments to the `main` binary.
Running the scripts for triggering of the camera is simple using the command line interface where trigger.sh is called with the minute to start as an argument eg `./trigger.sh 01` to start acquisition at one minute past the hour. By providing this minute, we agree on a synchronised time to start the acquisition in the case of multiple cameras on different devices.

```bash
imageDir=`date +'%Y-%m-%d_%H-%M'`
numImgs=100
width=1500
height=1000
framerate=10
saveDir=/run/media/pi/data/$imageDir/

minute_to_start=$1

mkdir -p $saveDir
mkdir -p ../logs

./build/aravis $numImgs $saveDir $width $height $framerate $minute_to_start 2>&1 | tee ../logs/$imageDir.txt
```

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
