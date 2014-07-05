whereami
========
whereami is a set of applications that can be used to estimate the coordinates of an agent in a cartesian 2D world, given the succesful recognition of two known QR codes that act as Landmarks. 
It uses the Quirc library to find and decode QR codes and the OpenCV (version 2.1+) library to handle cross platform video input.
We have included a fork of the Quirc library into the source tree to avoid dependency from an external repository and to improve the ease of use of the set of programs itself.

This set of applications has been developed as a final project for the Robotics class at the University of Brescia by Patrizio Bertoni, Giovanni Richini and Michael Maghella.

Installation
------------
To build the Quirc library and the associated programs, type ``make`` in the source tree.
This will create the executables needed to use all the features: ``calibration``, ``whereami`` and ``scorriCamera``

Usage
-----
The application is divided into two components:
- ``calibration``, inherited from Zaghen's fork, used to calibrate different parameters based on the live feed;
- ``whereami``, used to localize an agent with access of a video live feed.

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

Whereami library copyright
-----------------------

Copyright (C) 2010-2012 Patrizio Bertoni <<giskard89@gmail.com>>

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

