#include <gtest/gtest.h>
#include <QVector3D>
#include <QColor>
#include <cmath>
#include "astronomicalbody.h"

// ============================================================================
// CORE TESTS - AstronomicalBody Hierarchy
// ============================================================================

// ============================================================================
// Core: Planet Constructor Tests (1-5)
// ============================================================================

TEST(PlanetConstructor, DefaultConstructor) {
    Planet planet;
    EXPECT_EQ(planet.getPosition(), QVector3D(0, 0, 0));
    EXPECT_EQ(planet.getVelocity(), QVector3D(0, 0, 0));
    EXPECT_EQ(planet.getMass(), 0.0);
    EXPECT_EQ(planet.getRadius(), 0.0);
}

TEST(PlanetConstructor, FullConstructor) {
    QVector3D pos(100, 200, 300);
    QVector3D vel(1, 2, 3);
    double mass = 5000.0;
    double radius = 50.0;
    QColor color = Qt::blue;
    
    Planet planet(pos, vel, mass, radius, color);
    
    EXPECT_EQ(planet.getPosition(), pos);
    EXPECT_EQ(planet.getVelocity(), vel);
    EXPECT_EQ(planet.getMass(), mass);
    EXPECT_EQ(planet.getRadius(), radius);
    EXPECT_EQ(planet.getColor(), color);
}

TEST(PlanetConstructor, NegativeCoordinates) {
    QVector3D pos(-100, -200, -300);
    QVector3D vel(-1, -2, -3);
    
    Planet planet(pos, vel, 1000.0, 25.0, Qt::red);
    
    EXPECT_EQ(planet.getPosition(), pos);
    EXPECT_EQ(planet.getVelocity(), vel);
}

TEST(PlanetConstructor, ZeroValues) {
    Planet planet(QVector3D(0, 0, 0), QVector3D(0, 0, 0), 0.0, 0.0, Qt::white);
    
    EXPECT_EQ(planet.getPosition().length(), 0.0);
    EXPECT_EQ(planet.getVelocity().length(), 0.0);
    EXPECT_EQ(planet.getMass(), 0.0);
    EXPECT_EQ(planet.getRadius(), 0.0);
}

TEST(PlanetConstructor, VeryLargeValues) {
    double largeValue = 1e10;
    Planet planet(
        QVector3D(largeValue, largeValue, largeValue),
        QVector3D(largeValue, largeValue, largeValue),
        largeValue,
        largeValue,
        Qt::yellow
    );
    
    EXPECT_EQ(planet.getPosition().x(), largeValue);
    EXPECT_EQ(planet.getMass(), largeValue);
}

// ============================================================================
// Core: Sun Constructor Tests (6-10)
// ============================================================================

TEST(SunConstructor, DefaultConstructor) {
    Sun sun;
    EXPECT_EQ(sun.getPosition(), QVector3D(0, 0, 0));
    EXPECT_EQ(sun.getMass(), 0.0);
    EXPECT_EQ(sun.getRadius(), 0.0);
    EXPECT_EQ(sun.getColor(), Qt::yellow);
}

TEST(SunConstructor, FullConstructor) {
    QVector3D pos(500, 500, 0);
    double mass = 1000000.0;
    double radius = 40.0;
    QColor color = Qt::yellow;
    
    Sun sun(pos, mass, radius, color);
    
    EXPECT_EQ(sun.getPosition(), pos);
    EXPECT_EQ(sun.getMass(), mass);
    EXPECT_EQ(sun.getRadius(), radius);
    EXPECT_EQ(sun.getColor(), color);
}

TEST(SunConstructor, NoVelocityMethod) {
    Sun sun(QVector3D(100, 100, 0), 1e6, 50, Qt::yellow);
    
    // Sun should not have getVelocity() - compile-time check
    EXPECT_EQ(sun.getMass(), 1e6);
    EXPECT_EQ(sun.getRadius(), 50);
}

TEST(SunConstructor, PositionCanBeSet) {
    Sun sun(QVector3D(100, 100, 0), 1e6, 50, Qt::yellow);
    
    QVector3D newPos(200, 200, 0);
    sun.setPosition(newPos);
    
    EXPECT_EQ(sun.getPosition(), newPos);
}

TEST(SunConstructor, MassCanBeModified) {
    Sun sun(QVector3D(100, 100, 0), 1e6, 50, Qt::yellow);
    
    double newMass = 2e6;
    sun.setMass(newMass);
    
    EXPECT_EQ(sun.getMass(), newMass);
}

