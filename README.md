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

Copyright (C) 2014 brm: Patrizio Bertoni, Giovanni Richini, Michael Maghella
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    For any further information please contact 
	Patrizio Bertoni at giskard89@gmail.com, 
	Giovanni Richini at richgio2@hotmail.it, 
	Michael Maghella at magoo90@gmail.com

