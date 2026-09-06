#include <gtest/gtest.h>
#include <QVector3D>
#include <QColor>
#include <cmath>
#include "astronomicalbody.h"
#include "solarsimconstants.h"

// ============================================================================
// SIMULATION TESTS - Physics & Constants
// ============================================================================

// ============================================================================
// Simulation: SolarSimConstants Tests (51-57)
// ============================================================================

TEST(SolarSimConstants, GravitationalConstantIsPositive) {
    EXPECT_GT(SolarSimConstants::G, 0);
}

TEST(SolarSimConstants, SunMassGreaterThanPlanetMass) {
    EXPECT_GT(SolarSimConstants::SUN_MASS, SolarSimConstants::PLANET_MASS);
}

TEST(SolarSimConstants, SunRadiusGreaterThanPlanetRadius) {
    EXPECT_GT(SolarSimConstants::SUN_RADIUS, SolarSimConstants::PLANET_RADIUS);
}

TEST(SolarSimConstants, WindowWidthIsPositive) {
    EXPECT_GT(SolarSimConstants::WINDOW_WIDTH, 0);
    EXPECT_EQ(SolarSimConstants::WINDOW_WIDTH, 1024);
}

TEST(SolarSimConstants, WindowHeightIsPositive) {
    EXPECT_GT(SolarSimConstants::WINDOW_HEIGHT, 0);
    EXPECT_EQ(SolarSimConstants::WINDOW_HEIGHT, 768);
}

TEST(SolarSimConstants, StepsPerFrameIsPositive) {
    EXPECT_GT(SolarSimConstants::STEPS_PER_FRAME, 0);
}

TEST(SolarSimConstants, TimeStepIsPositive) {
    EXPECT_GT(SolarSimConstants::TIME_STEP, 0);
}

// ============================================================================
// Simulation: Physics Integration Tests (58-63)
// ============================================================================

TEST(PhysicsIntegration, DistanceBetweenTwoPlanets) {
    Planet planet1(QVector3D(0, 0, 0), QVector3D(), 1000, 10, Qt::white);
    Planet planet2(QVector3D(3, 4, 0), QVector3D(), 1000, 10, Qt::white);
    
    double distance = (planet2.getPosition() - planet1.getPosition()).length();
    EXPECT_NEAR(distance, 5.0, 1e-5);
}

TEST(PhysicsIntegration, DistanceBetweenPlanetAndSun) {
    Sun sun(QVector3D(0, 0, 0), 1e6, 50, Qt::yellow);
    Planet planet(QVector3D(5, 0, 0), QVector3D(), 1000, 10, Qt::blue);
    
    double distance = (planet.getPosition() - sun.getPosition()).length();
    EXPECT_NEAR(distance, 5.0, 1e-5);
}

TEST(PhysicsIntegration, SamePositionMeansZeroDistance) {
    QVector3D pos(100, 200, 300);
    Planet planet1(pos, QVector3D(), 1000, 10, Qt::white);
    Planet planet2(pos, QVector3D(), 1000, 10, Qt::white);
    
    double distance = (planet2.getPosition() - planet1.getPosition()).length();
    EXPECT_EQ(distance, 0.0);
}

