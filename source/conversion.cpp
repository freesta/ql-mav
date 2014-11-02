// Copyright (C) 2014 freesta
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include "conversion.h"

double SensToMcpiSens (double sens, double yaw, double mcpi) {
  return sens * (yaw * mcpi / 2.54);
}

double McpiSensToSens (double mcpi_sens, double yaw, double mcpi) {
  return mcpi_sens / (yaw * mcpi /2.54);
}

double AccelToMcpiAccel (double accel, double yaw, double mcpi) {
  return accel * ( yaw * pow((mcpi/2.54),2) ) / 1000;
}

double McpiAccelToAccel (double mcpi_accel, double yaw, double mcpi) {
  return mcpi_accel / (( yaw * pow((mcpi/2.54),2) ) / 1000);
}

double OffsetToMcpiOffset (double offset, double mcpi) {
  return offset * 1000 * (2.54 / mcpi);
}

double McpiOffsetToOffset (double mcpi_offset, double mcpi) {
  return mcpi_offset / (1000 * (2.54 / mcpi));
}

double SpeedToMcpiSpeed (double speed, double mcpi) {
  return 2.54*speed*1000/mcpi;
}
double McpiSpeedToSpeed (double mcpi_speed, double mcpi) {
  return mcpi_speed*mcpi/(1000*2.54);
}