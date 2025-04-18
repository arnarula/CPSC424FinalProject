#include "test_framework.hpp"

// Declare test groups
extern std::vector<testing::TestCase> init_tests;
extern std::vector<testing::TestCase> graph_builder_tests;
extern std::vector<testing::TestCase> runtime_tests;

int main() {
    // std::vector<testing::TestCase> all_tests;
    // all_tests.insert(all_tests.end(), init_tests.begin(), init_tests.end());
    testing::run_tests("graph building", graph_builder_tests);
    testing::run_tests("algorithm", init_tests);
    testing::run_tests("runtime", runtime_tests);

    return 0;
}