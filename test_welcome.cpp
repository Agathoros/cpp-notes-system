#include <gtest/gtest.h>
#include "welcome.h"

// Тест проверяет, что функция возвращает правильное приветствие
TEST(WelcomeTest, ReturnsCorrectMessage) {
    std::string expected = "Добро пожаловать!";
    std::string actual = getWelcomeMessage();
    EXPECT_EQ(expected, actual);
}

// Тест проверяет, что сообщение не пустое
TEST(WelcomeTest, MessageNotEmpty) {
    std::string message = getWelcomeMessage();
    EXPECT_FALSE(message.empty());
}

// Тест проверяет длину сообщения
TEST(WelcomeTest, MessageHasCorrectLength) {
    std::string message = getWelcomeMessage();
    EXPECT_GT(message.length(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 