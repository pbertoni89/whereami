qrlocate
========
qrlocate is a set of applications that can be used to estimate the distance and perspective rotation of a QR code from a live video feed. 
It uses the Quirc library to find and decode QR codes and the OpenCV (version 2.1+) library to handle cross platform video input.
We have included a fork of the Quirc library into the source tree to avoid dependency from an external repository and to improve the ease of use of the set of programs itself.

This set of applications has been developed as a final project for the Robotics class at the University of Brescia by Nicola Zaghen and Alex Catania Zampieri.

Installation
------------
To build the Quirc library and the associated programs, type ``make`` in the source tree.
This will create the executables needed to use all the features: ``calibration``, ``inspect`` and ``client``

Usage
-----
The application is divided into three components:
- ``calibration``, used to calibrate different parameters based on the live feed;
- ``inspect``, the executible that has to be run from a computer with a webcam attached (with, as argument, the output file of the calibration phase)
This program starts a server listening on port 9930 that comunicates with clients all the informations about the last QR code that has been found in the live feed.
- ``client``, the client needs the IP address of the ``inspect`` server and simply prints all the informations that the server sends to it and then terminates.

Quirc library copyright
-----------------------

Copyright (C) 2010-2012 Daniel Beer <<dlbeer@gmail.com>>

Permission to use, copy, modify, and/or distribute this software for
any purpose with or without fee is hereby granted, provided that the
above copyright notice and this permission notice appear in all
copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
