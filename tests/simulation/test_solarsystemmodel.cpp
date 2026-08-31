#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cmath>

#include "solarsimconstants.h"
#include "solarsystemmodel.h"

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

namespace {

constexpr int kViewWidth = 1024;
constexpr int kViewHeight = 768;

// The view drives the model STEPS_PER_FRAME times per timer tick, so one full tick is what
// the documented "1.0x = 1 simulated day per real second" speed is defined against.
void advanceOneFrame(SolarSystemModel& model) {
    for (int i = 0; i < SolarSimConstants::STEPS_PER_FRAME; ++i) {
        model.advance(kViewWidth, kViewHeight);
    }
}

SolarSystemModel makeModel() {
    SolarSystemModel model;
    model.initBodies(kViewWidth, kViewHeight);
    return model;
}

double distanceFromSun(const SolarSystemModel& model, const QString& planetName) {
    for (const auto& planet : model.getPlanets()) {
        if (planet.getName() == planetName) {
            return (planet.getPosition() - model.getSun().getPosition()).length();
        }
    }
    return -1.0;
}

}

TEST_CASE("SolarSystemModel starts paused with satellites visible", "[model]") {
    SolarSystemModel model;
    REQUIRE_FALSE(model.isSimulationActive());
    REQUIRE(model.getShowSatellites());
    REQUIRE_THAT(model.getSimulationSpeedMultiplier(), WithinAbs(1.0, 1e-9));
}

TEST_CASE("SolarSystemModel initBodies creates the eight planets in order", "[model]") {
    const SolarSystemModel model = makeModel();
    const QVector<Planet>& planets = model.getPlanets();

    REQUIRE(planets.size() == 8);
    REQUIRE(planets[0].getName() == "Mercury");
    REQUIRE(planets[2].getName() == "Earth");
    REQUIRE(planets[7].getName() == "Neptune");
}

TEST_CASE("SolarSystemModel initBodies creates the twelve modelled moons", "[model]") {
    const SolarSystemModel model = makeModel();
    // 1 Earth moon + 4 Galilean moons + 7 Saturnian moons.
    REQUIRE(model.getSatellites().size() == 12);
    REQUIRE(model.getSatellites().front().getName() == "Moon");
}

TEST_CASE("SolarSystemModel centres the sun in the view", "[model]") {
    const SolarSystemModel model = makeModel();
    const QVector3D sunPos = model.getSun().getPosition();

    REQUIRE_THAT(sunPos.x(), WithinAbs(kViewWidth / 2.0, 1e-6));
    REQUIRE_THAT(sunPos.y(), WithinAbs(kViewHeight / 2.0, 1e-6));
    REQUIRE_THAT(sunPos.z(), WithinAbs(0.0, 1e-6));
}

TEST_CASE("SolarSystemModel loads the sun sprite from Qt resources", "[model][sprite]") {
    const SolarSystemModel model = makeModel();
    REQUIRE(model.getSun().getSprite().isLoaded());
}

TEST_CASE("SolarSystemModel keeps the sun visible but smaller than Mercury's orbit", "[model]") {
    const SolarSystemModel model = makeModel();
    const double mercuryOrbit = model.screenAuDistance(kViewWidth, 0.387);

    REQUIRE(model.getSunDisplayRadius() >= 6.0);
    REQUIRE(model.getSunDisplayRadius() < mercuryOrbit);
}

TEST_CASE("screenAuDistance maps zero AU to the sun's own position", "[model][scaling]") {
    const SolarSystemModel model;
    REQUIRE_THAT(model.screenAuDistance(kViewWidth, 0.0), WithinAbs(0.0, 1e-9));
}

TEST_CASE("screenAuDistance is strictly increasing with distance", "[model][scaling]") {
    const SolarSystemModel model;
    double previous = -1.0;
    for (double au : {0.0, 0.387, 1.0, 5.203, 9.537, 19.191, 30.0}) {
        const double pixels = model.screenAuDistance(kViewWidth, au);
        REQUIRE(pixels > previous);
        previous = pixels;
    }
}

TEST_CASE("screenAuDistance fits Neptune's orbit inside the view margin", "[model][scaling]") {
    const SolarSystemModel model;
    const double maxDisplay = (kViewWidth / 2) - SolarSimConstants::MARGIN;
    REQUIRE_THAT(model.screenAuDistance(kViewWidth, 30.0), WithinRel(maxDisplay, 1e-9));
}

TEST_CASE("screenAuDistance compresses outer orbits logarithmically", "[model][scaling]") {
    const SolarSystemModel model;
    // Neptune is ~30x further out than Earth, but must not be drawn 30x further out.
    const double earth = model.screenAuDistance(kViewWidth, 1.0);
    const double neptune = model.screenAuDistance(kViewWidth, 30.0);
    REQUIRE(neptune < earth * 30.0);
    REQUIRE(neptune > earth);
}

TEST_CASE("SolarSystemModel places planets at their real ordered distances", "[model][ephemeris]") {
    const SolarSystemModel model = makeModel();

    // Real ephemeris positions vary with the current date, but the ordering of the
    // semi-major axes is wide enough that instantaneous distances never overlap.
    REQUIRE(distanceFromSun(model, "Mercury") < distanceFromSun(model, "Earth"));
    REQUIRE(distanceFromSun(model, "Earth") < distanceFromSun(model, "Jupiter"));
    REQUIRE(distanceFromSun(model, "Jupiter") < distanceFromSun(model, "Neptune"));
}

