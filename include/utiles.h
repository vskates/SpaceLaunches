#ifndef UTILES_H
#define UTILES_H
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include "constants.h"

class Vector {
  public:
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
	Vector() {};
	Vector(double x, double y, double z);
	Vector(std::vector<double> other);

	double len2() const;
	Vector& operator+=(const Vector& other);
	Vector& operator-=(const Vector& other);
	Vector operator+(const Vector& other);
	Vector operator*(double scalar);
	Vector operator/(double scalar);
    friend std::ostream& operator<<(std::ostream& os, const Vector& v);
};

Vector operator-(const Vector& one, const Vector& other);

class Body {
 public:
	double mass;
	double radius;
	Vector cords;
	Vector velo;
	Body();
	Body(double mass, std::vector<double> init_cords);
	Body(double mass, std::vector<double> init_cords, std::vector<double> velo);
	void updateCords(double x, double y, double z);
	void updateVelo(double x, double y, double z);
	Vector Force(const Body& b);
	Vector Acceleration(const Body& b);
	bool intersects(const Body& other);
};

class System {
 public:
	Body MainBody;
	Body device;
	double tempAccelerationTime = 0;
	double missionTime = 0;
	Vector tempAcceleration;
	Vector adableAccel;
	double dt = SystemConstants::defaultDt;
	bool runMission = true;
	System();
	System(Body MainBody, Body device);
	void StartMission();
	void iterate();
	Vector getCords();
	Vector getVelo();
	void updateVelo(Vector newVelo);
	void giveAcceleration(Vector acceleration, double time);
};

std::string secondsToHMS(double seconds);

#endif