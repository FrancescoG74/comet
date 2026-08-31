#pragma once
#include <QVector3D>
#include <cmath>

// Osculating (instantaneous two-body) orbital ellipse derived from a heliocentric
// position/velocity state vector. Used to draw a reference orbit that always passes
// through a body's current position, regardless of real orbital perturbations.
struct OsculatingElements {
    double semiMajorAxisAU = 0.0;
    double eccentricity = 0.0;
    QVector3D periapsisDirection = QVector3D(1, 0, 0);      // Unit vector toward periapsis
    QVector3D perpendicularDirection = QVector3D(0, 1, 0);  // Unit vector, 90 deg ahead in the orbital plane
};

// rVec/vVec must be expressed in the same frame (e.g. AU / AU-per-day heliocentric).
// muSun is the Sun's standard gravitational parameter in AU^3/day^2.
inline OsculatingElements computeOsculatingElements(const QVector3D& rVec, const QVector3D& vVec, double muSun) {
    OsculatingElements result;
    double rAu = rVec.length();
    if (rAu < 1e-9 || muSun <= 0.0) return result;
    QVector3D dir = rVec / static_cast<float>(rAu);

    // Specific angular momentum and eccentricity vector (standard two-body orbit formulas).
    QVector3D hVec = QVector3D::crossProduct(rVec, vVec);
    QVector3D eVec = QVector3D::crossProduct(vVec, hVec) / static_cast<float>(muSun) - dir;
    double eMag = eVec.length();
    result.periapsisDirection = eMag > 1e-6 ? eVec / static_cast<float>(eMag) : dir;

    QVector3D hHat = hVec.length() > 1e-9 ? hVec.normalized() : QVector3D(0, 0, 1);
    result.perpendicularDirection = QVector3D::crossProduct(hHat, result.periapsisDirection).normalized();

    double energy = 0.5 * vVec.lengthSquared() - muSun / rAu;
    result.semiMajorAxisAU = std::abs(energy) > 1e-12 ? -muSun / (2.0 * energy) : rAu;
    result.eccentricity = eMag;
    return result;
}
