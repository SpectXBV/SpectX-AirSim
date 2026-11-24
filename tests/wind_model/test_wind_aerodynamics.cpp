
#include "physics/FastPhysicsEngine.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace msr::airlib;

static Wrench getCylinderDragWrench(
            const Quaternionr& orientation,
            const Vector3r& linear_vel,
            const Vector3r& wind_world,
            const StochasticWindModel::AerodynamicsConfig& aero)
        {
            Wrench wrench = Wrench::zero();

            // Relative air velocity in the vehicle body frame.
            const Vector3r relative_world =
                linear_vel - wind_world;

            Vector3r relative_body =
                VectorMath::transformToBodyFrame(
                    relative_world, orientation);

            // Extract roll and pitch from the orientation quaternion.
            const double qw = orientation.w();
            const double qx = orientation.x();
            const double qy = orientation.y();
            const double qz = orientation.z();

            const double roll = std::atan2(
                2.0 * (qw * qx + qy * qz),
                1.0 - 2.0 * (qx * qx + qy * qy));

            const double sin_pitch = std::max(
                -1.0, std::min(
                    1.0, 2.0 * (qw * qy - qz * qx)));

            const double pitch = std::asin(sin_pitch);

            constexpr double pi = 3.14159265358979323846;

            const double radius =
                std::max(0.0, double(aero.radius_m));

            const double height =
                std::max(0.0, double(aero.height_m));

            // Effective lateral and circular areas.
            const double lateral_area =
                std::max(0.0, double(aero.lateral_area_scale))
                * pi * radius * height;

            const double frontal_area =
                std::max(0.0, double(aero.frontal_area_scale))
                * pi * radius * radius;

            // Attitude-dependent projected areas.
            const double area_x =
                lateral_area * std::abs(std::cos(pitch))
                + frontal_area * std::abs(std::sin(pitch));

            const double area_y =
                lateral_area * std::abs(std::cos(roll))
                + frontal_area * std::abs(std::sin(roll));

            if (!aero.vertical_drag)
                relative_body.z() = 0.0f;

            const double relative_speed = relative_body.norm();

            const double density =
                std::max(0.0, double(aero.air_density_kg_m3));

            const double cd =
                std::max(0.0, double(aero.drag_coefficient));

            const double factor =
                -0.5 * density * cd * relative_speed;

            // Quadratic aerodynamic force in the body frame.
            Vector3r force_body(
                static_cast<real_T>(
                    factor * area_x * relative_body.x()),

                static_cast<real_T>(
                    factor * area_y * relative_body.y()),

                aero.vertical_drag
                    ? static_cast<real_T>(
                        factor
                        * std::max(0.0, double(aero.vertical_area_m2))
                        * relative_body.z())
                    : 0.0f);

            // Reject non-finite aerodynamic forces.
            if (!force_body.allFinite())
                return wrench;

            // Limit the total aerodynamic force magnitude.
            const real_T magnitude = force_body.norm();

            if (aero.maximum_force_n > 0.0f &&
                magnitude > aero.maximum_force_n)
            {
                force_body *= aero.maximum_force_n / magnitude;
            }

            // Aerodynamic moment about the center of gravity.
            wrench.torque =
                aero.center_of_pressure_body_m.cross(force_body);

            // AirSim expects world-frame force and body-frame torque.
            wrench.force =
                VectorMath::transformToWorldFrame(
                    force_body, orientation);

            return wrench;
        }


