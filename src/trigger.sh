#!/bin/bash

imageDir=`date +'%Y-%m-%d_%H-%M'`
numImgs=6000
width=2256
height=960
framerate=5

#saveDir=/run/media/pi/data/$imageDir/
saveDir0="data/0/$imageDir/"
saveDir1="data/1/$imageDir/"

cam0="JAI Corporation-U507994"
cam1="JAI Corporation-U507993"

XOFF0=0
YOFF0=700

XOFF1=300
YOFF1=650

minute_to_start=100
pixel_format=8

mkdir -p "$saveDir0"
mkdir -p "$saveDir1"

./build/main $numImgs "$saveDir0" $width $height $framerate $minute_to_start $pixel_format "$cam0" $XOFF0 $YOFF0 &
./build/main $numImgs "$saveDir1" $width $height $framerate $minute_to_start $pixel_format "$cam1" $XOFF1 $YOFF1

# Process images after completed acquisition
#./build/convert "$saveDir0" $numImgs
#./build/convert "$saveDir1" $numImgs
