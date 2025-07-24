#include <iostream>
#include "lib/hello.h"

int main() {
    demo::Greeter greeter("World");
    
    std::cout << greeter.SayHello() << std::endl;
    std::cout << greeter.SayGoodbye() << std::endl;
    
    return 0;
} 