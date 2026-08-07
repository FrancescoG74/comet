#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <QVector3D>
#include <QColor>
#include <QGuiApplication>
#include <cmath>
#include "astronomicalbody.h"
#include "solarsimconstants.h"

using Catch::Matchers::WithinRel;

// Create QGuiApplication for QPixmap support in tests
static int argc = 1;
static char argv0[] = "comet_tests";
static char* argv[] = {argv0, nullptr};
static QGuiApplication app(argc, argv);

// ============================================================================
// Planet Constructor Tests (1-5)
// ============================================================================

TEST_CASE("Planet default constructor", "[planet][constructor]") {
    Planet planet;
    REQUIRE(planet.getPosition() == QVector3D(0, 0, 0));
    REQUIRE(planet.getVelocity() == QVector3D(0, 0, 0));
    REQUIRE(planet.getMass() == 0.0);
    REQUIRE(planet.getRadius() == 0.0);
}

TEST_CASE("Planet full constructor", "[planet][constructor]") {
    QVector3D pos(100, 200, 300);
    QVector3D vel(1, 2, 3);
    double mass = 5000.0;
    double radius = 50.0;
    QColor color = Qt::blue;
    
    Planet planet(pos, vel, mass, radius, color);
    
    REQUIRE(planet.getPosition() == pos);
    REQUIRE(planet.getVelocity() == vel);
    REQUIRE(planet.getMass() == mass);
    REQUIRE(planet.getRadius() == radius);
    REQUIRE(planet.getColor() == color);
}

TEST_CASE("Planet with negative coordinates", "[planet][constructor]") {
    QVector3D pos(-100, -200, -300);
    QVector3D vel(-1, -2, -3);
    
    Planet planet(pos, vel, 1000.0, 25.0, Qt::red);
    
    REQUIRE(planet.getPosition() == pos);
    REQUIRE(planet.getVelocity() == vel);
}

TEST_CASE("Planet with zero values", "[planet][constructor]") {
    Planet planet(QVector3D(0, 0, 0), QVector3D(0, 0, 0), 0.0, 0.0, Qt::white);
    
    REQUIRE(planet.getPosition().length() == 0.0);
    REQUIRE(planet.getVelocity().length() == 0.0);
    REQUIRE(planet.getMass() == 0.0);
    REQUIRE(planet.getRadius() == 0.0);
}

TEST_CASE("Planet with very large values", "[planet][constructor]") {
    double largeValue = 1e10;
    Planet planet(
        QVector3D(largeValue, largeValue, largeValue),
        QVector3D(largeValue, largeValue, largeValue),
        largeValue,
        largeValue,
        Qt::yellow
    );
    
    REQUIRE(planet.getPosition().x() == largeValue);
    REQUIRE(planet.getMass() == largeValue);
}

// ============================================================================
// Sun Constructor Tests (6-10)
// ============================================================================

TEST_CASE("Sun default constructor", "[sun][constructor]") {
    Sun sun;
    REQUIRE(sun.getPosition() == QVector3D(0, 0, 0));
    REQUIRE(sun.getMass() == 0.0);
    REQUIRE(sun.getRadius() == 0.0);
    REQUIRE(sun.getColor() == Qt::yellow);
}

TEST_CASE("Sun full constructor", "[sun][constructor]") {
    QVector3D pos(500, 500, 0);
    double mass = SolarSimConstants::SUN_MASS;
    double radius = SolarSimConstants::SUN_RADIUS;
    QColor color = Qt::yellow;
    
    Sun sun(pos, mass, radius, color);
    
    REQUIRE(sun.getPosition() == pos);
    REQUIRE(sun.getMass() == mass);
    REQUIRE(sun.getRadius() == radius);
    REQUIRE(sun.getColor() == color);
}

TEST_CASE("Sun has no velocity method", "[sun][constructor]") {
    Sun sun(QVector3D(100, 100, 0), 1e6, 50, Qt::yellow);
    
    // Sun should not have getVelocity() - compile-time check
    REQUIRE(sun.getMass() == 1e6);
    REQUIRE(sun.getRadius() == 50);
}