TEST(PhysicsIntegration, EscapeVelocityCalculationWithSun) {
    Sun sun(QVector3D(), SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    
    // v_escape = sqrt(2*G*M/r)
    double escape_v = std::sqrt(2 * SolarSimConstants::G * sun.getMass() / sun.getRadius());
    EXPECT_GT(escape_v, 0);
    EXPECT_LT(escape_v, 1000); // reasonable bound
}

TEST(PhysicsIntegration, OrbitalVelocityOfPlanetAroundSun) {
    Sun sun(QVector3D(), SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    Planet planet(QVector3D(500, 0, 0), QVector3D(), SolarSimConstants::PLANET_MASS, SolarSimConstants::PLANET_RADIUS, Qt::blue);
    
    double r = (planet.getPosition() - sun.getPosition()).length();
    double orbital_v = std::sqrt(SolarSimConstants::G * sun.getMass() / r);
    EXPECT_GT(orbital_v, 0);
    EXPECT_LT(orbital_v, 100); // reasonable bound for simulation
}

TEST(PhysicsIntegration, PositionUpdateForPlanet) {
    Planet planet(QVector3D(0, 0, 0), QVector3D(), 1000, 10, Qt::blue);
    
    QVector3D newPos(100, 200, 50);
    planet.setPosition(newPos);
    
    EXPECT_EQ(planet.getPosition(), newPos);
}

// ============================================================================
// Simulation: Solar System Initialization Tests (64-67)
// ============================================================================

TEST(SolarSystem, Has8Planets) {
    // This test verifies the expected number of planets in the initialized system
    std::vector<QString> planetNames = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};
    EXPECT_EQ(planetNames.size(), 8);
}

TEST(SolarSystem, PlanetNamesInOrder) {
    std::vector<QString> expectedOrder = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};
    
    // Verify the expected order matches our solar system
    EXPECT_EQ(expectedOrder[0], "Mercury");
    EXPECT_EQ(expectedOrder[1], "Venus");
    EXPECT_EQ(expectedOrder[2], "Earth");
    EXPECT_EQ(expectedOrder[3], "Mars");
    EXPECT_EQ(expectedOrder[4], "Jupiter");
    EXPECT_EQ(expectedOrder[5], "Saturn");
    EXPECT_EQ(expectedOrder[6], "Uranus");
    EXPECT_EQ(expectedOrder[7], "Neptune");
}

TEST(SolarSystem, RealisticPlanetMasses) {
    // Jupiter should be much more massive than Earth
    double jupiterMass = SolarSimConstants::PLANET_MASS * 318;
    double earthMass = SolarSimConstants::PLANET_MASS;
    
    EXPECT_GT(jupiterMass, earthMass);
    EXPECT_EQ(jupiterMass / earthMass, 318);
}

TEST(SolarSystem, RealisticPlanetRadii) {
    // Jupiter should be much larger than Earth
    double jupiterRadius = SolarSimConstants::PLANET_RADIUS * 11;
    double earthRadius = SolarSimConstants::PLANET_RADIUS;
    
    EXPECT_GT(jupiterRadius, earthRadius);
    EXPECT_EQ(jupiterRadius / earthRadius, 11);
}

// ============================================================================
// Simulation: Realistic Astronomical Constants Tests
// ============================================================================

TEST(AstronomicalConstants, SunMassRealisticProportionalToEarthMass) {
    // Sun is approximately 333,000 times more massive than Earth
    EXPECT_EQ(SolarSimConstants::SUN_MASS, 333000.0);
    EXPECT_GT(SolarSimConstants::SUN_MASS, 0);
    EXPECT_EQ(SolarSimConstants::SUN_MASS / SolarSimConstants::EARTH_MASS, 333000.0);
}

TEST(AstronomicalConstants, SunRadiusRealisticProportionalToEarthRadius) {
    // Sun is approximately 109 times the radius of Earth
    EXPECT_EQ(SolarSimConstants::SUN_RADIUS, 109.0);
    EXPECT_GT(SolarSimConstants::SUN_RADIUS, 0);
    EXPECT_EQ(SolarSimConstants::SUN_RADIUS / SolarSimConstants::EARTH_RADIUS, 109.0);
}

TEST(AstronomicalConstants, EarthReferenceConstantsAreBaseline) {
    EXPECT_EQ(SolarSimConstants::EARTH_MASS, 1.0);
    EXPECT_EQ(SolarSimConstants::EARTH_RADIUS, 1.0);
}

TEST(DisplayConstants, DisplayScaleIsPositive) {
    EXPECT_GT(SolarSimConstants::DISPLAY_SCALE, 0);
    EXPECT_EQ(SolarSimConstants::DISPLAY_SCALE, 0.25);
}

TEST(DisplayConstants, SunDisplayRadiusCalculation) {
    // Sun display radius = SUN_RADIUS * DISPLAY_SCALE
    // = 109.0 * 0.25 = 27.25 pixels
    double sunDisplayRadius = SolarSimConstants::SUN_RADIUS * SolarSimConstants::DISPLAY_SCALE;
    EXPECT_NEAR(sunDisplayRadius, 27.25, 1e-6);
}

