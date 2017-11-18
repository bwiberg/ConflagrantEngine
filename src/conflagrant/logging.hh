#pragma once

#include <iostream>

#ifndef LOG_ERROR
#define LOG_ERROR(identifier) std::cerr << #identifier << ": "
#endif // LOG_ERROR

#ifndef LOG_INFO
#define LOG_INFO(identifier) std::cout << #identifier << ": "
#endif // LOG_INFO
