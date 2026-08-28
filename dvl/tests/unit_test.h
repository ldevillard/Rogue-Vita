#pragma once

#include <cmath>
#include <iostream>
#include <vector>

namespace dvl
{
    using TestMethod = bool(*)();

    struct UnitTest
    {
        const char* name;
        TestMethod method;
    };

    inline std::vector<UnitTest>& GetUnitTests()
    {
        static std::vector<UnitTest> tests;
        return tests;
    }

    struct UnitTestRegistrar
    {
        UnitTestRegistrar(const char* name, TestMethod method)
        {
            GetUnitTests().push_back({ name, method });
        }
    };

    inline int RunUnitTests()
    {
        const std::vector<UnitTest>& tests = GetUnitTests();
        constexpr const char* cyan = "\033[1;36m";
        constexpr const char* green = "\033[1;32m";
        constexpr const char* red = "\033[1;31m";
        constexpr const char* reset = "\033[0m";

        std::size_t passed = 0;

        std::cout << "Running " << tests.size() << " tests\n\n";

        for (const UnitTest& test : tests)
        {
            std::cout << '[' << cyan << "RUN" << reset << "] "
                      << test.name << '\n';

            if (test.method())
            {
                passed++;
                std::cout << '[' << green << "PASS" << reset << "] "
                          << test.name << "\n\n";
            }
            else
            {
                std::cout << '[' << red << "FAIL" << reset << "] "
                          << test.name << "\n\n";
            }
        }

        std::cout << passed << "/" << tests.size() << " tests passed\n";

        return passed == tests.size() ? 0 : 1;
    }
}

#define DVL_TEST(name)                                              \
    static bool name();                                             \
    static ::dvl::UnitTestRegistrar name##_registrar(#name, &name); \
    static bool name()

#define DVL_EXPECT_TRUE(expression)                     \
    do                                                  \
    {                                                   \
        if (!(expression))                              \
        {                                               \
            std::cerr                                   \
                << "  Expected true: " #expression "\n" \
                << "  " << __FILE__ << ":" << __LINE__  \
                << '\n';                                \
            return false;                               \
        }                                               \
    } while (false)

#define DVL_EXPECT_FALSE(expression)                     \
    do                                                   \
    {                                                    \
        if (expression)                                  \
        {                                                \
            std::cerr                                    \
                << "  Expected false: " #expression "\n" \
                << "  " << __FILE__ << ":" << __LINE__   \
                << '\n';                                 \
            return false;                                \
        }                                                \
    } while (false)

#define DVL_EXPECT_EQ(a, b)                            \
    do                                                 \
    {                                                  \
        if (!((a) == (b)))                             \
        {                                              \
            std::cerr                                  \
                << "  Expected: " #a " == " #b "\n"    \
                << "  " << __FILE__ << ":" << __LINE__ \
                << '\n';                               \
            return false;                              \
        }                                              \
    } while (false)

#define DVL_EXPECT_NEAR(a, b, epsilon)                       \
    do                                                       \
    {                                                        \
        const auto dvl_test_a = (a);                         \
        const auto dvl_test_b = (b);                         \
                                                             \
        if (std::fabs(dvl_test_a - dvl_test_b) > (epsilon))  \
        {                                                    \
            std::cerr                                        \
                << "  Expected: " #a " ~= " #b "\n"          \
                << "  Actual: " << dvl_test_a                \
                << " vs " << dvl_test_b << '\n'              \
                << "  " << __FILE__ << ":" << __LINE__       \
                << '\n';                                     \
            return false;                                    \
        }                                                    \
    } while (false)
