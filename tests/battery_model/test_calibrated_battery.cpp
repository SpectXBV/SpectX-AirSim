#include <iostream>
#include <cmath>
#include <stdexcept>
#include "MultirotorBatteryModel.hpp"

void check(const char* name, double actual,
           double expected, double tolerance)
{
    const bool passed =
        std::isfinite(actual) &&
        std::abs(actual - expected) <= tolerance;

    std::cout << (passed ? "[PASS] " : "[FAIL] ")
              << name
              << " | Actual: " << actual
              << " | Expected: " << expected
              << std::endl;

    if (!passed)
        throw std::runtime_error(name);
}

int main()
{
    MultirotorBatteryModel battery;
    MultirotorBatteryModel::Config config;

    config.calibrated_power_model = true;
    config.capacity_wh = 648.0f;
    config.nominal_voltage = 21.6f;
    config.mechanical_power_scale = 17.5187f;
    config.avionics_draw_w = 10.0f;
    config.climb_power_coefficient = 0.0f;
    config.initial_state_of_charge = 1.0f;
    config.internal_resistance_ohm = 0.013f;
    config.low_state_of_charge_threshold = 0.15f;

    config.charging_enabled = true;
    config.charge_rate_c = 0.5f;
    config.charging_efficiency = 0.92f;
    config.cv_start_state_of_charge = 0.90f;
    config.cutoff_current_fraction = 0.05f;

    const double pm = 66.9;
    const double pe = 17.5187 * pm + 10.0;

    battery.initialize(config);

    check("Rated battery capacity",
          battery.getState().capacity_wh,
          648.0, 0.001);

    check("Initial stored energy",
          battery.getState().charge_wh,
          648.0, 0.001);

    check("Fully charged OCV",
          battery.getState().voltage,
          25.2, 0.001);

    check("Initial SOC",
          battery.getState().state_of_charge,
          1.0, 0.0001);

    // Reference power and initial current
    battery.update(0.01f, 66.9f, true, false);

    check("Reference electrical power",
          battery.getLastElectricalPowerW(),
          pe, 0.01);

    check("Electrical power consistency",
          battery.getState().voltage *
          battery.getState().current,
          pe, 0.1);

    const double expected_current =
        (25.2 - std::sqrt(
            25.2 * 25.2 - 4.0 * 0.013 * pe))
        / (2.0 * 0.013);

    check("Initial loaded current",
          battery.getState().current,
          expected_current, 0.01);

    check("Initial loaded voltage",
          battery.getState().voltage,
          25.2 - expected_current * 0.013,
          0.01);

    // Energy consumption
    battery.reset();
    battery.update(60.0f, 66.9f, true, false);

    check("Energy consumed in one minute",
          648.0 - battery.getState().charge_wh,
          pe / 60.0, 0.01);

    // Ten-minute flight
    battery.reset();
    battery.update(600.0f, 66.9f, true, false);

    check("SOC after ten minutes",
          battery.getState().state_of_charge,
          1.0 - pe * 600.0 / (3600.0 * 648.0),
          0.0001);

    // Twenty-minute flight
    battery.reset();
    battery.update(1200.0f, 66.9f, true, false);

    check("SOC after twenty minutes",
          battery.getState().state_of_charge,
          1.0 - pe * 1200.0 / (3600.0 * 648.0),
          0.0001);

    // Simulate 28 minutes with one-second updates
    battery.reset();

    for (int t = 0; t < 1680; ++t)
        battery.update(1.0f, 66.9f, true, false);

    check("SOC after 28 minutes",
          battery.getState().state_of_charge,
          1.0 - pe * 1680.0 / (3600.0 * 648.0),
          0.0001);

    check("Low battery reserve flag",
          battery.getState().is_low ? 1 : 0,
          1, 0);

    // Dynamic mechanical loading
    battery.reset();
    battery.update(1.0f, 100.0f, true, false);

    check("High rotor load power",
          battery.getLastElectricalPowerW(),
          17.5187 * 100.0 + 10.0, 0.01);

    // Disarmed battery behavior
    const double energy_before =
        battery.getState().charge_wh;

    battery.update(60.0f, 0.0f, false, false);

    check("Disarmed power",
          battery.getLastElectricalPowerW(),
          0.0, 0.001);

    check("Disarmed energy unchanged",
          battery.getState().charge_wh,
          energy_before, 0.001);

    // Optional climb correction
    config.climb_power_coefficient = 35.0f;
    battery.initialize(config);

    battery.update(1.0f, 66.9f, true, false, 1.0f);

    check("Positive climb correction",
          battery.getLastElectricalPowerW(),
          pe + 35.0, 0.01);

    battery.update(1.0f, 66.9f, true, false, -1.0f);

    check("No climb correction during descent",
          battery.getLastElectricalPowerW(),
          pe, 0.01);

    // Charging at manufacturer-recommended 0.5C
    config.climb_power_coefficient = 0.0f;
    config.initial_state_of_charge = 0.5f;

    battery.initialize(config);
    battery.update(60.0f, 0.0f, false, true);

    check("Charging current",
          battery.getState().current,
          -15.0, 0.001);

    check("Charging SOC",
          battery.getState().state_of_charge,
          0.5 + 0.92 * 0.5 / 60.0,
          0.0001);

    check("Charging active",
          battery.getState().is_charging ? 1 : 0,
          1, 0);

    // Armed state takes precedence over docking
    battery.update(1.0f, 66.9f, true, true);

    check("No charging while armed",
          battery.getState().is_charging ? 1 : 0,
          0, 0);

    // Full battery depletion
    config.initial_state_of_charge = 1.0f;
    battery.initialize(config);
    battery.update(3600.0f, 66.9f, true, false);

    check("Depleted SOC",
          battery.getState().state_of_charge,
          0.0, 0.0001);

    check("Depleted voltage",
          battery.getState().voltage,
          0.0, 0.001);

    // Legacy battery regression test
    MultirotorBatteryModel::Config legacy;
    battery.initialize(legacy);

    check("Legacy battery capacity",
          battery.getState().capacity_wh,
          44.4, 0.001);

    battery.update(60.0f, 66.9f, true, false);

    check("Legacy electrical power",
          battery.getLastElectricalPowerW(),
          66.9 / 0.85 + 10.0, 0.01);

    std::cout << "\nALL BATTERY TESTS PASSED\n";
    return 0;
}