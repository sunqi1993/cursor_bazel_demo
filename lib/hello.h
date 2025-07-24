#pragma once

#include <string>

namespace demo {

class Greeter {
public:
    explicit Greeter(const std::string& name);
    std::string SayHello() const;
    std::string SayGoodbye() const;

private:
    std::string name_;
};

} // namespace demo