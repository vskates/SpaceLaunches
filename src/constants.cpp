#include "constants.h"

long long calculate_difference_in_seconds(const std::string& start, const std::string& end) {
	std::tm start_tm = {};
	std::tm end_tm = {};
	std::istringstream start_ss(start);
	std::istringstream end_ss(end);

	start_ss >> std::get_time(&start_tm, "%Y-%m-%d %H:%M:%S");
	end_ss >> std::get_time(&end_tm, "%Y-%m-%d %H:%M:%S");

	auto start_time_point = std::chrono::system_clock::from_time_t(std::mktime(&start_tm));
	auto end_time_point = std::chrono::system_clock::from_time_t(std::mktime(&end_tm));

	return std::chrono::duration_cast<std::chrono::seconds>(end_time_point - start_time_point).count();
}