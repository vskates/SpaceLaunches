#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "controller.h"
#include "model.h"
#include "view.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <interface_type>" << std::endl;
    return 1;
  }

  std::string interfaceType = argv[1];

  if (interfaceType != "gui" && interfaceType != "console") {
    std::cerr << "Invalid interface type specified!" << std::endl;
    return 1;
  }

  if (interfaceType == "gui") {

    if (!glfwInit()) {
      return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1000, 800, "SpaceLaunches", NULL, NULL);
    if (window == NULL) {
      glfwTerminate();
      return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Couldn't load OpenGL\n";
      glfwTerminate();
      return -1;
    }

    Model model;
    Controller controller(model);
    IView* interface = MakeInterface::createInterface(interfaceType, controller, window);
    if (!interface) {
      std::cerr << "Invalid interface type specified!" << std::endl;
      glfwDestroyWindow(window);
      glfwTerminate();
      return 1;
    }

    controller.setView(interface);
    interface->run();

    delete interface;
    glfwDestroyWindow(window);
    glfwTerminate();
  } else if (interfaceType == "console") {
    Model model;
    Controller controller(model);
    IView* interface = MakeInterface::createInterface(interfaceType, controller, nullptr);
    if (!interface) {
      std::cerr << "Invalid interface type specified!" << std::endl;
      return 1;
    }

    controller.setView(interface);
    if (interfaceType == "gui") {
      controller.gui = true;
    }
    interface->run();

    delete interface;
  }

  return 0;
}
