#include <iostream>
#include <string>
#include "request.h"

int main(int argc, char** argv) {
    std::string response = HttpGet("https://www.baidu.com");
    std::cout << response << std::endl;
    return 1;
}