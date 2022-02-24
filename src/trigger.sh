#!/bin/bash

imageDir=date +'%Y-%m-%d_%H-%M'
numImgs=100

mkdir ../data/$imageDir

build/aravis $numImages ../data/$imageDir |& tee ../logs/logging.txt
