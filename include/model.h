#ifndef MODEL_H
#define MODEL_H
#include "datacontainers.h"
#include "utiles.h"
#include <atomic>
#include <functional>

class IModel {
 public:
  virtual double updateMissionTime() = 0;

  virtual Vector updateDeviceCords() = 0;

  virtual void iterateMission() = 0;

  virtual bool isSimulationFinished() = 0;

  virtual void stopSimulation() = 0;

  virtual void startSimulation(const InitParams& init) = 0;

  virtual void shiftDevice(Vector newVel) = 0;

  virtual void updatePace(int x) = 0;

  virtual std::vector<double> CalculateInitCords(const InitParams& init) = 0;

  virtual std::vector<double> CalculateInitVelo(const InitParams& init) = 0;

  virtual void updateAcceleration(Vector acel) = 0;

  virtual Vector updateDeviceVelo() = 0;
};

class Model : public IModel {
 public:
  ObjectState state;
  System system;
  Vector newDeviceCords;
  Vector newVelocity;
  double newTime;
  Model() {};
  std::atomic<bool> running = true;

  double updateMissionTime() override;

  Vector updateDeviceCords() override;

  Vector updateDeviceVelo() override;

  void iterateMission() override;

  bool isSimulationFinished() override;

  void stopSimulation() override;

  void startSimulation(const InitParams& init) override;

  void shiftDevice(Vector newVel) override;

  void updatePace(int x) override;

  std::vector<double> CalculateInitCords(const InitParams& init) override;

  std::vector<double> CalculateInitVelo(const InitParams& init) override;

  void updateAcceleration(Vector acel) override;
};

#endif