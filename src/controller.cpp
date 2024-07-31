#include "controller.h"
#include "constants.h"
#include <thread>
#include <fstream>

void Controller::processSimulation(const InitParams& init) {
    stopSimulation();
    model.startSimulation(init);
    Vector newDeviceCords(model.CalculateInitCords(init));
    Vector newVelo(model.CalculateInitVelo(init));
    view->updateInitCords(newDeviceCords);
    double missionTime = 0;
    paused.store(false);
    while (!model.isSimulationFinished()) {
      if (paused) {
        continue;
      }
      model.updateAcceleration(tempAcceleration);
      model.iterateMission();
      newDeviceCords = model.updateDeviceCords();
      newVelo = model.updateDeviceVelo();
      missionTime = model.updateMissionTime();
      view->displayIterationInfo(newDeviceCords, missionTime, newVelo);
      std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(tempDt * 1000)));
    }
    paused.store(false);
    view->exitMission(newDeviceCords, missionTime);
    stopSimulation();
    tempDt = SystemConstants::defaultDt;
    model.updateAcceleration({0, 0, 0});
    tempAcceleration = {0.0, 0.0, 0.0};
}

void Controller::Resume() {
  paused.store(false);
}

void Controller::Pause() {
  paused.store(true);
}

Controller::Controller(IModel& m) : model(m), view(nullptr) {
}

void Controller::setView(IView* v) {
  view = v;
}

void Controller::stopSimulation() {
  model.stopSimulation();
}

void Controller::includeUserInput(Vector newV) {
  model.shiftDevice(newV * (tempDt * 1000));
}

void Controller::speedUpMission(int x) {
    tempDt *= static_cast<double>(x);
    model.updatePace(x);
}

void Controller::updateAcel(Vector temp) {
  tempAcceleration = temp;
}