#include "physics/StochasticWindModel.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>

using namespace msr::airlib;

int main()
{
    StochasticWindModel model;
    StochasticWindModel::Config config;

    config.enabled = true;
    config.mean_speed_mps = 10.0f;
    config.direction_ned = Vector3r(0, 1, 0);
    config.reference_altitude_m = 5.0f;
    config.shear_exponent = 0.0f;
    config.update_rate_hz = 10.0f;
    config.random_seed = 42;
    config.gust_enabled = false;
    config.turbulence_enabled = false;

    std::cout << std::fixed << std::setprecision(6);

    // TEST 1: Constant reference wind.
    model.configure(config);

    auto wind = model.sample(0, 5.0f);

    std::cout << "\n===== TEST 1: MEAN WIND =====\n";
    std::cout << "Expected NED: 0 10 0\n";
    std::cout << "Actual NED:   "
              << wind.transpose() << "\n";

    // TEST 2: Power-law shear.
    config.shear_exponent = 0.14f;
    model.configure(config);

    std::cout << "\n===== TEST 2: WIND SHEAR =====\n";

    for (float z : {1.0f, 5.0f, 10.0f, 50.0f, 100.0f})
    {
        wind = model.sample(0, z);

        double expected =
            10.0 * std::pow(z / 5.0, 0.14);

        std::cout << "Altitude: " << z
                  << " m | Expected: " << expected
                  << " | Actual: " << wind.norm()
                  << "\n";
    }

    // TEST 3: Gusts only.
    config.shear_exponent = 0.0f;
    config.gust_enabled = true;
    model.configure(config);

    std::ofstream gust_csv("/tmp/wind_gusts.csv");
    gust_csv << "time,north,east,down,speed\n";

    double gust_min = 1e9;
    double gust_max = -1e9;

    for (int i = 0; i < 6000; ++i)
    {
        wind = model.sample(
            uint64_t(i) * 100000000ULL, 5.0f);

        double speed = wind.norm();

        gust_min = std::min(gust_min, speed);
        gust_max = std::max(gust_max, speed);

        gust_csv << i / 10.0 << ","
                 << wind.x() << ","
                 << wind.y() << ","
                 << wind.z() << ","
                 << speed << "\n";
    }

    std::cout << "\n===== TEST 3: GUSTS =====\n";
    std::cout << "Minimum speed: " << gust_min << "\n";
    std::cout << "Maximum speed: " << gust_max << "\n";

    // TEST 4: Kaimal turbulence only.
    config.gust_enabled = false;
    config.turbulence_enabled = true;
    config.turbulence_stddev_mps = 1.0f;
    config.turbulence_length_scale_m = 50.0f;
    config.turbulence_series_duration_sec = 600.0f;

    model.configure(config);

    std::ofstream turbulence_csv("/tmp/wind_turbulence.csv");
    turbulence_csv << "time,speed,turbulence\n";

    double sum = 0.0;
    double sum_sq = 0.0;

    const int count = 6000;

    for (int i = 0; i < count; ++i)
    {
        wind = model.sample(
            uint64_t(i) * 100000000ULL, 5.0f);

        double fluctuation = wind.y() - 10.0;

        sum += fluctuation;
        sum_sq += fluctuation * fluctuation;

        turbulence_csv << i / 10.0 << ","
                       << wind.y() << ","
                       << fluctuation << "\n";
    }

    double mean = sum / count;
    double stddev = std::sqrt(
        std::max(0.0, sum_sq / count - mean * mean));

    std::cout << "\n===== TEST 4: KAIMAL TURBULENCE =====\n";
    std::cout << "Samples: " << count << "\n";
    std::cout << "Turbulence mean: " << mean << " m/s\n";
    std::cout << "Target stddev: 1.0 m/s\n";
    std::cout << "Actual stddev: " << stddev << " m/s\n";

    // TEST 5: Seed reproducibility.
    StochasticWindModel model2;

    model.configure(config);
    model2.configure(config);

    double max_error = 0.0;

    for (int i = 0; i < count; ++i)
    {
        uint64_t t = uint64_t(i) * 100000000ULL;

        auto a = model.sample(t, 5.0f);
        auto b = model2.sample(t, 5.0f);

        max_error = std::max(
            max_error, double((a - b).norm()));
    }

    std::cout << "\n===== TEST 5: REPRODUCIBILITY =====\n";
    std::cout << "Maximum difference: "
              << max_error << " m/s\n";

    // TEST 6: Turbulence repetition.
    model.configure(config);

    auto first = model.sample(0, 5.0f);
    auto repeated = model.sample(600000000000ULL, 5.0f);

    std::cout << "\n===== TEST 6: PERIODICITY =====\n";
    std::cout << "Wind at 0 s: "
              << first.transpose() << "\n";
    std::cout << "Wind at 600 s: "
              << repeated.transpose() << "\n";
    std::cout << "Difference: "
              << (first - repeated).norm() << "\n";

    std::cout << "\nCSV files saved in /tmp\n";

    return 0;
}
