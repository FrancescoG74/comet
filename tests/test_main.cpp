#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <QVector3D>
#include <QColor>
#include <cmath>
#include "astronomicalbody.h"
#include "solarsimconstants.h"

using Catch::Matchers::WithinRel;

// ============================================================================
// AstronomicalBody Constructor Tests (1-5)
// ============================================================================

TEST_CASE("AstronomicalBody default constructor", "[constructor]") {
    AstronomicalBody body;
    REQUIRE(body.getPosition() == QVector3D(0, 0, 0));
    REQUIRE(body.getVelocity() == QVector3D(0, 0, 0));
    REQUIRE(body.getMass() == 0.0);
    REQUIRE(body.getRadius() == 0.0);
}

TEST_CASE("AstronomicalBody full constructor", "[constructor]") {
    QVector3D pos(100, 200, 300);
    QVector3D vel(1, 2, 3);
    double mass = 5000.0;
    double radius = 50.0;
    QColor color = Qt::blue;
    
    AstronomicalBody body(pos, vel, mass, radius, color);
    
    REQUIRE(body.getPosition() == pos);
    REQUIRE(body.getVelocity() == vel);
    REQUIRE(body.getMass() == mass);
    REQUIRE(body.getRadius() == radius);
    REQUIRE(body.getColor() == color);
}

TEST_CASE("AstronomicalBody with negative coordinates", "[constructor]") {
    QVector3D pos(-100, -200, -300);
    QVector3D vel(-1, -2, -3);
    
    AstronomicalBody body(pos, vel, 1000.0, 25.0, Qt::red);
    
    REQUIRE(body.getPosition() == pos);
    REQUIRE(body.getVelocity() == vel);
}

TEST_CASE("AstronomicalBody with zero values", "[constructor]") {
    AstronomicalBody body(QVector3D(0, 0, 0), QVector3D(0, 0, 0), 0.0, 0.0, Qt::white);
    
    REQUIRE(body.getPosition().length() == 0.0);
    REQUIRE(body.getVelocity().length() == 0.0);
    REQUIRE(body.getMass() == 0.0);
    REQUIRE(body.getRadius() == 0.0);
}

TEST_CASE("AstronomicalBody with very large values", "[constructor]") {
    double largeValue = 1e10;
    AstronomicalBody body(
        QVector3D(largeValue, largeValue, largeValue),
        QVector3D(largeValue, largeValue, largeValue),
        largeValue,
        largeValue,
        Qt::yellow
    );
    
    REQUIRE(body.getPosition().x() == largeValue);
    REQUIRE(body.getMass() == largeValue);
}

// ============================================================================
// Position Getter/Setter Tests (6-15)
// ============================================================================

