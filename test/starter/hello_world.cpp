#include "starter/hello_world.hpp"

#include <optional>  // std::optional
#include <stdexcept> // std::invalid_argument
#include <string>    // std::string
#include <vector>    // std::vector

#include <gmock/gmock.h> // EXPECT_THAT, matchers
#include <gtest/gtest.h> // TEST, EXPECT_*, ASSERT_*

using ::testing::ElementsAre;
using ::testing::HasSubstr;
using ::testing::Optional;
using ::testing::SizeIs;

// test 이름은 PascalCase 로 한다. _ 언더바는 권장하지 않음
TEST(HelloWorld, TEST) {
    starter::say_hello();
    EXPECT_TRUE(true);
}

// NOLINTNEXTLINE
int parse_positive(const std::string& s) {
    int const v = std::stoi(s);
    if (v <= 0) {
        throw std::invalid_argument("not positive");
    }
    return v;
}

TEST(Sample, Basic) {
    std::vector<int> v{1, 2, 3};
    ASSERT_THAT(v, SizeIs(3)); // 크기 다르면 아래는 의미 없으니 ASSERT
    EXPECT_EQ(v[0], 1) << "첫 원소 확인";
    EXPECT_THAT(v, ElementsAre(1, 2, 3));
}

TEST(Sample, StringAndFloat) {
    constexpr std::string msg = "hello world";
    EXPECT_THAT(msg, HasSubstr("world"));
    EXPECT_STREQ("abc", std::string("abc").c_str()); // const char* 비교
    EXPECT_NEAR(0.1 + 0.2, 0.3, 1e-9);
    EXPECT_DOUBLE_EQ(0.1 + 0.2, 0.3);
}

TEST(Sample, Exceptions) {
    EXPECT_THROW(parse_positive("-1"), std::invalid_argument);
    EXPECT_NO_THROW(parse_positive("5"));
}

TEST(Sample, Optional) {
    constexpr std::optional<int> o = 42;
    EXPECT_THAT(o, Optional(42));
}
