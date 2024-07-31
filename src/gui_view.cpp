#include <filesystem>
namespace fs = std::filesystem;
#include <cmath>

#include "rendmodel.h"
#include "view.h"

const unsigned int width = 1000;
const unsigned int height = 800;

float skyboxVertices[] = {-1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f};

unsigned int skyboxIndices[] = {
    // Right
    1, 2, 6, 6, 5, 1,
    // Left
    0, 4, 7, 7, 3, 0,
    // Top
    4, 5, 6, 6, 7, 4,
    // Bottom
    0, 3, 2, 2, 1, 0,
    // Back
    0, 1, 5, 5, 4, 0,
    // Front
    3, 7, 6, 6, 2, 3};

float randf() { return -1.0f + (rand() / (RAND_MAX / 2.0f)); }

glm::quat rotateShipToVelocity(Vector velo) {
  glm::vec3 velocity = {velo.x, velo.y, velo.z};
  glm::vec3 normVelocity = glm::normalize(velocity);
  glm::vec3 up(0.0f, 1.0f, 0.0f);

  if (glm::length(normVelocity) < 1e-6f) {
    return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // No rotation if no velocity
  }

  float angle = glm::acos(glm::dot(up, normVelocity));
  glm::vec3 rotationAxis = glm::cross(up, normVelocity);

  if (glm::length(rotationAxis) < 1e-6f) {
    rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
  }

  return glm::angleAxis(angle, glm::normalize(rotationAxis));
}

GUI::GUI(GLFWwindow* window, IController& controller) : window_(window), controller(controller) {}

