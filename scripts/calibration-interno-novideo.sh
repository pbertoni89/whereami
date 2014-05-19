#!/bin/bash

# il numero 1 finale è la mia videocamera USB. opencv indicizza da 0: quindi un laptop con videocamera integrata DOVREBBE (compatibilità linux..)
# almeno la videocamera 0. in ogni caso, se opencv non trova quella cercata, fallisce e termina avvisando su stdout.
#rm -f parameters.yml
cd ..
./calibration -w 9 -h 6 -o parameters.yml -op -oe -nf 0
cd scripts
