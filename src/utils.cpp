#include "utils.hpp"

#include <sstream>
#include <iomanip>
#include <chrono>

time_t Utils::parseTimestapm(std::string timestamp) {
    std::tm date = {};
    std::stringstream ss {timestamp};
    ss >> std::get_time(&date, "%Y-%m-%dT%H:%M:%S");
    return mktime(&date);
}

std::string Utils::timeToString(const time_t time) {
    char buffer[26];
    std::tm* date = localtime(&time);
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", date);
    return std::string(buffer);
};