void GUI::run() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;

  io.Fonts->AddFontFromFileTTF("gui_style/font.ttf", 16.0f);
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init("#version 130");
  ImGui::StyleColorsDark();

  glBindTexture(GL_TEXTURE_2D, 0);

  glViewport(0, 0, width, height);

  Shader shaderProgram("shaders/default.vert", "shaders/default.frag");
  Shader skyboxShader("shaders/skybox.vert", "shaders/skybox.frag");
  Shader asteroidShader("shaders/asteroid.vert", "shaders/default.frag");

  glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);

  shaderProgram.Activate();
  glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
  glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
  skyboxShader.Activate();
  glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
  asteroidShader.Activate();
  glUniform4f(glGetUniformLocation(asteroidShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
  glUniform3f(glGetUniformLocation(asteroidShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CCW);

  float cX = 30.0f;
  float cY = cX / 1.0;
  float cZ = cX / 5;
  Camera camera(width, height, {cX, cZ, cY}, {-2.0f, 0.0f, -2.0f});

  std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
  std::string jupiterPath = "rendmodels/jupiter/scene.gltf";
  std::string asteroidPath = "rendmodels/asteroid/scene.gltf";
  std::string spaceshipPath = "rendmodels/spaceship/scene.gltf";

  RendModel jupiter("rendmodels/jupiter/scene.gltf");
  RendModel ship("rendmodels/spaces/scene.gltf");
  RendModel sun("rendmodels/sun/scene.gltf");

  double prevTime = 0.0;
  double crntTime = 0.0;
  double timeDiff;
  unsigned int counter = 0;

  unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glGenBuffers(1, &skyboxEBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  std::string facesCubemap[6] = {
      "rendmodels/skybox/right.png", "rendmodels/skybox/left.png", "rendmodels/skybox/top.png", "rendmodels/skybox/bottom.png", "rendmodels/skybox/front.png", "rendmodels/skybox/back.png",
  };

  unsigned int cubemapTexture;
  glGenTextures(1, &cubemapTexture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  for (unsigned int i = 0; i < 6; i++) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      stbi_set_flip_vertically_on_load(false);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
      stbi_image_free(data);
    }
  }

  const unsigned int number = 5000;
  float radius = 100.0f;
  float radiusDeviation = 40.0f;

  std::vector<glm::mat4> instanceMatrix;

  for (unsigned int i = 0; i < number; i++) {
    float x = randf();
    float finalRadius = radius + randf() * radiusDeviation;
    float y = ((rand() % 2) * 2 - 1) * sqrt(1.0f - x * x);

    glm::vec3 tempTranslation;
    glm::quat tempRotation;
    glm::vec3 tempScale;

    if (randf() > 0.5f) {
      tempTranslation = glm::vec3(y * finalRadius, randf(), x * finalRadius);
    } else {
      tempTranslation = glm::vec3(x * finalRadius, randf(), y * finalRadius);
    }
    tempRotation = glm::quat(1.0f, randf(), randf(), randf());
    tempScale = 0.1f * glm::vec3(randf(), randf(), randf());

    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    trans = glm::translate(trans, tempTranslation);
    rot = glm::mat4_cast(tempRotation);
    sca = glm::scale(sca, tempScale);

    instanceMatrix.push_back(trans * rot * sca);
  }
  currentDeviceCoords_ = {15.0f, 13.0f, 0.0f};
  initDeviceCords_ = currentDeviceCoords_;

  RendModel asteroid("rendmodels/asteroid/scene.gltf", number, instanceMatrix);
  std::future<void> futureProcess;
  bool observerMode = false;
  acceleratorMode = false;
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ShowCameraControls(observerMode);
    ShowAcceleratorControls(acceleratorMode, fixDistanceMode);
    processInput();

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);       // Set position
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Always);  // Set fixed size
    // Show Mission Window
    ImGui::Begin("New Mission", &showMissionWindow_, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    if (ImGui::BeginTabBar("##Tabs")) {
      if (ImGui::BeginTabItem("Manual Input")) {
        static char dateBuffer[256];
        strcpy(dateBuffer, missionParams.date.c_str());
        ImGui::InputText("##Date", dateBuffer, sizeof(dateBuffer));
        missionParams.date = dateBuffer;

        ImGui::InputDouble("##RA", &missionParams.ra);
        ImGui::InputDouble("##Declination", &missionParams.declination);
        ImGui::InputDouble("##Velocity", &missionParams.v);
        ImGui::InputDouble("##Longitude", &missionParams.longitude);
        ImGui::InputDouble("##Latitude", &missionParams.latitude);
        ImGui::InputDouble("##Elevation", &missionParams.height);

        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Load from File")) {
        static char filePathBuffer[1024] = "";
        ImGui::InputText("File Path", filePathBuffer, sizeof(filePathBuffer));
        if (ImGui::Button("Load")) {
          LoadMissionParamsFromFile(filePathBuffer, missionParams);
        }
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Menu")) {
        static char filePathBuffer[1024] = "";
        ImGui::NewLine();
        if (missionRunning.load() && ImGui::Button("Stop")) {
          stopPressed();
          missionRunning.store(false);
        }

        ImGui::SameLine();
        if (ImGui::Button("Get Back")) {
          std::lock_guard<std::mutex> lock(vectorMutex);
          currentDeviceCoords_ = initDeviceCords_;
          missionRunning.store(false);
          controller.stopSimulation();
        }

        ImGui::SameLine();
        if (ImGui::Button("Resume")) {
          std::lock_guard<std::mutex> lock(vectorMutex);
          controller.Resume();
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
          std::lock_guard<std::mutex> lock(vectorMutex);
          controller.Pause();
        }
        ImGui::NewLine();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));

        // Начало нового фрейма без заголовка и перемещения
        ImGui::BeginChild("Mission Data", ImVec2(0, 200), true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGui::SetWindowFontScale(0.9f);  // Уменьшение масштаба шрифта

        ImGui::Text("Mission Time: %.2f seconds", currentMissionTime_.load());
        ImGui::Text("Coordinates: X=%.2f, Y=%.2f, Z=%.2f", 1e-3 * currentDeviceCoords_.x / missionscale, 1e-3 * currentDeviceCoords_.y / missionscale, 1e-3 * currentDeviceCoords_.z / missionscale);
        ImGui::Text("Velocity: X=%.2f, Y=%.2f, Z=%.2f", 1e-3 * currentDeviceVelo_.x / missionscale, 1e-3 * currentDeviceVelo_.y / missionscale, 1e-3 * currentDeviceVelo_.z / missionscale);
        ImGui::Text("Add. acceleration: %.2f mc^2", acceleration);

        ImGui::EndChild();

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();

        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }

    static std::string errorMessage;
    static bool showError = false;

    if (!missionRunning.load() && ImGui::Button("Start")) {
      if (!missionParams.date.empty() && missionParams.ra != 0.0 && missionParams.declination != 0.0 && missionParams.v != 0.0 && missionParams.latitude != 0.0 && missionParams.longitude != 0.0 && missionParams.height != 0.0) {
        futureProcess = std::async(std::launch::async, &IController::processSimulation, &controller, missionParams);
        missionRunning.store(true);
        showError = false;
      } else {
        showError = true;
        errorMessage = "Please fill in all fields before starting the mission.";
        ImGui::OpenPopup("Error");
      }
    }

    if (showError) {
      ImGui::OpenPopup("Error");
    }
    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("%s", errorMessage.c_str());
      if (ImGui::Button("Close")) {
        showError = false;
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    ImGui::End();
    // End Mission Window

    // Camera orientation
    if (observerMode) {
      Vector vc = currentDeviceCoords_;
      auto vv = glm::vec3(vc.x, vc.z, vc.y);
      camera.Orientation = vv - camera.Position;
    }
    crntTime = glfwGetTime();
    timeDiff = crntTime - prevTime;
    counter++;
    if (timeDiff >= 1.0 / 30.0) {
      std::string FPS = std::to_string((1.0 / timeDiff) * counter);
      std::string ms = std::to_string((timeDiff / counter) * 1000);
      std::string newTitle = "SpaceLaunches - " + FPS + "FPS / " + ms + "ms";
      glfwSetWindowTitle(window_, newTitle.c_str());
      prevTime = crntTime;
      counter = 0;
    }
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.Inputs(window_);
    camera.updateMatrix(45.0f, 0.1f, 1000.0f);
    jupiter.Draw(shaderProgram, camera, glm::vec3(0.0f, 0.0f, 0.0f), {}, {1.0f, 1.0f, 1.0f});
    sun.Draw(shaderProgram, camera, 600.0f * glm::vec3(-0.0f, -0.5f, 1.0f), {1.0f, 0.0f, 0.0f, 0.0f}, {7.0f, 7.0f, 7.0f});

    Vector cords = getDeviceCords();
    float angle = glm::radians(90.0f);
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    glm::quat initrot = glm::angleAxis(angle, axis);
    glm::quat rotation = rotateShipToVelocity(currentDeviceVelo_);
    ship.Draw(shaderProgram, camera, {cords.x, cords.z, cords.y}, rotation * initrot, {0.8f, 0.8f, 0.8f});
    asteroid.Draw(asteroidShader, camera);

    glDepthFunc(GL_LEQUAL);
    skyboxShader.Activate();
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
    projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window_);
  }
  shaderProgram.Delete();
  skyboxShader.Delete();
  glfwDestroyWindow(window_);
  glfwTerminate();
}