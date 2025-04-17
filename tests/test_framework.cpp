#include "test_framework.hpp"

#include<string>
#include<functional>
#include<vector>
#include<iostream>

namespace testing {
    void run_tests(std::string header, const std::vector<TestCase>& tests) {
        int passed = 0;
        std::cout << "\nRunning " << header << " tests...\n" << std::endl;
        for (const auto& test : tests) {
            bool success = test.func();
            std::cout << (success ? "[PASS] " : "[FAIL] ") << test.name << std::endl;
            passed += success;
        }
        std::cout << "Summary: " << passed << "/" << tests.size() << " tests passed." << std::endl;
    }
}
