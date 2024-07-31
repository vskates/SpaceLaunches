#include "view.h"

void GUI::processInput() {
  bool accelerationUpdated = false;
  //Увеличить ускорение
  if (glfwGetKey(window_, GLFW_KEY_L) == GLFW_PRESS) {
    acceleration += 0.5f;
  }
  //Уменьшить ускорение
  if (glfwGetKey(window_, GLFW_KEY_M) == GLFW_PRESS) {
    acceleration -= 0.5f;
  }
  //Просто ускорить вперед по скорости)
  if (glfwGetKey(window_, GLFW_KEY_K) == GLFW_PRESS) {
    accelerationUpdated = true;
    Vector vec = currentDeviceVelo_;
    double len = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    Vector tempac = (vec / len) * acceleration;
    if (acceleratorMode && missionRunning.load()) {
      std::lock_guard<std::mutex> lock(vectorMutex);
      controller.updateAcel(tempac);
    }
  }
  //по X
  if (glfwGetKey(window_, GLFW_KEY_H) == GLFW_PRESS) {
    accelerationUpdated = true;
    Vector vec = {1.0f, 0.0f, 0.0f};
    double len = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    Vector tempac = (vec)*acceleration;
    if (acceleratorMode && missionRunning.load()) {
      std::lock_guard<std::mutex> lock(vectorMutex);
      controller.updateAcel(tempac);
    }
  }
  //по Y
  if (glfwGetKey(window_, GLFW_KEY_J) == GLFW_PRESS) {
    accelerationUpdated = true;
    Vector vec = {0.0f, 0.0f, 1.0f};
    double len = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    Vector tempac = (vec)*acceleration;
    if (acceleratorMode && missionRunning.load()) {
      std::lock_guard<std::mutex> lock(vectorMutex);
      controller.updateAcel(tempac - currentDeviceCoords_);
    }
  }
  //Наверх
  if (glfwGetKey(window_, GLFW_KEY_U) == GLFW_PRESS) {
    accelerationUpdated = true;
    Vector vec = {0.0f, 1.0f, 0.0f};
    double len = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    Vector tempac = (vec)*acceleration;
    if (acceleratorMode && missionRunning.load()) {
      std::lock_guard<std::mutex> lock(vectorMutex);
      controller.updateAcel(tempac);
    }
  }
  //К центру
  if (glfwGetKey(window_, GLFW_KEY_U) == GLFW_PRESS) {
    accelerationUpdated = true;
    Vector vec = currentDeviceCoords_ * (-1.0);
    double len = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    Vector tempac = (vec)*acceleration;
    if (acceleratorMode && missionRunning.load()) {
      std::lock_guard<std::mutex> lock(vectorMutex);
      controller.updateAcel(tempac);
    }
  }

  if (glfwGetKey(window_, GLFW_KEY_P) == GLFW_PRESS) {
    accelerationUpdated = true;
    Vector velo = currentDeviceVelo_;
    Vector cors = currentDeviceCoords_;
    glm::vec3 velocity = {velo.x, velo.y, velo.z};
    glm::vec3 position = {cors.x, cors.y, cors.z};
    float scl = velo.len2() / (std::sqrt(cors.len2()) * (std::sqrt(cors.len2())) * (-1.0));
    Vector tempac = cors * scl;
    if (acceleratorMode && missionRunning.load()) {
      std::lock_guard<std::mutex> lock(vectorMutex);
      controller.updateAcel(tempac);
    }
  }

  if (!accelerationUpdated) {
    std::lock_guard<std::mutex> lock(vectorMutex);
    controller.updateAcel({0.0, 0.0, 0.0});
  }
}

