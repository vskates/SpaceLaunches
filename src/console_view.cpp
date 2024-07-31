#include <fstream>
#include <future>
#include <iostream>

#include "utiles.h"
#include "view.h"

Console::Console(IController& controller) : controller(controller) {}

void Console::updateInitCords(const Vector& newDeviceCords)  {
}

void Console::run() {
  std::string command;
  std::cout << "Start mission with 'START', stop - 'EXIT'\n";
  while (true) {
    std::getline(std::cin, command);
    if (command == "START") {
      InitParams inits;

      getParamsSource(inits);
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Display of device current position is off. To turn on - enter 's' after start of mission, to disable - 'q'\n";
      std::future<void> futureProcess = std::async(std::launch::async, &IController::processSimulation, &controller, inits);
      while (true) {
        std::getline(std::cin, command);
        if (command == "EXIT") {
          controller.stopSimulation();
          break;
        }
        if (command == "UPDATE") {
          Vector velo;
          std::cout << "Enter new velocity - input the components\n";
          std::cin >> velo.x >> velo.y >> velo.z;
          controller.includeUserInput(velo);
        }
        if (command == "SPEED UP") {
          std::cout << "Type thr acceleration multiplier\n";
          int multiplier;
          std::cin >> multiplier;
          controller.speedUpMission(multiplier);
        }
        if (command == "q") {
          displayMode = false;
          std::cout << "Display Info mode is off\n";
        }
        if (command == "s") {
          displayMode = true;
          std::cout << "Display Info mode is on\n";
        }
      }
    } else if (command == "EXIT") {
      break;
    }
  }
}

void Console::LoadMissionParamsFromCin(InitParams& missionParams) {
    std::cout << "Enter mission parameters:\n";
    std::cout << "Date (YYYY-MM-DD): ";
    std::cin >> missionParams.date;
    std::cout << "Right Ascension (RA): ";
    std::cin >> missionParams.ra;
    std::cout << "Declination: ";
    std::cin >> missionParams.declination;
    std::cout << "Velocity: ";
    std::cin >> missionParams.v;
    std::cout << "Longitude: ";
    std::cin >> missionParams.longitude;
    std::cout << "Latitude: ";
    std::cin >> missionParams.latitude;
    std::cout << "Elevation: ";
    std::cin >> missionParams.height;
}

bool Console::LoadMissionParamsFromFile(InitParams& params, const std::string& filePath) {
  std::ifstream file(filePath);
        std::string line, key;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            double value;
            iss >> key;
            if (key == "date") {
                std::getline(iss >> std::ws, params.date);
            } else {
                iss >> value;
                if (key == "ra") params.ra = value;
                else if (key == "declination") params.declination = value;
                else if (key == "velocity") params.v = value;
                else if (key == "longitude") params.longitude = value;
                else if (key == "latitude") params.latitude = value;
                else if (key == "elevation") params.height = value;
            }
        }
    return file.good() || file.eof();
}

void Console::getParamsSource(InitParams& params) {
  std::cout << "Enter 'cin' for console input or 'file' to read from a file:\n";
  std::string source;
  std::cin >> source;
  if (source == "cin") {
    LoadMissionParamsFromCin(params);
  } else if (source == "file") {
    std::cout << "Enter file path:\n";
    std::string filePath;
    std::cin >> filePath;
    LoadMissionParamsFromFile(params, filePath);
  } else {
    std::cout << "Invalid input source.\n";
  }
}

void Console::displayIterationInfo(const Vector& newDeviceCords, double& missionTime, const Vector& newVelo) {
  if (!displayMode) {
    return;
  }
  double deviceHeight = std::sqrt(newDeviceCords.len2()) - Constants::EarthRadius;
  double deviceLatitude = std::atan(newDeviceCords.z / std::sqrt(std::pow(newDeviceCords.x, 2) + std::pow(newDeviceCords.y, 2))) * 180.0 / 3.14;
  double deviceLongtitude = std::atan(newDeviceCords.y / newDeviceCords.x) * 180.0 / 3.14;
  std::cout << "Device position: " << deviceHeight << ", lat : " << deviceLatitude << ", lon : " << deviceLongtitude << "\n";
  std::cout << "Time passed:" << secondsToHMS(missionTime) << "\n";
}

void Console::exitMission(Vector tempCords, double tempTime) {
  std::cout << "Mission completed\n";
  std::cout << "Final coordinates : " << tempCords << "\n";
  std::cout << "Mission time : " << tempTime << "\n";
}