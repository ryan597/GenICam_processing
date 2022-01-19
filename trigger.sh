#!/bin/bash

Date=date +'%Y-%m-%d_%H-%M'
NumImgs=100

mkdir data/$Date

conda activate genicam

python main.py -p $Date -n $NumImgs |& tee logs/logging.txt 