TEST_CASE("getPosition returns correct position", "[position]") {
    QVector3D testPos(10, 20, 30);
    AstronomicalBody body(testPos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE(body.getPosition() == testPos);
}

TEST_CASE("Position x component", "[position]") {
    QVector3D pos(123.456, 0, 0);
    AstronomicalBody body(pos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE_THAT(body.getPosition().x(), WithinRel(123.456, 1e-5));
}

TEST_CASE("Position y component", "[position]") {
    QVector3D pos(0, 789.012, 0);
    AstronomicalBody body(pos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE_THAT(body.getPosition().y(), WithinRel(789.012, 1e-5));
}

TEST_CASE("Position z component", "[position]") {
    QVector3D pos(0, 0, 345.678);
    AstronomicalBody body(pos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE_THAT(body.getPosition().z(), WithinRel(345.678, 1e-5));
}

TEST_CASE("Position magnitude calculation", "[position]") {
    QVector3D pos(3, 4, 0);  // magnitude should be 5
    AstronomicalBody body(pos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE_THAT(body.getPosition().length(), WithinRel(5.0, 1e-5));
}

TEST_CASE("Position is independent for different bodies", "[position]") {
    AstronomicalBody body1(QVector3D(1, 2, 3), QVector3D(), 100, 10, Qt::white);
    AstronomicalBody body2(QVector3D(4, 5, 6), QVector3D(), 100, 10, Qt::white);
    
    REQUIRE(body1.getPosition() != body2.getPosition());
}

TEST_CASE("Position with fractional coordinates", "[position]") {
    QVector3D pos(1.5, 2.7, 3.9);
    AstronomicalBody body(pos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE_THAT(body.getPosition().x(), WithinRel(1.5, 1e-5));
    REQUIRE_THAT(body.getPosition().y(), WithinRel(2.7, 1e-5));
    REQUIRE_THAT(body.getPosition().z(), WithinRel(3.9, 1e-5));
}

// ============================================================================
// Velocity Getter Tests (16-25)
// ============================================================================

TEST_CASE("getVelocity returns correct velocity", "[velocity]") {
    QVector3D testVel(5, 10, 15);
    AstronomicalBody body(QVector3D(), testVel, 100, 10, Qt::white);
    
    REQUIRE(body.getVelocity() == testVel);
}

TEST_CASE("Velocity x component", "[velocity]") {
    QVector3D vel(11.111, 0, 0);
    AstronomicalBody body(QVector3D(), vel, 100, 10, Qt::white);
    
    REQUIRE_THAT(body.getVelocity().x(), WithinRel(11.111, 1e-4));
}

TEST_CASE("Velocity y component", "[velocity]") {
    QVector3D vel(0, 22.222, 0);
    AstronomicalBody body(QVector3D(), vel, 100, 10, Qt::white);
    
    REQUIRE_THAT(body.getVelocity().y(), WithinRel(22.222, 1e-4));
}

TEST_CASE("Velocity z component", "[velocity]") {
    QVector3D vel(0, 0, 33.333);
    AstronomicalBody body(QVector3D(), vel, 100, 10, Qt::white);
    
    REQUIRE_THAT(body.getVelocity().z(), WithinRel(33.333, 1e-4));
}

TEST_CASE("Velocity magnitude", "[velocity]") {
    QVector3D vel(3, 4, 0);  // magnitude should be 5
    AstronomicalBody body(QVector3D(), vel, 100, 10, Qt::white);
    
    REQUIRE_THAT(body.getVelocity().length(), WithinRel(5.0, 1e-5));
}

TEST_CASE("Negative velocity components", "[velocity]") {
    QVector3D vel(-7, -8, -9);
    AstronomicalBody body(QVector3D(), vel, 100, 10, Qt::white);
    
    REQUIRE(body.getVelocity() == vel);
}

TEST_CASE("Zero velocity", "[velocity]") {
    AstronomicalBody body(QVector3D(), QVector3D(0, 0, 0), 100, 10, Qt::white);
    
    REQUIRE(body.getVelocity().length() == 0.0);
}

// ============================================================================
// Mass Getter Tests (26-32)
// ============================================================================

TEST_CASE("getMass returns correct mass", "[mass]") {
    double testMass = 2500.0;
    AstronomicalBody body(QVector3D(), QVector3D(), testMass, 10, Qt::white);
    
    REQUIRE(body.getMass() == testMass);
}

TEST_CASE("Mass of sun", "[mass]") {
    AstronomicalBody sun(QVector3D(), QVector3D(), SolarSimConstants::SUN_MASS, 40, Qt::yellow);
    
    REQUIRE(sun.getMass() == SolarSimConstants::SUN_MASS);
}

TEST_CASE("Mass of planet", "[mass]") {
    AstronomicalBody planet(QVector3D(), QVector3D(), SolarSimConstants::PLANET_MASS, 20, Qt::blue);
    
    REQUIRE(planet.getMass() == SolarSimConstants::PLANET_MASS);
}

TEST_CASE("Very small mass", "[mass]") {
    AstronomicalBody body(QVector3D(), QVector3D(), 0.001, 1, Qt::white);
    
    REQUIRE(body.getMass() == 0.001);
}

TEST_CASE("Very large mass", "[mass]") {
    AstronomicalBody body(QVector3D(), QVector3D(), 1e15, 100, Qt::white);
    
    REQUIRE(body.getMass() == 1e15);
}

// ============================================================================
// Radius Getter Tests (33-38)
// ============================================================================

TEST_CASE("getRadius returns correct radius", "[radius]") {
    double testRadius = 75.5;
    AstronomicalBody body(QVector3D(), QVector3D(), 1000, testRadius, Qt::white);
    
    REQUIRE(body.getRadius() == testRadius);
}

TEST_CASE("Radius of sun", "[radius]") {
    AstronomicalBody sun(QVector3D(), QVector3D(), 1000000, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    
    REQUIRE(sun.getRadius() == SolarSimConstants::SUN_RADIUS);
}

TEST_CASE("Radius of planet", "[radius]") {
    AstronomicalBody planet(QVector3D(), QVector3D(), 1000, SolarSimConstants::PLANET_RADIUS, Qt::blue);
    
    REQUIRE(planet.getRadius() == SolarSimConstants::PLANET_RADIUS);
}

TEST_CASE("Very small radius", "[radius]") {
    AstronomicalBody body(QVector3D(), QVector3D(), 100, 0.1, Qt::white);
    
    REQUIRE(body.getRadius() == 0.1);
}

TEST_CASE("Very large radius", "[radius]") {
    AstronomicalBody body(QVector3D(), QVector3D(), 1e20, 1000000, Qt::white);
    
    REQUIRE(body.getRadius() == 1000000);
}

// ============================================================================
// Color Getter Tests (39-45)
// ============================================================================

TEST_CASE("getColor returns correct color - Blue", "[color]") {
    AstronomicalBody body(QVector3D(), QVector3D(), 100, 10, Qt::blue);
    
    REQUIRE(body.getColor() == Qt::blue);
}

TEST_CASE("getColor returns correct color - Yellow", "[color]") {
    AstronomicalBody body(QVector3D(), QVector3D(), 100, 10, Qt::yellow);
    
    REQUIRE(body.getColor() == Qt::yellow);
}

TEST_CASE("getColor returns correct color - Red", "[color]") {
    AstronomicalBody body(QVector3D(), QVector3D(), 100, 10, Qt::red);
    
    REQUIRE(body.getColor() == Qt::red);
}

TEST_CASE("getColor returns correct color - Green", "[color]") {
    AstronomicalBody body(QVector3D(), QVector3D(), 100, 10, Qt::green);
    
    REQUIRE(body.getColor() == Qt::green);
}

TEST_CASE("getColor with custom color", "[color]") {
    QColor custom(255, 128, 64);
    AstronomicalBody body(QVector3D(), QVector3D(), 100, 10, custom);
    
    REQUIRE(body.getColor() == custom);
}

// ============================================================================
// SolarSimConstants Tests (46-52)
// ============================================================================

TEST_CASE("Gravitational constant is positive", "[constants]") {
    REQUIRE(SolarSimConstants::G > 0);
}

TEST_CASE("Sun mass is greater than planet mass", "[constants]") {
    REQUIRE(SolarSimConstants::SUN_MASS > SolarSimConstants::PLANET_MASS);
}

TEST_CASE("Sun radius is greater than planet radius", "[constants]") {
    REQUIRE(SolarSimConstants::SUN_RADIUS > SolarSimConstants::PLANET_RADIUS);
}

TEST_CASE("Window width is positive", "[constants]") {
    REQUIRE(SolarSimConstants::WINDOW_WIDTH > 0);
    REQUIRE(SolarSimConstants::WINDOW_WIDTH == 1024);
}

TEST_CASE("Window height is positive", "[constants]") {
    REQUIRE(SolarSimConstants::WINDOW_HEIGHT > 0);
    REQUIRE(SolarSimConstants::WINDOW_HEIGHT == 768);
}

TEST_CASE("Steps per frame is positive", "[constants]") {
    REQUIRE(SolarSimConstants::STEPS_PER_FRAME > 0);
}

TEST_CASE("Time step is positive", "[constants]") {
    REQUIRE(SolarSimConstants::TIME_STEP > 0);
}

// ============================================================================
// Physics-related Tests (53-56)
// ============================================================================

TEST_CASE("Distance calculation between two bodies", "[physics]") {
    AstronomicalBody body1(QVector3D(0, 0, 0), QVector3D(), 1000, 10, Qt::white);
    AstronomicalBody body2(QVector3D(3, 4, 0), QVector3D(), 1000, 10, Qt::white);
    
    double distance = (body2.getPosition() - body1.getPosition()).length();
    REQUIRE_THAT(distance, WithinRel(5.0, 1e-5));
}

TEST_CASE("Same position means zero distance", "[physics]") {
    QVector3D pos(100, 200, 300);
    AstronomicalBody body1(pos, QVector3D(), 1000, 10, Qt::white);
    AstronomicalBody body2(pos, QVector3D(), 1000, 10, Qt::white);
    
    double distance = (body2.getPosition() - body1.getPosition()).length();
    REQUIRE(distance == 0.0);
}

TEST_CASE("Escape velocity calculation feasibility", "[physics]") {
    AstronomicalBody body(QVector3D(), QVector3D(), SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    
    // v_escape = sqrt(2*G*M/r)
    double escape_v = std::sqrt(2 * SolarSimConstants::G * body.getMass() / body.getRadius());
    REQUIRE(escape_v > 0);
    REQUIRE(escape_v < 1000); // reasonable bound
}

TEST_CASE("Orbital velocity calculation feasibility", "[physics]") {
    AstronomicalBody sun(QVector3D(), QVector3D(), SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    AstronomicalBody planet(QVector3D(500, 0, 0), QVector3D(), SolarSimConstants::PLANET_MASS, SolarSimConstants::PLANET_RADIUS, Qt::blue);
    
    double r = (planet.getPosition() - sun.getPosition()).length();
    double orbital_v = std::sqrt(SolarSimConstants::G * sun.getMass() / r);
    REQUIRE(orbital_v > 0);
    REQUIRE(orbital_v < 100); // reasonable bound for simulation
}