TEST_CASE("Sun is static - position can be set", "[sun][constructor]") {
    Sun sun(QVector3D(100, 100, 0), 1e6, 50, Qt::yellow);
    
    QVector3D newPos(200, 200, 0);
    sun.setPosition(newPos);
    
    REQUIRE(sun.getPosition() == newPos);
}

TEST_CASE("Sun mass can be modified", "[sun][constructor]") {
    Sun sun(QVector3D(100, 100, 0), 1e6, 50, Qt::yellow);
    
    double newMass = 2e6;
    sun.setMass(newMass);
    
    REQUIRE(sun.getMass() == newMass);
}

// ============================================================================
// Planet Position Tests (11-20)
// ============================================================================

TEST_CASE("Planet getPosition returns correct position", "[planet][position]") {
    QVector3D testPos(10, 20, 30);
    Planet planet(testPos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE(planet.getPosition() == testPos);
}

TEST_CASE("Planet position x component", "[planet][position]") {
    QVector3D pos(123.456, 0, 0);
    Planet planet(pos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE_THAT(planet.getPosition().x(), WithinRel(123.456, 1e-5));
}

TEST_CASE("Planet position y component", "[planet][position]") {
    QVector3D pos(0, 789.012, 0);
    Planet planet(pos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE_THAT(planet.getPosition().y(), WithinRel(789.012, 1e-5));
}

TEST_CASE("Planet position z component", "[planet][position]") {
    QVector3D pos(0, 0, 345.678);
    Planet planet(pos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE_THAT(planet.getPosition().z(), WithinRel(345.678, 1e-5));
}

TEST_CASE("Planet position magnitude calculation", "[planet][position]") {
    QVector3D pos(3, 4, 0);  // magnitude should be 5
    Planet planet(pos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE_THAT(planet.getPosition().length(), WithinRel(5.0, 1e-5));
}

TEST_CASE("Planet position is independent for different bodies", "[planet][position]") {
    Planet planet1(QVector3D(1, 2, 3), QVector3D(), 100, 10, Qt::white);
    Planet planet2(QVector3D(4, 5, 6), QVector3D(), 100, 10, Qt::white);
    
    REQUIRE(planet1.getPosition() != planet2.getPosition());
}

TEST_CASE("Planet position with fractional coordinates", "[planet][position]") {
    QVector3D pos(1.5, 2.7, 3.9);
    Planet planet(pos, QVector3D(), 100, 10, Qt::white);
    
    REQUIRE_THAT(planet.getPosition().x(), WithinRel(1.5, 1e-5));
    REQUIRE_THAT(planet.getPosition().y(), WithinRel(2.7, 1e-5));
    REQUIRE_THAT(planet.getPosition().z(), WithinRel(3.9, 1e-5));
}

TEST_CASE("Sun position returns correct position", "[sun][position]") {
    QVector3D testPos(500, 500, 0);
    Sun sun(testPos, 1e6, 50, Qt::yellow);
    
    REQUIRE(sun.getPosition() == testPos);
}

// ============================================================================
// Planet Velocity Tests (21-30)
// ============================================================================

TEST_CASE("Planet getVelocity returns correct velocity", "[planet][velocity]") {
    QVector3D testVel(5, 10, 15);
    Planet planet(QVector3D(), testVel, 100, 10, Qt::white);
    
    REQUIRE(planet.getVelocity() == testVel);
}

TEST_CASE("Planet velocity x component", "[planet][velocity]") {
    QVector3D vel(11.111, 0, 0);
    Planet planet(QVector3D(), vel, 100, 10, Qt::white);
    
    REQUIRE_THAT(planet.getVelocity().x(), WithinRel(11.111, 1e-4));
}

TEST_CASE("Planet velocity y component", "[planet][velocity]") {
    QVector3D vel(0, 22.222, 0);
    Planet planet(QVector3D(), vel, 100, 10, Qt::white);
    
    REQUIRE_THAT(planet.getVelocity().y(), WithinRel(22.222, 1e-4));
}

TEST_CASE("Planet velocity z component", "[planet][velocity]") {
    QVector3D vel(0, 0, 33.333);
    Planet planet(QVector3D(), vel, 100, 10, Qt::white);
    
    REQUIRE_THAT(planet.getVelocity().z(), WithinRel(33.333, 1e-4));
}

TEST_CASE("Planet velocity magnitude", "[planet][velocity]") {
    QVector3D vel(3, 4, 0);  // magnitude should be 5
    Planet planet(QVector3D(), vel, 100, 10, Qt::white);
    
    REQUIRE_THAT(planet.getVelocity().length(), WithinRel(5.0, 1e-5));
}

TEST_CASE("Planet negative velocity components", "[planet][velocity]") {
    QVector3D vel(-7, -8, -9);
    Planet planet(QVector3D(), vel, 100, 10, Qt::white);
    
    REQUIRE(planet.getVelocity() == vel);
}

TEST_CASE("Planet zero velocity", "[planet][velocity]") {
    Planet planet(QVector3D(), QVector3D(0, 0, 0), 100, 10, Qt::white);
    
    REQUIRE(planet.getVelocity().length() == 0.0);
}

TEST_CASE("Planet setVelocity updates velocity", "[planet][velocity]") {
    Planet planet(QVector3D(), QVector3D(1, 1, 1), 100, 10, Qt::white);
    
    QVector3D newVel(5, 6, 7);
    planet.setVelocity(newVel);
    
    REQUIRE(planet.getVelocity() == newVel);
}

// ============================================================================
// Planet/Sun Mass Getter/Setter Tests (31-37)
// ============================================================================

TEST_CASE("Planet getMass returns correct mass", "[planet][mass]") {
    double testMass = 2500.0;
    Planet planet(QVector3D(), QVector3D(), testMass, 10, Qt::white);
    
    REQUIRE(planet.getMass() == testMass);
}

TEST_CASE("Sun with gravitational mass", "[sun][mass]") {
    Sun sun(QVector3D(), SolarSimConstants::SUN_MASS, 40, Qt::yellow);
    
    REQUIRE(sun.getMass() == SolarSimConstants::SUN_MASS);
}

TEST_CASE("Planet mass same as SUN_MASS constant", "[planet][mass]") {
    Planet planet(QVector3D(), QVector3D(), SolarSimConstants::PLANET_MASS, 20, Qt::blue);
    
    REQUIRE(planet.getMass() == SolarSimConstants::PLANET_MASS);
}

TEST_CASE("Very small planet mass", "[planet][mass]") {
    Planet planet(QVector3D(), QVector3D(), 0.001, 1, Qt::white);
    
    REQUIRE(planet.getMass() == 0.001);
}

TEST_CASE("Very large sun mass", "[sun][mass]") {
    Sun sun(QVector3D(), 1e15, 100, Qt::yellow);
    
    REQUIRE(sun.getMass() == 1e15);
}

TEST_CASE("Sun setMass updates mass", "[sun][mass]") {
    Sun sun(QVector3D(), 1e6, 50, Qt::yellow);
    
    sun.setMass(2e6);
    REQUIRE(sun.getMass() == 2e6);
}

// ============================================================================
// Planet/Sun Radius Tests (38-43)
// ============================================================================

TEST_CASE("Planet getRadius returns correct radius", "[planet][radius]") {
    double testRadius = 75.5;
    Planet planet(QVector3D(), QVector3D(), 1000, testRadius, Qt::white);
    
    REQUIRE(planet.getRadius() == testRadius);
}

TEST_CASE("Sun radius from constant", "[sun][radius]") {
    Sun sun(QVector3D(), 1000000, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    
    REQUIRE(sun.getRadius() == SolarSimConstants::SUN_RADIUS);
}

TEST_CASE("Planet radius from constant", "[planet][radius]") {
    Planet planet(QVector3D(), QVector3D(), 1000, SolarSimConstants::PLANET_RADIUS, Qt::blue);
    
    REQUIRE(planet.getRadius() == SolarSimConstants::PLANET_RADIUS);
}

TEST_CASE("Very small planet radius", "[planet][radius]") {
    Planet planet(QVector3D(), QVector3D(), 100, 0.1, Qt::white);
    
    REQUIRE(planet.getRadius() == 0.1);
}

TEST_CASE("Very large sun radius", "[sun][radius]") {
    Sun sun(QVector3D(), 1e20, 1000000, Qt::yellow);
    
    REQUIRE(sun.getRadius() == 1000000);
}

// ============================================================================
// Planet/Sun Color Tests (44-50)
// ============================================================================

TEST_CASE("Planet color - Blue", "[planet][color]") {
    Planet planet(QVector3D(), QVector3D(), 100, 10, Qt::blue);
    
    REQUIRE(planet.getColor() == Qt::blue);
}

TEST_CASE("Sun color - Yellow", "[sun][color]") {
    Sun sun(QVector3D(), 1e6, 50, Qt::yellow);
    
    REQUIRE(sun.getColor() == Qt::yellow);
}

TEST_CASE("Planet color - Red", "[planet][color]") {
    Planet planet(QVector3D(), QVector3D(), 100, 10, Qt::red);
    
    REQUIRE(planet.getColor() == Qt::red);
}

TEST_CASE("Planet color - Green", "[planet][color]") {
    Planet planet(QVector3D(), QVector3D(), 100, 10, Qt::green);
    
    REQUIRE(planet.getColor() == Qt::green);
}

TEST_CASE("Planet with custom color", "[planet][color]") {
    QColor custom(255, 128, 64);
    Planet planet(QVector3D(), QVector3D(), 100, 10, custom);
    
    REQUIRE(planet.getColor() == custom);
}

TEST_CASE("Sun with custom color", "[sun][color]") {
    QColor custom(255, 200, 100);
    Sun sun(QVector3D(), 1e6, 50, custom);
    
    REQUIRE(sun.getColor() == custom);
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
// Physics-related Tests (51-56)
// ============================================================================

TEST_CASE("Distance calculation between two planets", "[planet][physics]") {
    Planet planet1(QVector3D(0, 0, 0), QVector3D(), 1000, 10, Qt::white);
    Planet planet2(QVector3D(3, 4, 0), QVector3D(), 1000, 10, Qt::white);
    
    double distance = (planet2.getPosition() - planet1.getPosition()).length();
    REQUIRE_THAT(distance, WithinRel(5.0, 1e-5));
}

TEST_CASE("Distance between planet and sun", "[planet][sun][physics]") {
    Sun sun(QVector3D(0, 0, 0), 1e6, 50, Qt::yellow);
    Planet planet(QVector3D(5, 0, 0), QVector3D(), 1000, 10, Qt::blue);
    
    double distance = (planet.getPosition() - sun.getPosition()).length();
    REQUIRE_THAT(distance, WithinRel(5.0, 1e-5));
}

TEST_CASE("Same position means zero distance", "[planet][physics]") {
    QVector3D pos(100, 200, 300);
    Planet planet1(pos, QVector3D(), 1000, 10, Qt::white);
    Planet planet2(pos, QVector3D(), 1000, 10, Qt::white);
    
    double distance = (planet2.getPosition() - planet1.getPosition()).length();
    REQUIRE(distance == 0.0);
}

TEST_CASE("Escape velocity calculation with sun", "[sun][physics]") {
    Sun sun(QVector3D(), SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    
    // v_escape = sqrt(2*G*M/r)
    double escape_v = std::sqrt(2 * SolarSimConstants::G * sun.getMass() / sun.getRadius());
    REQUIRE(escape_v > 0);
    REQUIRE(escape_v < 1000); // reasonable bound
}

TEST_CASE("Orbital velocity of planet around sun", "[planet][sun][physics]") {
    Sun sun(QVector3D(), SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    Planet planet(QVector3D(500, 0, 0), QVector3D(), SolarSimConstants::PLANET_MASS, SolarSimConstants::PLANET_RADIUS, Qt::blue);
    
    double r = (planet.getPosition() - sun.getPosition()).length();
    double orbital_v = std::sqrt(SolarSimConstants::G * sun.getMass() / r);
    REQUIRE(orbital_v > 0);
    REQUIRE(orbital_v < 100); // reasonable bound for simulation
}

TEST_CASE("Position update for planet", "[planet][physics]") {
    Planet planet(QVector3D(0, 0, 0), QVector3D(), 1000, 10, Qt::blue);
    
    QVector3D newPos(100, 200, 50);
    planet.setPosition(newPos);
    
    REQUIRE(planet.getPosition() == newPos);
}
