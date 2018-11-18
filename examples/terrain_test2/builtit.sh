#!/bin/sh

CURDIR=${PWD}
cd ../.. && make clean && make install && cd ${CURDIR} && make clean && make && gens out.bin
