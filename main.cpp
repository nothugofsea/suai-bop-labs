#include <iostream>
#include <algorithm>
#include <execution>
#include <vector>
#include <random>
#include <variant>
#include <chrono>

const int SIZE = 5000000;
const int E1 = 256;
const int E2 = 512;

using parallel_policy = std::variant
                        <
                         std::execution::sequenced_policy,
                         std::execution::parallel_policy,
                         std::execution::parallel_unsequenced_policy,
                         std::execution::unsequenced_policy
                        >;

void benchmark(const parallel_policy &policy) {
    std::vector<int> array(SIZE);
    std::mt19937 gen;
    std::uniform_int_distribution<int> dis(0, 100000);
    auto rand_num ([=] () mutable { return dis(gen); });

    std::visit
    (
        [&](auto policy_real)
        {
            std::generate(policy_real, std::begin(array), std::end(array), rand_num);
            std::sort(policy_real, std::begin(array), std::end(array));
            std::reverse(policy_real, std::begin(array), std::end(array));
            std::replace(policy_real, std::begin(array), std::end(array), E1, E2);
        },
        policy
    );
}

void print_time(const parallel_policy &policy) {
    auto start = std::chrono::high_resolution_clock::now();

    benchmark(std::execution::par);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Duration: " << duration.count() << " microseconds" << std::endl;
}


int main() {
    parallel_policy policies[] = { std::execution::seq, std::execution::par, std::execution::par_unseq };

    std::for_each(std::begin(policies), std::end(policies), print_time);

    return 0;
}