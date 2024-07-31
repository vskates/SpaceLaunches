#ifndef VIEW_H
#define VIEW_H
#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include "controller.h"
#include "datacontainers.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "utiles.h"

class IController;

class IView {
 public:
  virtual ~IView(){};
  virtual void run() = 0;
  virtual void displayIterationInfo(const Vector& newDeviceCords, double& missionTime, const Vector& newVelo) = 0;
  virtual void exitMission(Vector tempCords, double tempTime) = 0;
  virtual void updateInitCords(const Vector& vec) = 0;
};

class GUI : public IView {
 private:
  std::mutex vectorMutex;
  Vector currentDeviceCoords_;
  Vector currentDeviceVelo_;
  Vector initDeviceCords_;
  bool acceleratorMode = false;
  bool fixDistanceMode = true;
  bool previousFixDistance = false;
  bool fixclicked = false;
  std::atomic<double> currentMissionTime_{0.0};
  std::atomic<bool> missionRunning{false};
  double planetRadius = std::sqrt(16 * 16 + 13 * 13);
  double missionscale = planetRadius / Constants::EarthRadius;

 public:
  GUI(GLFWwindow* window, IController& controller);

  void run() override;
  IController& controller;
  GLFWwindow* window_;
  void displayIterationInfo(const Vector& newDeviceCords, double& missionTime, const Vector& newVelo) override;
  void exitMission(Vector tempCords, double tempTime) override;
  bool showMissionWindow_ = false;
  float acceleration = 1.0f;
  bool LoadMissionParamsFromFile(const std::string& filename, InitParams& params);
  void startPressed(const InitParams& params);
  void stopPressed();
  void ShowMissionWindow();
  void ShowSidePanel();
  void updateInitCords(const Vector& newDeviceCords) override;
  void ShowMissionData();
  void ShowCameraControls(bool&);
  void ShowAcceleratorControls(bool&, bool&);
  Vector getDeviceCords();
  void processInput();
  InitParams missionParams;
};

class Console : public IView {
 private:
  IController& controller;
  bool displayMode = false;

 public:
  Console(IController& controller);

  void run() override;
  bool LoadMissionParamsFromFile(InitParams& params, const std::string& filePath);
  void getParamsSource(InitParams& params);
  void displayIterationInfo(const Vector& newDeviceCords, double& missionTime, const Vector& newVelo) override;
  void exitMission(Vector tempCords, double tempTime) override;
  void LoadMissionParamsFromCin(InitParams& params);
  void updateInitCords(const Vector& newDeviceCords) override;
};

class MakeInterface {
 public:
  static IView* createInterface(const std::string& choice, IController& controller, GLFWwindow* window);
};

#endif