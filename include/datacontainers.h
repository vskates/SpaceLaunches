#ifndef DATACONTAINERS_H
#define DATACONTAINERS_H
#include <vector>
#include <string>
#include <cmath>

#pragma once
#include <string>
#include <ctime>
#include <cmath>

struct ACords {
    double ra;
    double declination;
};

ACords calculateRADeclination(const std::string& date, double latitude, double longitude);

struct InitParams {
        double latitude, longitude, height;
        std::string date;
        double ra;
        double declination;
        double v;

        InitParams() : latitude(0.0), longitude(0.0), height(0.0), ra(0.0), declination(0.0), v(0.0) {};
};

struct ObjectState {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double vx = 0.0;
  double vy = 0.0;
  double vz = 0.0;
  double Radius();
};

#endif