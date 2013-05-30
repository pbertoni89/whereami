#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/time.h>

#define MAXLENGTH 256 // the maximum length of the payload we will send
#define PORT "9930"

#pragma pack(1)
typedef struct sqrinfos{
  int message_length;
  int payload_truncated;
  char qr_message[MAXLENGTH+1];
  int x0,y0,x1,y1,x2,y2,x3,y3; // the coordinates of the four QR code points
  double distance;
  double perspective_rotation; // inclinazione rispetto all'osservatore (profondita')
  double vertical_rotation; // inclinazione verticale
  struct timeval timestamp_recognition;
  struct timeval timestamp_current;
} QRInfos;
#pragma pack(0)

#endif