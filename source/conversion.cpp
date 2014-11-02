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