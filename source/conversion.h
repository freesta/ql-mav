#pragma once
#include "Math.h";

double SensToMcpiSens (double sens, double yaw, double mcpi);
double McpiSensToSens (double mcpi_sens, double yaw, double mcpi);

double AccelToMcpiAccel (double accel, double yaw, double mcpi);
double McpiAccelToAccel (double mcpi_accel, double yaw, double mcpi);

double OffsetToMcpiOffset (double offset, double mcpi);
double McpiOffsetToOffset (double mcpi_offset, double mcpi);

double SpeedToMcpiSpeed (double speed, double mcpi);
double McpiSpeedToSpeed (double mcpi_speed, double mcpi);