// ============================================================================
// Core: Planet Position Tests (11-20)
// ============================================================================

TEST(PlanetPosition, GetPositionReturnsCorrectPosition) {
    QVector3D testPos(10, 20, 30);
    Planet planet(testPos, QVector3D(), 100, 10, Qt::white);
    
    EXPECT_EQ(planet.getPosition(), testPos);
}

TEST(PlanetPosition, XComponent) {
    QVector3D pos(123.456, 0, 0);
    Planet planet(pos, QVector3D(), 100, 10, Qt::white);
    
    EXPECT_NEAR(planet.getPosition().x(), 123.456, 1e-5);
}

TEST(PlanetPosition, YComponent) {
    QVector3D pos(0, 789.012, 0);
    Planet planet(pos, QVector3D(), 100, 10, Qt::white);
    
    EXPECT_NEAR(planet.getPosition().y(), 789.012, 1e-4);
}

TEST(PlanetPosition, ZComponent) {
    QVector3D pos(0, 0, 345.678);
    Planet planet(pos, QVector3D(), 100, 10, Qt::white);
    
    EXPECT_NEAR(planet.getPosition().z(), 345.678, 1e-5);
}

TEST(PlanetPosition, MagnitudeCalculation) {
    QVector3D pos(3, 4, 0);  // magnitude should be 5
    Planet planet(pos, QVector3D(), 100, 10, Qt::white);
    
    EXPECT_NEAR(planet.getPosition().length(), 5.0, 1e-5);
}

TEST(PlanetPosition, IndependentPositions) {
    Planet planet1(QVector3D(1, 2, 3), QVector3D(), 100, 10, Qt::white);
    Planet planet2(QVector3D(4, 5, 6), QVector3D(), 100, 10, Qt::white);
    
    EXPECT_NE(planet1.getPosition(), planet2.getPosition());
}

TEST(PlanetPosition, FractionalCoordinates) {
    QVector3D pos(1.5, 2.7, 3.9);
    Planet planet(pos, QVector3D(), 100, 10, Qt::white);
    
    EXPECT_NEAR(planet.getPosition().x(), 1.5, 1e-5);
    EXPECT_NEAR(planet.getPosition().y(), 2.7, 1e-5);
    EXPECT_NEAR(planet.getPosition().z(), 3.9, 1e-5);
}

TEST(SunPosition, GetPositionReturnsCorrectPosition) {
    QVector3D testPos(500, 500, 0);
    Sun sun(testPos, 1e6, 50, Qt::yellow);
    
    EXPECT_EQ(sun.getPosition(), testPos);
}

// ============================================================================
// Core: Planet Velocity Tests (21-30)
// ============================================================================

TEST(PlanetVelocity, GetVelocityReturnsCorrectVelocity) {
    QVector3D testVel(5, 10, 15);
    Planet planet(QVector3D(), testVel, 100, 10, Qt::white);
    
    EXPECT_EQ(planet.getVelocity(), testVel);
}

TEST(PlanetVelocity, XComponent) {
    QVector3D vel(11.111, 0, 0);
    Planet planet(QVector3D(), vel, 100, 10, Qt::white);
    
    EXPECT_NEAR(planet.getVelocity().x(), 11.111, 1e-4);
}

TEST(PlanetVelocity, YComponent) {
    QVector3D vel(0, 22.222, 0);
    Planet planet(QVector3D(), vel, 100, 10, Qt::white);
    
    EXPECT_NEAR(planet.getVelocity().y(), 22.222, 1e-4);
}

TEST(PlanetVelocity, ZComponent) {
    QVector3D vel(0, 0, 33.333);
    Planet planet(QVector3D(), vel, 100, 10, Qt::white);
    
    EXPECT_NEAR(planet.getVelocity().z(), 33.333, 1e-4);
}

TEST(PlanetVelocity, Magnitude) {
    QVector3D vel(3, 4, 0);  // magnitude should be 5
    Planet planet(QVector3D(), vel, 100, 10, Qt::white);
    
    EXPECT_NEAR(planet.getVelocity().length(), 5.0, 1e-5);
}

TEST(PlanetVelocity, NegativeComponents) {
    QVector3D vel(-7, -8, -9);
    Planet planet(QVector3D(), vel, 100, 10, Qt::white);
    
    EXPECT_EQ(planet.getVelocity(), vel);
}

