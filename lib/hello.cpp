#include "hello.h"

namespace demo {

Greeter::Greeter(const std::string& name) : name_(name) {}

std::string Greeter::SayHello() const {
    return "Hello, " + name_ + "!";
}

std::string Greeter::SayGoodbye() const {
    return "Goodbye, " + name_ + "!";
}

} // namespace demo 