#include <algorithm>
#include <cmath>
#include <vector>
#include <QtMath>
#include <QDate>
#include <QTime>
#include <QTimeZone>

#include "solarsimconstants.h"
#include "solarsystemmodel.h"
#include "orbitalelements.h"

namespace {

// Static astronomical data for the 8 real planets rendered by the simulation.
// Position/velocity are no longer stored here: real positions come from
// Astronomy Engine for the current simulated date/time.
struct PlanetMeta {
    astro_body_t body;
    double semiMajorAxis_AU;   // Used only to draw the faint reference orbit ellipse
    double eccentricity;
    double massEarthRatios;
    double radiusEarthRatios;
    QColor color;
    QString name;
};

const std::vector<PlanetMeta>& planetTable() {
    static const std::vector<PlanetMeta> table = {
        { BODY_MERCURY, 0.387,  0.206, 0.055, 0.383, QColor(169, 169, 169), "Mercury" },
        { BODY_VENUS,   0.723,  0.007, 0.815, 0.949, QColor(255, 200, 100), "Venus" },
        { BODY_EARTH,   1.0,    0.017, 1.0,   1.0,   QColor(70, 120, 255),  "Earth" },
        { BODY_MARS,    1.524,  0.093, 0.107, 0.532, QColor(200, 100, 50),  "Mars" },
        { BODY_JUPITER, 5.203,  0.049, 317.8, 10.97, QColor(180, 140, 80),  "Jupiter" },
        { BODY_SATURN,  9.537,  0.056, 95.2,  9.14,  QColor(220, 200, 100), "Saturn" },
        { BODY_URANUS,  19.191, 0.047, 14.5,  3.98,  QColor(100, 180, 200), "Uranus" },
        { BODY_NEPTUNE, 30.069, 0.009, 17.1,  3.86,  QColor(50, 100, 200),  "Neptune" }
    };
    return table;
}

// Rotates an EQJ (equatorial J2000) Cartesian vector into the ecliptic frame,
// which keeps the visualization close to a top-down view of the solar system.
QVector3D eqjToEclipticAU(double x, double y, double z, astro_time_t t) {
    astro_vector_t v{ASTRO_SUCCESS, x, y, z, t};
    astro_ecliptic_t ecl = Astronomy_Ecliptic(v);
    return QVector3D(ecl.vec.x, ecl.vec.y, ecl.vec.z);
}

} // namespace

void SolarSystemModel::setSimulationSpeedMultiplier(double multiplier) {
    speedMultiplier = std::max(0.1, multiplier);
}