TEST(PlanetVelocity, ZeroVelocity) {
    Planet planet(QVector3D(), QVector3D(0, 0, 0), 100, 10, Qt::white);
    
    EXPECT_EQ(planet.getVelocity().length(), 0.0);
}

TEST(PlanetVelocity, SetVelocityUpdatesVelocity) {
    Planet planet(QVector3D(), QVector3D(1, 1, 1), 100, 10, Qt::white);
    
    QVector3D newVel(5, 6, 7);
    planet.setVelocity(newVel);
    
    EXPECT_EQ(planet.getVelocity(), newVel);
}

// ============================================================================
// Core: Planet/Sun Mass Getter/Setter Tests (31-37)
// ============================================================================

TEST(PlanetMass, GetMassReturnsCorrectMass) {
    double testMass = 2500.0;
    Planet planet(QVector3D(), QVector3D(), testMass, 10, Qt::white);
    
    EXPECT_EQ(planet.getMass(), testMass);
}

TEST(SunMass, WithGravitationalMass) {
    Sun sun(QVector3D(), 1000000.0, 40, Qt::yellow);
    
    EXPECT_EQ(sun.getMass(), 1000000.0);
}

TEST(PlanetMass, SameasSunMassConstant) {
    Planet planet(QVector3D(), QVector3D(), 1000.0, 20, Qt::blue);
    
    EXPECT_EQ(planet.getMass(), 1000.0);
}

TEST(PlanetMass, VerySmallMass) {
    Planet planet(QVector3D(), QVector3D(), 0.001, 1, Qt::white);
    
    EXPECT_EQ(planet.getMass(), 0.001);
}

TEST(SunMass, VeryLargeMass) {
    Sun sun(QVector3D(), 1e15, 100, Qt::yellow);
    
    EXPECT_EQ(sun.getMass(), 1e15);
}

TEST(SunMass, SetMassUpdatesMass) {
    Sun sun(QVector3D(), 1e6, 50, Qt::yellow);
    
    sun.setMass(2e6);
    EXPECT_EQ(sun.getMass(), 2e6);
}

// ============================================================================
// Core: Planet/Sun Radius Tests (38-43)
// ============================================================================

TEST(PlanetRadius, GetRadiusReturnsCorrectRadius) {
    double testRadius = 75.5;
    Planet planet(QVector3D(), QVector3D(), 1000, testRadius, Qt::white);
    
    EXPECT_EQ(planet.getRadius(), testRadius);
}

TEST(SunRadius, FromConstant) {
    Sun sun(QVector3D(), 1000000, 40, Qt::yellow);
    
    EXPECT_EQ(sun.getRadius(), 40);
}

TEST(PlanetRadius, FromConstant) {
    Planet planet(QVector3D(), QVector3D(), 1000, 20, Qt::blue);
    
    EXPECT_EQ(planet.getRadius(), 20);
}

TEST(PlanetRadius, VerySmallRadius) {
    Planet planet(QVector3D(), QVector3D(), 100, 0.1, Qt::white);
    
    EXPECT_EQ(planet.getRadius(), 0.1);
}

TEST(SunRadius, VeryLargeRadius) {
    Sun sun(QVector3D(), 1e20, 1000000, Qt::yellow);
    
    EXPECT_EQ(sun.getRadius(), 1000000);
}

// ============================================================================
// Core: Planet/Sun Color Tests (44-50)
// ============================================================================

TEST(PlanetColor, Blue) {
    Planet planet(QVector3D(), QVector3D(), 100, 10, Qt::blue);
    
    EXPECT_EQ(planet.getColor(), Qt::blue);
}

TEST(SunColor, Yellow) {
    Sun sun(QVector3D(), 1e6, 50, Qt::yellow);
    
    EXPECT_EQ(sun.getColor(), Qt::yellow);
}

TEST(PlanetColor, Red) {
    Planet planet(QVector3D(), QVector3D(), 100, 10, Qt::red);
    
    EXPECT_EQ(planet.getColor(), Qt::red);
}

TEST(PlanetColor, Green) {
    Planet planet(QVector3D(), QVector3D(), 100, 10, Qt::green);
    
    EXPECT_EQ(planet.getColor(), Qt::green);
}

TEST(PlanetColor, CustomColor) {
    QColor custom(255, 128, 64);
    Planet planet(QVector3D(), QVector3D(), 100, 10, custom);
    
    EXPECT_EQ(planet.getColor(), custom);
}

