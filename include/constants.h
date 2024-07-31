#ifndef _CONSTANTS_H
#define	_CONSTANTS_H
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

namespace Constants {
	static constexpr double MassOfSun = 1.988435e30;
	static constexpr double ParsecInMeter = 3.08567758129e16;
	static constexpr double Gamma = 6.67428e-11;
	static constexpr double EarthMass = 2.172e28;
	static constexpr double EarthRadius = 6371e3;
	static std::string initDate = "2024-03-20 00:00:00";
};

class SystemConstants {
 public:
  static constexpr double defaultDt = 0.02;
};

long long calculate_difference_in_seconds(const std::string& start, const std::string& end);

#endif // _CONSTANTS_H