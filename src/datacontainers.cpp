#include "datacontainers.h"
#include <cmath>
#include "constants.h"

ACords calculateRADeclination(const std::string& date, double latitude, double longitude) {
    struct std::tm tm = {};
    strptime(date.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    std::time_t time = std::mktime(&tm);
    double dayOfYear = tm.tm_yday + (tm.tm_hour + (tm.tm_min / 60.0)) / 24.0;

    double orbitAngle = 2 * M_PI * (dayOfYear / 365.25);
    double gst = std::fmod(6.697374558 + 0.06570982441908 * tm.tm_yday + 1.00273790935 * tm.tm_hour, 24.0) * (M_PI / 12);
    longitude = longitude * M_PI / 180.0;

    double ra = std::fmod(gst + longitude + orbitAngle, 2 * M_PI);
    double declination = latitude * M_PI / 180.0;

    return {ra * (180 / M_PI), declination * (180 / M_PI)};
}

double ObjectState::Radius() {
	return x * x + y * y + z * z;
}