TEST(SunColor, CustomColor) {
    QColor custom(255, 200, 100);
    Sun sun(QVector3D(), 1e6, 50, custom);
    
    EXPECT_EQ(sun.getColor(), custom);
}

// ============================================================================
// Core: Planet/Sun Name Tests (51-56)
// ============================================================================

TEST(SunName, DefaultNameIsSun) {
    Sun sun(QVector3D(), 1e6, 50, Qt::yellow);
    
    EXPECT_EQ(sun.getName(), "Sun");
}

TEST(PlanetName, WithName) {
    Planet earth(QVector3D(100, 0, 0), QVector3D(), 1000, 10, Qt::blue, "Earth");
    
    EXPECT_EQ(earth.getName(), "Earth");
}

TEST(PlanetName, DefaultNameIsEmpty) {
    Planet planet(QVector3D(), QVector3D(), 1000, 10, Qt::white);
    
    EXPECT_EQ(planet.getName(), "");
}

TEST(PlanetName, UniquePlanetNames) {
    Planet mercury(QVector3D(), QVector3D(), 1000, 10, Qt::gray, "Mercury");
    Planet venus(QVector3D(), QVector3D(), 1000, 10, QColor(255, 200, 100), "Venus");
    Planet earth(QVector3D(), QVector3D(), 1000, 10, Qt::blue, "Earth");
    Planet mars(QVector3D(), QVector3D(), 1000, 10, QColor(200, 100, 50), "Mars");
    
    EXPECT_EQ(mercury.getName(), "Mercury");
    EXPECT_EQ(venus.getName(), "Venus");
    EXPECT_EQ(earth.getName(), "Earth");
    EXPECT_EQ(mars.getName(), "Mars");
    EXPECT_NE(mercury.getName(), venus.getName());
    EXPECT_NE(venus.getName(), earth.getName());
    EXPECT_NE(earth.getName(), mars.getName());
}

TEST(PlanetName, CanBeUpdated) {
    Planet planet(QVector3D(), QVector3D(), 1000, 10, Qt::blue, "Earth");
    
    planet.setName("New Planet");
    EXPECT_EQ(planet.getName(), "New Planet");
}

// ============================================================================
// Core: Planet Osculating Orbit Parameters Tests
// ============================================================================

TEST(PlanetOrbit, DefaultOrbitalParams) {
    Planet planet;
    EXPECT_EQ(planet.getSemiMajorAxis(), 0.0);
    EXPECT_EQ(planet.getEccentricity(), 0.0);
    EXPECT_EQ(planet.getPeriapsisDirection(), QVector3D(1, 0, 0));
    EXPECT_EQ(planet.getPerpendicularDirection(), QVector3D(0, 1, 0));
}

TEST(PlanetOrbit, SetOrbitalParamsStoresSmaAndEccentricity) {
    Planet planet;
    planet.setOrbitalParams(1.524, 0.093, QVector3D(0, 1, 0), QVector3D(-1, 0, 0));
    
    EXPECT_NEAR(planet.getSemiMajorAxis(), 1.524, 1e-9);
    EXPECT_NEAR(planet.getEccentricity(), 0.093, 1e-9);
}

TEST(PlanetOrbit, SetOrbitalParamsStoresPeriapsisAndPerpendicularDirections) {
    Planet planet;
    QVector3D periapsis(0, 1, 0);
    QVector3D perpendicular(-1, 0, 0);
    planet.setOrbitalParams(1.0, 0.017, periapsis, perpendicular);
    
    EXPECT_EQ(planet.getPeriapsisDirection(), periapsis);
    EXPECT_EQ(planet.getPerpendicularDirection(), perpendicular);
}

TEST(PlanetOrbit, SetOrbitalParamsCanBeUpdatedAcrossFrames) {
    Planet planet;
    planet.setOrbitalParams(5.203, 0.049, QVector3D(1, 0, 0), QVector3D(0, 1, 0));
    planet.setOrbitalParams(5.204, 0.048, QVector3D(0, 0, 1), QVector3D(0, -1, 0));
    
    EXPECT_NEAR(planet.getSemiMajorAxis(), 5.204, 1e-9);
    EXPECT_NEAR(planet.getEccentricity(), 0.048, 1e-9);
    EXPECT_EQ(planet.getPeriapsisDirection(), QVector3D(0, 0, 1));
    EXPECT_EQ(planet.getPerpendicularDirection(), QVector3D(0, -1, 0));
}

