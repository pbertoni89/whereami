#!/bin/bash

# il numero 1 finale è la mia videocamera USB. opencv indicizza da 0: quindi un laptop con videocamera integrata DOVREBBE (compatibilità linux..)
# almeno la videocamera 0. in ogni caso, se opencv non trova quella cercata, fallisce e termina avvisando su stdout.
#rm -f parameters.yml
../calibration -w 9 -h 6 -o parameters.yml -op -oe 1
