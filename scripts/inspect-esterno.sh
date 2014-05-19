#!/bin/bash

# il numero 1 finale è la mia videocamera USB. opencv indicizza da 0: quindi un laptop con videocamera integrata DOVREBBE (compatibilità linux..)
# almeno la videocamera 0. in ogni caso, se opencv non trova quella cercata, fallisce e termina avvisando su stdout.
cd ..
./inspect ./out_camera_data.yml -cf 1
cd scripts
