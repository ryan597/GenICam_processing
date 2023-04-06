#!/bin/bash

imageDir=`date +'%Y-%m-%d_%H-%M'`
numImgs=100
width=1024
height=1024
framerate=10
pixel_format=8

#saveDir=/run/media/pi/data/$imageDir/
saveDir0=/run/media/pi/data/0/$imageDir/

cam0="JAI Corporation-U507994"

minute_to_start=$1

mkdir -p $saveDir0

sudo ./build/main $numImgs $saveDir0 $width $height $framerate $minute_to_start $pixel_format "$cam0"

# Process images after completed acquisition
./build/convert $saveDir0 $numImgs