void GUI::ShowCameraControls(bool& observer) {
  ImGuiIO& io = ImGui::GetIO();
  ImVec2 window_pos = ImVec2(10.0f, io.DisplaySize.y - 30.0f);
  ImVec2 window_pos_pivot = ImVec2(0.0f, 1.0f);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

  ImGui::Begin("Camera Control", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Checkbox("Follow", &observer);
  ImGui::End();
}

void GUI::ShowAcceleratorControls(bool& accelerator, bool& fixDistanceMode) {
  ImGuiIO& io = ImGui::GetIO();
  ImVec2 window_pos = ImVec2(10.0f, io.DisplaySize.y - 30.0f);
  ImVec2 window_pos_pivot = ImVec2(0.0f, 1.0f);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

  ImGui::Begin("Camera Control", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Checkbox("Accelerate", &accelerator);
  if (ImGui::Checkbox("Fix Distance", &fixDistanceMode)) {
    if (fixDistanceMode != previousFixDistance) {
      if (fixDistanceMode) {
        fixclicked = true;
      }
      previousFixDistance = fixDistanceMode;
    }
  }
  ImGui::End();
}

void GUI::ShowMissionData() {
  if (missionRunning.load()) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));

    ImGui::Begin("Mission Data", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    ImGui::Text("Mission Time: %.2f seconds", currentMissionTime_.load());
    ImGui::Text("Coordinates: X=%.2f, Y=%.2f, Z=%.2f", 1e-3 * currentDeviceCoords_.x / missionscale, 1e-3 * currentDeviceCoords_.y / missionscale, 1e-3 * currentDeviceCoords_.z / missionscale);
    ImGui::Text("Velocity: X=%.2f, Y=%.2f, Z=%.2f", 1e-3 * currentDeviceVelo_.x / missionscale, 1e-3 * currentDeviceVelo_.y / missionscale, 1e-3 * currentDeviceVelo_.z / missionscale);
    ImGui::Text("Velocity: %.2f mc^2", acceleration);
    ImGui::End();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
  }
}

void GUI::updateInitCords(const Vector& newDeviceCords) {
  std::lock_guard<std::mutex> lock(vectorMutex);
  initDeviceCords_.x = newDeviceCords.x * missionscale;
  initDeviceCords_.y = newDeviceCords.y * missionscale;
  initDeviceCords_.z = newDeviceCords.z * missionscale;
}

void GUI::displayIterationInfo(const Vector& newDeviceCords, double& missionTime, const Vector& newVelo) {
  std::lock_guard<std::mutex> lock(vectorMutex);
  currentDeviceCoords_.x = newDeviceCords.x * missionscale;
  currentDeviceCoords_.y = newDeviceCords.y * missionscale;
  currentDeviceCoords_.z = newDeviceCords.z * missionscale;
  currentDeviceVelo_.x = newVelo.x;
  currentDeviceVelo_.y = newVelo.y;
  currentDeviceVelo_.z = newVelo.z;
  currentMissionTime_.store(missionTime);
}

Vector GUI::getDeviceCords() {
  std::lock_guard<std::mutex> lock(vectorMutex);
  return currentDeviceCoords_;
}

bool GUI::LoadMissionParamsFromFile(const std::string& filename, InitParams& params) {
  std::ifstream file(filename);
  std::string line, key;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    double value;
    iss >> key;
    if (key == "date") {
      std::getline(iss >> std::ws, params.date);
    } else {
      iss >> value;
      if (key == "ra")
        params.ra = value;
      else if (key == "declination")
        params.declination = value;
      else if (key == "velocity")
        params.v = value;
      else if (key == "longitude")
        params.longitude = value;
      else if (key == "latitude")
        params.latitude = value;
      else if (key == "elevation")
        params.height = value;
    }
  }
  return file.good() || file.eof();
}

void GUI::stopPressed() { controller.stopSimulation(); }

void GUI::ShowMissionWindow() {}

void GUI::ShowSidePanel() {
  const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;

  ImVec2 window_pos = ImVec2(0, 0);
  ImVec2 window_pos_pivot = ImVec2(50.0f, 0);
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

  ImGui::Begin("Mission Data", nullptr, window_flags);

  ImGui::Text("Device Coordinates: X=%.2f, Y=%.2f, Z=%.2f", currentDeviceCoords_.x, currentDeviceCoords_.y, currentDeviceCoords_.z);

  ImGui::End();
}

void GUI::exitMission(Vector tempCords, double tempTime) {
  missionRunning.store(false);
  showMissionWindow_ = true;
}

void GUI::startPressed(const InitParams& params) {}