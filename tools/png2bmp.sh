#!/bin/bash

INIMG=${1:?}
OUTIMG=${2:?}
PALIMG=${3:-$INIMG}

convert $INIMG -type palette -remap BMP3:$PALIMG -depth 4 -colors 16 BMP3:$OUTIMG
