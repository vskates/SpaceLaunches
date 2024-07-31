#include "view.h"

IView* MakeInterface::createInterface(const std::string& choice, IController& controller, GLFWwindow* window) {
  if (choice == "gui")
    return new GUI(window, controller);
  else if (choice == "console")
    return new Console(controller);
  else
    return nullptr;
}