void SolarSystemModel::initBodies(int viewWidth, int viewHeight) {
    sun = Sun(QVector3D(viewWidth / 2.0, viewHeight / 2.0, 0),
              SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
    sun.setSprite("../assets/sun-blasts-a-m66-flare.jpg");

    // Reset simulated time to "now": real ephemeris positions are computed for this epoch.
    epochTime = Astronomy_CurrentTime();
    simDaysElapsed = 0.0;

    planets.clear();
    for (const auto& meta : planetTable()) {
        double mass = meta.massEarthRatios * SolarSimConstants::PLANET_MASS;
        double radius = meta.radiusEarthRatios * SolarSimConstants::PLANET_RADIUS;
        Planet planet(sun.getPosition(), QVector3D(), mass, radius, meta.color, meta.name);
        planets.append(planet);
    }

    // Initialize satellites (moons); positions are placeholders until updateCelestialPositions() runs.
    satellites.clear();
    Planet* earthPtr = nullptr;
    Planet* jupiterPtr = nullptr;
    Planet* saturnPtr = nullptr;
    for (auto& planet : planets) {
        if (planet.getName() == "Earth") earthPtr = &planet;
        else if (planet.getName() == "Jupiter") jupiterPtr = &planet;
        else if (planet.getName() == "Saturn") saturnPtr = &planet;
    }

    if (earthPtr) {
        double moonMass = earthPtr->getMass() * (1.0 / 81.3);
        double moonRadius = earthPtr->getRadius() * 0.27;
        satellites.append(Satellite(earthPtr->getPosition(), QVector3D(), moonMass, moonRadius, Qt::lightGray, earthPtr, "Moon"));
    }

    if (jupiterPtr) {
        struct JupiterMoonMeta { double mass; double radius; QColor color; QString name; };
        static const std::vector<JupiterMoonMeta> jupiterMoons = {
            { 0.0015, 0.28, QColor(200, 150, 100), "Io" },
            { 0.0008, 0.25, QColor(100, 150, 200), "Europa" },
            { 0.0025, 0.41, QColor(150, 120, 100), "Ganymede" },
            { 0.0018, 0.38, QColor(120, 100, 80), "Callisto" }
        };
        for (const auto& m : jupiterMoons) {
            double moonMass = jupiterPtr->getMass() * m.mass;
            double moonRadius = jupiterPtr->getRadius() * m.radius;
            satellites.append(Satellite(jupiterPtr->getPosition(), QVector3D(), moonMass, moonRadius, m.color, jupiterPtr, m.name));
        }
    }

    if (saturnPtr) {
        struct SaturnMoonMeta { double mass; double radius; QColor color; QString name; };
        static const std::vector<SaturnMoonMeta> saturnMoons = {
            { 0.0003, 0.27, QColor(200, 180, 160), "Mimas" },
            { 0.0007, 0.40, QColor(220, 200, 180), "Enceladus" },
            { 0.0017, 0.49, QColor(180, 160, 140), "Tethys" },
            { 0.0018, 0.48, QColor(160, 140, 120), "Dione" },
            { 0.0024, 0.47, QColor(140, 130, 110), "Rhea" },
            { 0.0225, 0.80, QColor(120, 110, 90), "Titan" },
            { 0.0028, 0.73, QColor(100, 80, 60), "Iapetus" }
        };
        for (const auto& m : saturnMoons) {
            double moonMass = saturnPtr->getMass() * m.mass;
            double moonRadius = saturnPtr->getRadius() * m.radius;
            satellites.append(Satellite(saturnPtr->getPosition(), QVector3D(), moonMass, moonRadius, m.color, saturnPtr, m.name));
        }
    }

    updateCelestialPositions(viewWidth, viewHeight);
}

void SolarSystemModel::advance(int viewWidth, int viewHeight) {
    // Advance simulated time: at 1.0x speed, 1 simulated day passes per real second.
    simDaysElapsed += speedMultiplier * (SolarSimConstants::TIMER_INTERVAL_MS / 1000.0)
                      / SolarSimConstants::STEPS_PER_FRAME;
    updateCelestialPositions(viewWidth, viewHeight);
    elapsed += 16;
}

double SolarSystemModel::screenAuDistance(int viewWidth, double au) const {
    double maxDisplayDist = ((viewWidth / 2) - SolarSimConstants::MARGIN);
    double maxAU = 30.0; // Neptune's semi-major axis
    double displayScale = maxDisplayDist / std::log(1.0 + maxAU);
    return std::log(1.0 + au) * displayScale;
}

void SolarSystemModel::updateCelestialPositions(int viewWidth, int viewHeight) {
    QPointF center(viewWidth / 2.0, viewHeight / 2.0);

    // The Sun sits at the heliocentric origin; keep it pinned to the live window
    // center so it tracks layout/resize changes instead of the size at construction time.
    sun.setPosition(QVector3D(center.x(), center.y(), 0));

    // Cap the Sun's visual size relative to Mercury's (innermost planet's) orbit radius so it
    // doesn't visually swallow the first orbit; also keeps it correctly scaled on resize.
    double mercuryOrbitPx = screenAuDistance(viewWidth, planetTable().front().semiMajorAxis_AU);
    sunDisplayRadius = std::max(6.0, mercuryOrbitPx * 0.35);

    astro_time_t simTime = Astronomy_AddDays(epochTime, simDaysElapsed);
    double muSun = Astronomy_MassProduct(BODY_SUN); // AU^3/day^2

    Planet* earthPtr = nullptr;
    Planet* jupiterPtr = nullptr;
    Planet* saturnPtr = nullptr;

    const auto& table = planetTable();
    for (int i = 0; i < planets.size() && i < static_cast<int>(table.size()); ++i) {
        Planet& planet = planets[i];
        const PlanetMeta& meta = table[i];

        astro_state_vector_t state = Astronomy_HelioState(meta.body, simTime);
        QVector3D rVec = eqjToEclipticAU(state.x, state.y, state.z, simTime);
        QVector3D vVec = eqjToEclipticAU(state.vx, state.vy, state.vz, simTime);
        double rAu = rVec.length();
        QVector3D dir = rAu > 1e-9 ? rVec / rAu : QVector3D(1, 0, 0);
        double pixelR = screenAuDistance(viewWidth, rAu);

        planet.setPosition(QVector3D(center.x() + dir.x() * pixelR,
                                      center.y() + dir.y() * pixelR,
                                      dir.z() * pixelR));

        // Osculating orbit (derived from the current real position+velocity) so the drawn
        // reference orbit always passes through the body's actual current position, instead
        // of assuming a fixed periapsis orientation.
        OsculatingElements elements = computeOsculatingElements(rVec, vVec, muSun);
        planet.setOrbitalParams(elements.semiMajorAxisAU, elements.eccentricity,
                                 elements.periapsisDirection, elements.perpendicularDirection);

        if (meta.name == "Earth") earthPtr = &planet;
        else if (meta.name == "Jupiter") jupiterPtr = &planet;
        else if (meta.name == "Saturn") saturnPtr = &planet;
    }

    int satIdx = 0;
    constexpr double moonPixelDist = 10.0;

    // Earth's Moon: real ephemeris direction, artistic fixed display distance.
    if (earthPtr && satIdx < satellites.size()) {
        astro_vector_t geo = Astronomy_GeoMoon(simTime);
        QVector3D dirVec = eqjToEclipticAU(geo.x, geo.y, geo.z, simTime);
        double len = dirVec.length();
        QVector3D dir = len > 1e-9 ? dirVec / len : QVector3D(1, 0, 0);
        satellites[satIdx].setPosition(earthPtr->getPosition() + dir * moonPixelDist);
        ++satIdx;
    }

    // Jupiter's 4 Galilean moons: real ephemeris positions from Astronomy_JupiterMoons.
    if (jupiterPtr) {
        astro_jupiter_moons_t jm = Astronomy_JupiterMoons(simTime);
        const astro_state_vector_t* moons[4] = {&jm.io, &jm.europa, &jm.ganymede, &jm.callisto};
        const double pixelDist[4] = {8.0, 12.0, 16.0, 22.0};
        for (int i = 0; i < 4 && satIdx < satellites.size(); ++i, ++satIdx) {
            QVector3D dirVec = eqjToEclipticAU(moons[i]->x, moons[i]->y, moons[i]->z, simTime);
            double len = dirVec.length();
            QVector3D dir = len > 1e-9 ? dirVec / len : QVector3D(1, 0, 0);
            satellites[satIdx].setPosition(jupiterPtr->getPosition() + dir * pixelDist[i]);
        }
    }

    // Saturn's 7 main moons: Astronomy Engine has no ephemeris model for them, so their
    // motion is approximated with a simple circular orbit at their real orbital period.
    if (saturnPtr) {
        struct SaturnMoon { double periodDays; double pixelDist; double initialPhaseDeg; };
        static const SaturnMoon saturnMoons[7] = {
            {0.942,  6.0,  0.0},   // Mimas
            {1.370,  7.5,  51.4},  // Enceladus
            {1.888,  9.0,  102.8}, // Tethys
            {2.737,  10.5, 154.2}, // Dione
            {4.518,  13.0, 205.6}, // Rhea
            {15.945, 20.0, 257.0}, // Titan
            {79.33,  35.0, 308.4}  // Iapetus
        };
        for (const auto& m : saturnMoons) {
            if (satIdx >= satellites.size()) break;
            double angle = qDegreesToRadians(m.initialPhaseDeg) + qDegreesToRadians(360.0) * (simDaysElapsed / m.periodDays);
            QVector3D dir(std::cos(angle), std::sin(angle), 0.0);
            satellites[satIdx].setPosition(saturnPtr->getPosition() + dir * m.pixelDist);
            ++satIdx;
        }
    }
}

QDateTime SolarSystemModel::getSimulationDateTime() const {
    astro_time_t simTime = Astronomy_AddDays(epochTime, simDaysElapsed);
    astro_utc_t utc = Astronomy_UtcFromTime(simTime);
    return QDateTime(QDate(utc.year, utc.month, utc.day),
                      QTime(utc.hour, utc.minute, static_cast<int>(utc.second)),
                      QTimeZone::UTC);
}