TEST_CASE("SolarSystemModel puts Earth about one AU from the sun", "[model][ephemeris]") {
    const SolarSystemModel model = makeModel();
    // Earth's real distance oscillates between 0.983 and 1.017 AU over a year.
    const double nearest = model.screenAuDistance(kViewWidth, 0.98);
    const double farthest = model.screenAuDistance(kViewWidth, 1.02);
    const double actual = distanceFromSun(model, "Earth");

    REQUIRE(actual > nearest);
    REQUIRE(actual < farthest);
}

TEST_CASE("SolarSystemModel derives bound elliptical orbits for every planet", "[model][ephemeris]") {
    const SolarSystemModel model = makeModel();
    for (const auto& planet : model.getPlanets()) {
        INFO("planet: " << planet.getName().toStdString());
        REQUIRE(planet.getSemiMajorAxis() > 0.0);
        REQUIRE(planet.getEccentricity() >= 0.0);
        REQUIRE(planet.getEccentricity() < 1.0);
    }
}

TEST_CASE("SolarSystemModel keeps each moon near its parent planet", "[model][ephemeris]") {
    const SolarSystemModel model = makeModel();

    QVector3D earthPos;
    for (const auto& planet : model.getPlanets()) {
        if (planet.getName() == "Earth") earthPos = planet.getPosition();
    }

    // The Moon is drawn at a fixed 10px offset from Earth.
    const double offset = (model.getSatellites().front().getPosition() - earthPos).length();
    REQUIRE_THAT(offset, WithinAbs(10.0, 1e-4));
}

TEST_CASE("SolarSystemModel advances one simulated day per second at 1.0x", "[model][time]") {
    SolarSystemModel model = makeModel();
    const QDateTime start = model.getSimulationDateTime();

    // 1 real second at ~60 FPS.
    const int framesPerSecond = 1000 / SolarSimConstants::TIMER_INTERVAL_MS;
    for (int i = 0; i < framesPerSecond; ++i) advanceOneFrame(model);

    const double daysElapsed = start.msecsTo(model.getSimulationDateTime()) / 86400000.0;
    REQUIRE_THAT(daysElapsed, WithinAbs(1.0, 0.01));
}

TEST_CASE("SolarSystemModel scales elapsed time by the speed multiplier", "[model][time]") {
    SolarSystemModel slow = makeModel();
    SolarSystemModel fast = makeModel();
    fast.setSimulationSpeedMultiplier(10.0);

    const QDateTime slowStart = slow.getSimulationDateTime();
    const QDateTime fastStart = fast.getSimulationDateTime();
    for (int i = 0; i < 60; ++i) {
        advanceOneFrame(slow);
        advanceOneFrame(fast);
    }

    const qint64 slowDelta = slowStart.msecsTo(slow.getSimulationDateTime());
    const qint64 fastDelta = fastStart.msecsTo(fast.getSimulationDateTime());
    REQUIRE_THAT(static_cast<double>(fastDelta), WithinRel(static_cast<double>(slowDelta) * 10.0, 0.01));
}

TEST_CASE("SolarSystemModel clamps the speed multiplier to a positive minimum", "[model][time]") {
    SolarSystemModel model;
    model.setSimulationSpeedMultiplier(0.0);
    REQUIRE_THAT(model.getSimulationSpeedMultiplier(), WithinAbs(0.1, 1e-9));

    model.setSimulationSpeedMultiplier(-5.0);
    REQUIRE_THAT(model.getSimulationSpeedMultiplier(), WithinAbs(0.1, 1e-9));
}

TEST_CASE("SolarSystemModel moves planets as simulated time passes", "[model][ephemeris]") {
    SolarSystemModel model = makeModel();
    const QVector3D before = model.getPlanets().front().getPosition();

    // Mercury's year is 88 days, so ~30 simulated days is a large, unambiguous arc.
    for (int i = 0; i < 30 * (1000 / SolarSimConstants::TIMER_INTERVAL_MS); ++i) {
        advanceOneFrame(model);
    }

    REQUIRE((model.getPlanets().front().getPosition() - before).length() > 1.0);
}

TEST_CASE("SolarSystemModel rescales positions when the view is resized", "[model][scaling]") {
    SolarSystemModel model = makeModel();
    const double wideDistance = distanceFromSun(model, "Earth");

    model.updateCelestialPositions(kViewWidth / 2, kViewHeight);
    const double narrowDistance = distanceFromSun(model, "Earth");

    REQUIRE(narrowDistance < wideDistance);
    REQUIRE_THAT(model.getSun().getPosition().x(), WithinAbs(kViewWidth / 4.0, 1e-6));
}

TEST_CASE("SolarSystemModel initBodies resets the simulated clock", "[model][time]") {
    SolarSystemModel model = makeModel();
    for (int i = 0; i < 200; ++i) advanceOneFrame(model);
    const QDateTime drifted = model.getSimulationDateTime();

    model.initBodies(kViewWidth, kViewHeight);
    const QDateTime reset = model.getSimulationDateTime();

    REQUIRE(reset < drifted);
    REQUIRE(std::abs(reset.secsTo(QDateTime::currentDateTimeUtc())) < 60);
}
