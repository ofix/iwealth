#include "ui/RichHelper.h"
#include <iomanip>
#include <sstream>

std::string convertDouble(double digit, int precision) {
    std::stringstream sstream;
    sstream << std::fixed << std::setprecision(precision) << digit;
    return sstream.str();
}
