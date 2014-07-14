/*
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
*/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/time.h>

#define MAXLENGTH 256 // the maximum length, in usable chars, of the qr_message we will send
#define PORT "9930"

#pragma pack(1)
typedef struct sqrinfos{
  int message_length;
  int payload_truncated;
  char qr_message[MAXLENGTH+1];
  int x0,y0,x1,y1,x2,y2,x3,y3;  // the coordinates of the four QR code points
  int distance;
  int perspective_rotation;     // perspective rotation (depth)
  int vertical_rotation;        // rotation of the vertical axis of the QR code
  struct timeval timestamp_recognition;
  struct timeval timestamp_current;
} QRInfos;
#pragma pack(0)

#endif
