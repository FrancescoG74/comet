#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <QVector3D>
#include <QtMath>
#include <cmath>
#include "orbitalelements.h"

using Catch::Matchers::WithinRel;
using Catch::Matchers::WithinAbs;

// ============================================================================
// Simulation: Osculating orbital elements (computeOsculatingElements)
// ============================================================================
// These tests build known analytic two-body state vectors (position/velocity) for
// circular and elliptical orbits and verify that the extracted osculating elements
// (semi-major axis, eccentricity, periapsis direction) match, and that the resulting
// orbit reproduces the input distance at the input's true anomaly. This is the same
// math used to draw each planet's reference orbit so it always passes through its
// current real position.

namespace {
constexpr double kMuSun = 0.2959122082855911e-03; // AU^3/day^2 (Sun GM, matches Astronomy Engine)
}

TEST_CASE("Circular orbit has near-zero eccentricity", "[orbit][elements]") {
    double r0 = 1.0; // AU
    double vCirc = std::sqrt(kMuSun / r0);
    QVector3D rVec(r0, 0, 0);
    QVector3D vVec(0, vCirc, 0);
    
    OsculatingElements elements = computeOsculatingElements(rVec, vVec, kMuSun);
    
    REQUIRE_THAT(elements.semiMajorAxisAU, WithinRel(r0, 1e-6));
    REQUIRE_THAT(elements.eccentricity, WithinAbs(0.0, 1e-6));
}

TEST_CASE("Elliptical orbit at periapsis recovers sma, eccentricity and direction", "[orbit][elements]") {
    double a = 1.524;   // Mars-like semi-major axis (AU)
    double e = 0.093;
    double rPeri = a * (1.0 - e);
    double vPeri = std::sqrt(kMuSun * ((2.0 / rPeri) - (1.0 / a)));
    QVector3D rVec(rPeri, 0, 0);
    QVector3D vVec(0, vPeri, 0); // prograde, perpendicular to r at periapsis/apoapsis
    
    OsculatingElements elements = computeOsculatingElements(rVec, vVec, kMuSun);
    
    REQUIRE_THAT(elements.semiMajorAxisAU, WithinRel(a, 1e-6));
    REQUIRE_THAT(elements.eccentricity, WithinRel(e, 1e-6));
    // At periapsis the position itself points toward periapsis.
    REQUIRE_THAT(elements.periapsisDirection.x(), WithinAbs(1.0, 1e-6));
    REQUIRE_THAT(elements.periapsisDirection.y(), WithinAbs(0.0, 1e-6));
}

TEST_CASE("Elliptical orbit at apoapsis places periapsis on the opposite side", "[orbit][elements]") {
    double a = 5.203;   // Jupiter-like semi-major axis (AU)
    double e = 0.049;
    double rApo = a * (1.0 + e);
    double vApo = std::sqrt(kMuSun * ((2.0 / rApo) - (1.0 / a)));
    QVector3D rVec(rApo, 0, 0);
    QVector3D vVec(0, vApo, 0);
    
    OsculatingElements elements = computeOsculatingElements(rVec, vVec, kMuSun);
    
    REQUIRE_THAT(elements.semiMajorAxisAU, WithinRel(a, 1e-6));
    REQUIRE_THAT(elements.eccentricity, WithinRel(e, 1e-6));
    // At apoapsis the position points away from periapsis.
    REQUIRE_THAT(elements.periapsisDirection.x(), WithinAbs(-1.0, 1e-6));
    REQUIRE_THAT(elements.periapsisDirection.y(), WithinAbs(0.0, 1e-6));
}

TEST_CASE("Periapsis and perpendicular directions are unit length", "[orbit][elements]") {
    double a = 9.537; // Saturn-like
    double e = 0.056;
    double rPeri = a * (1.0 - e);
    double vPeri = std::sqrt(kMuSun * ((2.0 / rPeri) - (1.0 / a)));
    QVector3D rVec(rPeri, 0, 0);
    QVector3D vVec(0, vPeri, 0);
    
    OsculatingElements elements = computeOsculatingElements(rVec, vVec, kMuSun);
    
    REQUIRE_THAT(elements.periapsisDirection.length(), WithinRel(1.0, 1e-6));
    REQUIRE_THAT(elements.perpendicularDirection.length(), WithinRel(1.0, 1e-6));
}

TEST_CASE("Periapsis and perpendicular directions are orthogonal", "[orbit][elements]") {
    double a = 19.191; // Uranus-like
    double e = 0.047;
    double rPeri = a * (1.0 - e);
    double vPeri = std::sqrt(kMuSun * ((2.0 / rPeri) - (1.0 / a)));
    QVector3D rVec(rPeri, 0, 0);
    QVector3D vVec(0, vPeri, 0);
    
    OsculatingElements elements = computeOsculatingElements(rVec, vVec, kMuSun);
    double dot = QVector3D::dotProduct(elements.periapsisDirection, elements.perpendicularDirection);
    
    REQUIRE_THAT(dot, WithinAbs(0.0, 1e-6));
}

TEST_CASE("Orbit polar equation reproduces distance at an arbitrary true anomaly", "[orbit][elements]") {
    // Pick an orbit and a true anomaly, build the state vector analytically, then verify
    // the reconstructed osculating ellipse reproduces the same distance at that anomaly.
    double a = 30.069;  // Neptune-like semi-major axis
    double e = 0.009;
    double trueAnomaly = qDegreesToRadians(72.0);
    double mu = kMuSun;
    
    double rMag = a * (1.0 - e * e) / (1.0 + e * std::cos(trueAnomaly));
    // Standard planar Keplerian state vector construction (perifocal frame, periapsis along +X).
    double p = a * (1.0 - e * e);
    double h = std::sqrt(mu * p);
    QVector3D rVec(rMag * std::cos(trueAnomaly), rMag * std::sin(trueAnomaly), 0.0);
    QVector3D vVec((-mu / h) * std::sin(trueAnomaly), (mu / h) * (e + std::cos(trueAnomaly)), 0.0);
    
    OsculatingElements elements = computeOsculatingElements(rVec, vVec, mu);
    
    // QVector3D uses single-precision floats internally, so allow a looser tolerance here.
    REQUIRE_THAT(elements.semiMajorAxisAU, WithinRel(a, 1e-5));
    REQUIRE_THAT(elements.eccentricity, WithinRel(e, 1e-4));
    
    // Reconstruct distance using the extracted elements at the same true anomaly and
    // confirm it matches the original distance (this is exactly what paintEvent relies on).
    QVector3D dir = rVec.normalized();
    double cosT = QVector3D::dotProduct(dir, elements.periapsisDirection);
    double sinT = QVector3D::dotProduct(dir, elements.perpendicularDirection);
    double reconstructedAnomaly = std::atan2(sinT, cosT);
    double reconstructedR = elements.semiMajorAxisAU * (1.0 - elements.eccentricity * elements.eccentricity)
                           / (1.0 + elements.eccentricity * std::cos(reconstructedAnomaly));
    
    REQUIRE_THAT(reconstructedR, WithinRel(rMag, 1e-6));
}

TEST_CASE("Degenerate zero position returns default elements without crashing", "[orbit][elements]") {
    OsculatingElements elements = computeOsculatingElements(QVector3D(0, 0, 0), QVector3D(0, 1, 0), kMuSun);
    
    REQUIRE(elements.semiMajorAxisAU == 0.0);
    REQUIRE(elements.eccentricity == 0.0);
}
