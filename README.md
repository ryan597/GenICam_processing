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

***SWITCHING TO C++ IMPLEMENTATION***

Creating a virtual environment to install the Python dependencies packages is recommended, this can be done with either conda or the python3 venv.

### Using [`conda`](https://docs.conda.io/en/latest/)

```bash
conda env create -f environment.yml
conda activate newproject
```

#### exporting dependencies to yml file

```bash
conda env export > environment.yml
```

### Using [`pip`](https://pypi.org/project/pip/)

```bash
python3 -m venv newproject
source newproject/bin/activate
pip install -r requirements.txt
```

#### exporting dependencies to a txt file

```bash
pip freeze >> requirements.txt
```

<details>
    <summary>Dependencies list</summary>

- opencv
- numpy

</details>

---

### mvIMPACT

This software was written using the mvIMPACT_Acquire Python SDK and its supplied GenTL.
Install the mvIMPACT software for your device using the downloads page [here](http://static.matrix-vision.com/mvIMPACT_Acquire/2.45.0/).

---

### C++

The project is switching to a c++ source code for image acquisition. This is still using the mvIMPACT SDK for which the header files were also installed along with the Python SDK. Ensure that the mvIMPACT directory is included in your path.

```bash
export PATH=$PATH:/opt/mvIMPACT_Acquire/
```

The supplied makefile can be used for compilation of the source files and results in an executable `main` file. Call the executable within the `trigger.sh` script with required arguments. Note: The linked library for mvDeviceManager is written for the ARM64 CPU architecture of the Raspberry Pi, if using x86, change "arm64" to "x86_64".

---

## 3. Operation

Running the scripts for triggering of the camera is simple using the command line interface.

```bash
conda activate genicam
python main.py
```

See the help function for the arguments that can be passed to the program such as device numbers, max number of images to acquire, time of operation etc.

A simple bash script is also provided for scheduling the operation of the cameras for more regular intervals. This is `triggering.sh` and can be configured using the JSON file `camera_settings.json` in the config folder. Other parameters of the camera can also be changed through this JSON file. ***(NOT YET IMPLEMENTED)***

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

