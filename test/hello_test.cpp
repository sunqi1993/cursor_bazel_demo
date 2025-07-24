#include <gtest/gtest.h>
#include "lib/hello.h"

namespace demo {
namespace {

class GreeterTest : public ::testing::Test {
protected:
    void SetUp() override {
        greeter_ = std::make_unique<Greeter>("Test");
    }

    std::unique_ptr<Greeter> greeter_;
};

TEST_F(GreeterTest, SayHelloReturnsCorrectMessage) {
    EXPECT_EQ(greeter_->SayHello(), "Hello, Test!");
}

TEST_F(GreeterTest, SayGoodbyeReturnsCorrectMessage) {
    EXPECT_EQ(greeter_->SayGoodbye(), "Goodbye, Test!");
}

TEST(GreeterConstructorTest, ConstructorWithDifferentNames) {
    Greeter alice("Alice");
    Greeter bob("Bob");
    
    EXPECT_EQ(alice.SayHello(), "Hello, Alice!");
    EXPECT_EQ(bob.SayHello(), "Hello, Bob!");
}

} // namespace
} // namespace demo 