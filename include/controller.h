#ifndef CONTROLL_H
#define CONTROLL_H
#include "model.h"
#include "view.h"
#include <future>

class IView;

class IController {
 public:
  virtual ~IController() {};
  virtual void stopSimulation() = 0;
  virtual void includeUserInput(Vector velo) = 0;
  virtual void processSimulation(const InitParams& init) = 0;
  virtual void speedUpMission(int x) = 0;
  virtual void updateAcel(Vector temp) = 0;
  virtual void Pause() = 0;
  virtual void Resume() = 0;
};

class Controller : public IController {
 public:
  IModel& model;
  IView* view;
  double tempDt = SystemConstants::defaultDt;
  bool gui = false;
  std::atomic<bool> paused = false;
  Vector tempAcceleration = {0, 0, 0};

 public:
  Controller(IModel& m);

  void setView(IView* v);

  void updateAcel(Vector temp) override;

  void processSimulation(const InitParams& init) override;

  void stopSimulation() override;

  void includeUserInput(Vector newV) override;

  void speedUpMission(int x) override;

  void Pause() override;
  void Resume() override;
};

#endif