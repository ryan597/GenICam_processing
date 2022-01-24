#!/bin/bash

imageDir=date +'%Y-%m-%d_%H-%M'
numImgs=100
acquisitionMode=Continuous  # SingleFrame
pixelFormat=RGBx888Packed  # Mono8

mkdir data/$imageDir

build/main $numImages $imageDir $acquisitionMode $pixelFormat |& tee logs/logging.txt