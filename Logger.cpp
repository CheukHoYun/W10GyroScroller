#include "Logger.h"
#include <iostream>

void Logger::log(const std::string& message) {
#ifdef DEBUG_MODE
    std::cout << message << std::endl;
#endif
}
