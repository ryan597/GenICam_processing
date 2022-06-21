#!/bin/bash

imageDir=`date +'%Y-%m-%d_%H-%M'`
numImgs=100
width=1500
height=1000
framerate=10
saveDir=/run/media/pi/data/$imageDir/
minute_to_start=$1

mkdir -p $saveDir

./build/main $numImgs $saveDir $width $height $framerate $minute_to_start 2>&1 | tee ../logs/$imageDir.txt
