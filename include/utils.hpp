/**
 * @file utils.hpp
 * @author Sergey Serebryanskiy (serebryanskiysergei@gmail.com.com)
 * @brief Additional functions useful across all classes
 * @version 0.1
 * @date 2020-05-31
 * 
 * @copyright Copyright (c) 2020
 */

#if !defined(UTILS_H_)
#define UTILS_H_

#include <ctime>
#include <string>

/**
 * @brief Static cClass with all extra functions
 */
class Utils {
    public:
        /**
         * @brief Parse timestamp from defined format
         * 
         * @param timestamp time in format %Y-%m-%dT%H:%M:%S
         * @return time_t parsed c++ time structure 
         */
        static time_t parseTimestapm(const std::string timestamp);

        /**
         * @brief Converts time_t structure to a string
         * 
         * @param time time_t presentation of time
         * @return std::string string presentation in format %Y-%m-%dT%H:%M:%S
         */
        static std::string timeToString(const time_t time);
};

#endif // UTILS_H_