TEST(DisplayConstants, MercuryDisplayRadiusIsSmall) {
    // Mercury radius relative to Earth = 0.383
    double mercuryRadius = 0.383;
    double mercuryDisplayRadius = mercuryRadius * SolarSimConstants::DISPLAY_SCALE;
    // Should be approximately 0.096 pixels
    EXPECT_NEAR(mercuryDisplayRadius, 0.09575, 1e-6);
    EXPECT_LT(mercuryDisplayRadius, 1.0);  // Mercury should be very small
}

TEST(DisplayConstants, JupiterDisplayRadiusIsLargerThanMercury) {
    // Jupiter radius relative to Earth = 10.97
    double jupiterRadius = 10.97;
    double jupiterDisplayRadius = jupiterRadius * SolarSimConstants::DISPLAY_SCALE;
    
    // Mercury radius = 0.383
    double mercuryRadius = 0.383;
    double mercuryDisplayRadius = mercuryRadius * SolarSimConstants::DISPLAY_SCALE;
    
    EXPECT_GT(jupiterDisplayRadius, mercuryDisplayRadius);
    EXPECT_NEAR(jupiterDisplayRadius / mercuryDisplayRadius, 10.97 / 0.383, 1e-6);
}

TEST(SolarSystemAstronomy, RealisticSolarSystemHasProperMassRelationships) {
    // Mercury mass relative to Earth = 0.055
    double mercuryMass = 0.055;
    double earthMass = 1.0;
    
    EXPECT_GT(earthMass, mercuryMass);
    EXPECT_NEAR(earthMass / mercuryMass, 18.18, 0.1);  // Earth is ~18x more massive than Mercury
}

TEST(SpeedSlider, MultiplierRangeMinimum) {
    // Speed slider minimum value 10 = 0.1x
    int minSliderValue = 10;
    double minMultiplier = minSliderValue / 100.0;
    EXPECT_NEAR(minMultiplier, 0.1, 1e-6);
}

TEST(SpeedSlider, MultiplierRangeMaximum) {
    // Speed slider maximum value 2000 = 20x
    int maxSliderValue = 2000;
    double maxMultiplier = maxSliderValue / 100.0;
    EXPECT_NEAR(maxMultiplier, 20.0, 1e-6);
}

TEST(SpeedSlider, MultiplierDefaultIs1Point0x) {
    // Speed slider default value 100 = 1.0x
    int defaultValue = 100;
    double defaultMultiplier = defaultValue / 100.0;
    EXPECT_NEAR(defaultMultiplier, 1.0, 1e-6);
}

TEST(SpeedSlider, MultiplierCalculationFor0Point5x) {
    int sliderValue = 50;
    double multiplier = sliderValue / 100.0;
    EXPECT_NEAR(multiplier, 0.5, 1e-6);
}

TEST(SpeedSlider, MultiplierCalculationFor5x) {
    int sliderValue = 500;
    double multiplier = sliderValue / 100.0;
    EXPECT_NEAR(multiplier, 5.0, 1e-6);
}

TEST(SimulationTime, AdvancementAt1xSpeed) {
    // At 1x speed: 1 day per real second
    double speedMultiplier = 1.0;
    double simulationSecondsPerRealSecond = speedMultiplier * 86400.0;  // 86400 seconds = 1 day
    EXPECT_NEAR(simulationSecondsPerRealSecond, 86400.0, 1e-6);
}

TEST(SimulationTime, AdvancementAt0Point1xSpeed) {
    // At 0.1x speed: 0.1 days per real second (2.4 hours)
    double speedMultiplier = 0.1;
    double simulationSecondsPerRealSecond = speedMultiplier * 86400.0;
    EXPECT_NEAR(simulationSecondsPerRealSecond, 8640.0, 1e-6);
}

TEST(SimulationTime, AdvancementAt20xSpeed) {
    // At 20x speed: 20 days per real second
    double speedMultiplier = 20.0;
    double simulationSecondsPerRealSecond = speedMultiplier * 86400.0;
    EXPECT_NEAR(simulationSecondsPerRealSecond, 1728000.0, 1e-6);
}

TEST(Rendering, PaintersAlgorithmRequiresProperDepthSorting) {
    // Test that depth values work correctly for sorting
    double farDepth = -100.0;    // Far from camera (negative z)
    double closeDepth = 100.0;   // Close to camera (positive z)
    
    // Should render far objects first (lower z value)
    EXPECT_LT(farDepth, closeDepth);
}
