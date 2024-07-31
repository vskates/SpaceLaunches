#include "model.h"
#include "datacontainers.h"
#include "utiles.h"

#include <vector>
#include <thread>
#include <chrono>

void Model::stopSimulation() {
	running = false;
}

std::vector<double> Model::CalculateInitCords(const InitParams& init) {
    double earthR = Constants::EarthRadius;
    ACords acords = calculateRADeclination(init.date, init.latitude, init.longitude);
    double x = (earthR + init.height) * cos(acords.declination) * cos(acords.ra);
    double y = (earthR + init.height) * cos(acords.declination) * sin(acords.ra);
    double z = (earthR + init.height) * sin(acords.declination);
    std::vector<double> c;
    c.push_back(x);
    c.push_back(y);
    c.push_back(z);
    return c;
}

std::vector<double> Model::CalculateInitVelo(const InitParams& init) {
    double ra_rad = init.ra * M_PI / 180.0;
    double declination_rad = init.declination * M_PI / 180.0;

    double vx = init.v * cos(ra_rad) * cos(declination_rad);
    double vy = init.v * sin(ra_rad) * cos(declination_rad);
    double vz = init.v * sin(declination_rad);
    std::vector<double> v;
    v.push_back(vx);
    v.push_back(vy);
    v.push_back(vz);
    return v;
}

void Model::startSimulation(const InitParams& init) {
    std::vector<double> deviceCords = CalculateInitCords(init);
    std::vector<double> deviceVelo = CalculateInitVelo(init);
    Body Earth(Constants::EarthMass, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0});
    Earth.radius = Constants::EarthRadius;
    Body device(1.0, deviceCords, deviceVelo);
    device.radius = 0;
    System new_syst(Earth, device);
    system = new_syst;
    system.StartMission();
    newDeviceCords = deviceCords;
    newTime = 0;
    running = true;
}

void Model::iterateMission() {
    if (running == true && system.runMission) {
        system.iterate();
        newDeviceCords = system.getCords();
        newTime = system.missionTime;
    } else {
        stopSimulation();
        system.runMission = false;
        system.dt = SystemConstants::defaultDt;
    }
}

void Model::shiftDevice(Vector newVel) {
    if (!isSimulationFinished()) {
        system.updateVelo(newVel);
    }
}

Vector Model::updateDeviceCords() {
    return newDeviceCords;
}

Vector Model::updateDeviceVelo() {
    return system.getVelo();
}

double Model::updateMissionTime() {
    return newTime;
}

bool Model::isSimulationFinished() {
    return !running;
}

void Model::updatePace(int x) {
    system.dt = SystemConstants::defaultDt * static_cast<double>(x);
}

void Model::updateAcceleration(Vector acel) {
  system.adableAccel = acel;
}