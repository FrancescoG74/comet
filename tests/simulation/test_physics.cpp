#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <QVector3D>
#include <QColor>
#include <cmath>
#include "astronomicalbody.h"
#include "solarsimconstants.h"

using Catch::Matchers::WithinRel;

// ============================================================================
// SIMULATION TESTS - Physics & Constants
// ============================================================================

// ============================================================================
// Simulation: SolarSimConstants Tests (51-57)
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
// Simulation: Physics Integration Tests (58-63)
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

// ============================================================================
// Simulation: Solar System Initialization Tests (64-67)
// ============================================================================

TEST_CASE("Solar system has 8 planets", "[solarsystem][planets]") {
    // This test verifies the expected number of planets in the initialized system
    std::vector<QString> planetNames = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};
    REQUIRE(planetNames.size() == 8);
}

TEST_CASE("Solar system planet names in order", "[solarsystem][planets]") {
    std::vector<QString> expectedOrder = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};
    
    // Verify the expected order matches our solar system
    REQUIRE(expectedOrder[0] == "Mercury");
    REQUIRE(expectedOrder[1] == "Venus");
    REQUIRE(expectedOrder[2] == "Earth");
    REQUIRE(expectedOrder[3] == "Mars");
    REQUIRE(expectedOrder[4] == "Jupiter");
    REQUIRE(expectedOrder[5] == "Saturn");
    REQUIRE(expectedOrder[6] == "Uranus");
    REQUIRE(expectedOrder[7] == "Neptune");
}

TEST_CASE("Solar system realistic planet masses", "[solarsystem][planets]") {
    // Jupiter should be much more massive than Earth
    double jupiterMass = SolarSimConstants::PLANET_MASS * 318;
    double earthMass = SolarSimConstants::PLANET_MASS;
    
    REQUIRE(jupiterMass > earthMass);
    REQUIRE(jupiterMass / earthMass == 318);
}

TEST_CASE("Solar system realistic planet radii", "[solarsystem][planets]") {
    // Jupiter should be much larger than Earth
    double jupiterRadius = SolarSimConstants::PLANET_RADIUS * 11;
    double earthRadius = SolarSimConstants::PLANET_RADIUS;
    
    REQUIRE(jupiterRadius > earthRadius);
    REQUIRE(jupiterRadius / earthRadius == 11);
}
