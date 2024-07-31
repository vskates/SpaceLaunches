#include "utiles.h"

std::ostream& operator<<(std::ostream& os, const Vector& v) {
    os << v.x << " " << v.y << " " << v.z;
    return os;
}

Vector operator-(const Vector& one, const Vector& other) {
	return {one.x - other.x, one.y - other.y, one.z - other.z};
}

Vector::Vector(double x, double y, double z) : x(x), y(y), z(z){

}

Vector::Vector(const std::vector<double> other) {
	if (other.size() != 3) {
		throw std::invalid_argument("Vector must be of length 3");
	}
	x = other[0];
	y = other[1];
	z = other[2];
}

double Vector::len2() const {
	return x * x + y * y + z * z;
}

Vector& Vector::operator+=(const Vector& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vector& Vector::operator-=(const Vector& other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

Vector Vector::operator+(const Vector& other) {
	return {x + other.x, y + other.y, z + other.z};
}

Vector Vector::operator*(double scalar) {
	return {x * scalar, y * scalar, z * scalar};
}
Vector Vector::operator/(double scalar) {
	return {x / scalar, y / scalar, z / scalar};
}

Body::Body() {
}

Body::Body(double mass, std::vector<double> init_cords) : mass(mass), cords(init_cords){};

Body::Body(double mass, std::vector<double> init_cords, std::vector<double> velo) : mass(mass), cords(init_cords), velo(velo){};

void Body::updateCords(double x, double y, double z) {
	cords.x = x;
	cords.y = y;
	cords.z = z;
}

void Body::updateVelo(double x, double y, double z) {
	velo.x = x;
	velo.y = y;
	velo.z = z;
}

Vector Body::Force(const Body& b) {
	Vector ab = b.cords - cords;
	double forceToAValue = Constants::Gamma * mass * b.mass / ab.len2();
	Vector forceToA = ab * forceToAValue / std::sqrt(ab.len2());
	return forceToA;
}

Vector Body::Acceleration(const Body& b) {
	return Force(b) / mass;
}
bool Body::intersects(const Body& other) {
	Vector ab = cords - other.cords;
	return std::sqrt(ab.len2()) < radius + other.radius;
}

System::System() {

}

System::System(Body MainBody, Body device) : MainBody(MainBody), device(device){

}

void System::StartMission() {
  runMission = true;
	adableAccel = {0, 0, 0};
}

void System::iterate() {
    if (device.intersects(MainBody)) {
        runMission = false;
        return;
    }
    Vector accel = device.Acceleration(MainBody) + adableAccel * 1e3;
    accel = tempAccelerationTime > 0 ? accel + tempAcceleration : accel;
    tempAccelerationTime = std::max(0.0, tempAccelerationTime - dt);
    device.cords += device.velo * dt + accel * dt * dt / 2;
    device.velo += accel * dt;
    missionTime += dt;
}

Vector System::getCords() {
    return device.cords;
}

Vector System::getVelo() {
	return device.velo;
}

void System::updateVelo(Vector newVelo) {
    device.velo = newVelo;
}

void System::giveAcceleration(Vector acceleration, double time) {
    tempAccelerationTime = time;
    tempAcceleration = acceleration;
}

std::string secondsToHMS(double s) {
    auto seconds = static_cast<int64_t>(s);
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << secs;
    return oss.str();
}