int main()
{
    StochasticWindModel::AerodynamicsConfig aero;

    const Quaternionr level = Quaternionr::Identity();
    const Vector3r stationary = Vector3r::Zero();

    const double pi = 3.14159265358979323846;
    const double area = pi * aero.radius_m * aero.height_m;
    const double k = 0.5 * aero.air_density_kg_m3
                         * aero.drag_coefficient;

    int failures = 0;

    auto check = [&](const char* name,
                     double actual,
                     double expected)
    {
        bool pass = std::isfinite(actual) &&
            std::abs(actual - expected) < 0.001;

        std::cout << name
                  << " | Expected: " << expected
                  << " | Actual: " << actual
                  << " | " << (pass ? "PASS" : "FAIL")
                  << "\n";

        if (!pass)
            ++failures;
    };

    std::cout << std::fixed << std::setprecision(6);

    std::cout << "\n===== TEST 1: QUADRATIC DRAG =====\n";

    for (float speed : {0.f, 5.f, 10.f, 15.f, 20.f})
    {
        auto w = getCylinderDragWrench(
            level, stationary,
            Vector3r(speed, 0, 0), aero);

        std::cout << "Wind: " << speed << " m/s\n";

        check("Force X", w.force.x(),
              k * area * speed * speed);

        check("Force Y", w.force.y(), 0);
        check("Force Z", w.force.z(), 0);
    }

    std::cout << "\n===== TEST 2: RELATIVE VELOCITY =====\n";

    auto w = getCylinderDragWrench(
        level,
        Vector3r(5, 0, 0),
        Vector3r(10, 0, 0), aero);

    check("Following wind", w.force.x(), k * area * 25);

    w = getCylinderDragWrench(
        level,
        Vector3r(10, 0, 0),
        Vector3r(10, 0, 0), aero);

    check("Matching wind speed", w.force.norm(), 0);

    w = getCylinderDragWrench(
        level,
        Vector3r(-5, 0, 0),
        Vector3r(10, 0, 0), aero);

    check("Flying against wind", w.force.x(),
          k * area * 225);

    std::cout << "\n===== TEST 3: WIND DIRECTION =====\n";

    w = getCylinderDragWrench(
        level, stationary,
        Vector3r(-10, 0, 0), aero);

    check("Negative X wind", w.force.x(), -k * area * 100);

    w = getCylinderDragWrench(
        level, stationary,
        Vector3r(0, 10, 0), aero);

    check("East wind", w.force.y(), k * area * 100);

    std::cout << "\n===== TEST 4: PITCH DEPENDENCE =====\n";

    for (double degrees : {0.0, 15.0, 30.0, 45.0})
    {
        double angle = degrees * pi / 180.0;

        Quaternionr orientation(
            std::cos(angle / 2),
            0,
            std::sin(angle / 2),
            0);

        w = getCylinderDragWrench(
            orientation, stationary,
            Vector3r(10, 0, 0), aero);

        auto body_force =
            VectorMath::transformToBodyFrame(
                w.force, orientation);

        double projected_area =
            area * std::abs(std::cos(angle))
            + pi * aero.radius_m * aero.radius_m
                 * std::abs(std::sin(angle));

        double expected =
            k * projected_area * 100
              * std::cos(angle) * std::cos(angle);

        std::cout << "Pitch: " << degrees << " deg\n";

        check("Body X force", body_force.x(), expected);
        check("Body Z force", body_force.z(), 0);
    }

    std::cout << "\n===== TEST 5: CENTER OF PRESSURE =====\n";

    aero.center_of_pressure_body_m =
        Vector3r(0, 0, 0.2f);

    w = getCylinderDragWrench(
        level, stationary,
        Vector3r(10, 0, 0), aero);

    check("Torque Y", w.torque.y(),
          0.2 * k * area * 100);

    check("Torque X", w.torque.x(), 0);
    check("Torque Z", w.torque.z(), 0);

    aero.center_of_pressure_body_m = Vector3r::Zero();

    std::cout << "\n===== TEST 6: VERTICAL DRAG =====\n";

    aero.vertical_drag = false;

    w = getCylinderDragWrench(
        level, stationary,
        Vector3r(0, 0, 10), aero);

    check("Vertical drag disabled", w.force.norm(), 0);

    aero.vertical_drag = true;

    w = getCylinderDragWrench(
        level, stationary,
        Vector3r(0, 0, 10), aero);

    check("Vertical drag enabled", w.force.z(),
          k * aero.vertical_area_m2 * 100);

    std::cout << "\n===== TEST 7: FORCE CLAMP =====\n";

    aero.maximum_force_n = 100.0f;

    w = getCylinderDragWrench(
        level, stationary,
        Vector3r(50, 0, 0), aero);

    check("Force magnitude", w.force.norm(), 100);

    std::cout << "\n===== FINAL RESULT =====\n";
    std::cout << "Failed checks: " << failures << "\n";

    return failures == 0 ? 0 : 1;
}
