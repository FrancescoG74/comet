#include <algorithm>
#include <cmath>
#include <memory>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QMatrix4x4>
#include <QApplication>
#include <QKeyEvent>
#include <QtMath>
#include <QFontMetrics>
#include <QFont>

#include "solarsimconstants.h"
#include "solarsystem.h"
#include "solarsystemcontroller.h"
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

void SolarSystem::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    // Re-center the Sun/planets/moons on the new size even while the simulation is paused.
    updateCelestialPositions();
    update();
}

void SolarSystem::wheelEvent(QWheelEvent *event) {
    handleWheel(event);
}
void SolarSystem::handleWheel(QWheelEvent *event) {
    if (controller) {
        controller->handleWheel(event);
    }
}
void SolarSystem::mousePressEvent(QMouseEvent *event) {
    handleMousePress(event);
}
void SolarSystem::handleMousePress(QMouseEvent *event) {
    if (controller) {
        controller->handleMousePress(event);
    }
}

void SolarSystem::mouseMoveEvent(QMouseEvent *event) {
    handleMouseMove(event);
}
void SolarSystem::handleMouseMove(QMouseEvent *event) {
    if (controller) {
        controller->handleMouseMove(event);
    }
}

SolarSystem::SolarSystem(QWidget *parent) : QWidget(parent) {
    setMinimumSize(SolarSimConstants::WINDOW_WIDTH / 2, SolarSimConstants::WINDOW_HEIGHT / 2);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    timer = std::make_unique<QTimer>(this);
    connect(timer.get(), &QTimer::timeout, this, [this]() {
        if (simulationActive) {
            for (int i = 0; i < SolarSimConstants::STEPS_PER_FRAME; ++i) advance();
        }
        update();
    });
    timer->start(SolarSimConstants::TIMER_INTERVAL_MS);
    setFocusPolicy(Qt::StrongFocus);
    controller = std::make_unique<SolarSystemController>(this);
    
    // Set camera for optimal overview of entire solar system
    // Zoom out slightly to fit all planets (Mercury to Neptune)
    zoomFactor = 0.7;
    // Look down at the orbital plane at ~35 degree angle
    viewPitch = -35.0;
    // Slight yaw for 3D perspective
    viewYaw = 15.0;
    
    initBodies();
}

// Helper to validate drawing parameters
static inline bool isValidPoint(const QPointF& pt) {
    return std::isfinite(pt.x()) && std::isfinite(pt.y());
}

static inline bool isValidRadius(double r) {
    return std::isfinite(r) && r > 0;
}

// Convert black pixels to transparent in a pixmap
// This creates a circular appearance for square images with black backgrounds
static QPixmap makeBlackTransparent(const QPixmap& source) {
    QImage image = source.toImage().convertToFormat(QImage::Format_ARGB32);
    
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor color(image.pixel(x, y));
            // If pixel is very dark (near black), make it transparent
            // Using luminance threshold for better results
            int luminance = (color.red() * 299 + color.green() * 587 + color.blue() * 114) / 1000;
            if (luminance < 30) {  // Threshold for near-black pixels
                image.setPixelColor(x, y, QColor(0, 0, 0, 0));  // Transparent
            }
        }
    }
    
    return QPixmap::fromImage(image);
}

// Apply non-linear scaling to planet sizes for better visibility
// Maps realistic size ratios to visible screen sizes
static double scalePlanetRadius(double realRadius) {
    // Scale real astronomical radius to screen pixels using global display scale
    // Uses real radius ratios: Mercury (0.383), Venus (0.949), Earth (1.0), Jupiter (10.97), etc.
    if (realRadius <= 0) return 0;
    return realRadius * SolarSimConstants::DISPLAY_SCALE;
}

void SolarSystem::paintEvent(QPaintEvent *) {
    // DEBUG: Print number of planets being drawn
    // qDebug("[DEBUG] Number of planets: %d", planets.size());
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    // Draw black background
    p.fillRect(rect(), Qt::black);
    // Draw fixed stars
    static const QPoint starPositions[] = {
        {50, 60}, {200, 120}, {400, 80}, {700, 200}, {900, 100},
        {1000, 700}, {800, 600}, {600, 500}, {300, 700}, {150, 400},
        {500, 300}, {900, 400}, {750, 100}, {250, 650}, {950, 650}
    };
    p.setPen(QPen(Qt::white, 2));
    for (const QPoint& pt : starPositions) {
        p.drawPoint(pt);
    }
    // Apply zoom and 3D rotation
    p.save();
    QPointF center(width()/2, height()/2);
    // 3D rotation matrix (yaw, pitch)
    QMatrix4x4 rot;
    rot.rotate(viewYaw, 0, 1, 0);
    rot.rotate(viewPitch, 1, 0, 0);
    // Draw 3D axes (X=red, Y=green, Z=blue) from the sun's position
    const double axisLength = 120.0;
    QVector3D origin3D = sun.getPosition();
    QVector3D xAxis3D = origin3D + QVector3D(axisLength, 0, 0);
    QVector3D yAxis3D = origin3D + QVector3D(0, axisLength, 0);
    QVector3D zAxis3D = origin3D + QVector3D(0, 0, axisLength);
    QVector3D axes3D[4] = {origin3D, xAxis3D, yAxis3D, zAxis3D};
    QPointF axes2D[4];
    for (int i = 0; i < 4; ++i) {
        QVector3D p3 = rot.map(axes3D[i] - QVector3D(width()/2, height()/2, 0));
        axes2D[i] = QPointF(p3.x(), p3.y()) * zoomFactor + QPointF(width()/2, height()/2) - QPointF(cameraOffset.x(), cameraOffset.y());
    }
    QPen oldPen = p.pen();
    QPen xPen(Qt::red, 2);
    QPen yPen(Qt::green, 2);
    QPen zPen(Qt::blue, 2);
    
    // Only draw valid axis lines
    if (isValidPoint(axes2D[0]) && isValidPoint(axes2D[1])) {
        p.setPen(xPen);
        p.drawLine(axes2D[0], axes2D[1]);
    }
    if (isValidPoint(axes2D[0]) && isValidPoint(axes2D[2])) {
        p.setPen(yPen);
        p.drawLine(axes2D[0], axes2D[2]);
    }
    if (isValidPoint(axes2D[0]) && isValidPoint(axes2D[3])) {
        p.setPen(zPen);
        p.drawLine(axes2D[0], axes2D[3]);
    }
    p.setPen(oldPen);
    // Draw orbital paths, rotated through the same 3D view transform as the planets.
    // The orbit shape/orientation is the body's current osculating ellipse (derived from
    // its real position+velocity in updateCelestialPositions), so the path always passes
    // through the body's actual current position instead of assuming a fixed orientation.
    // Each sampled radius is log-scaled individually (like the real planet position) since
    // log-scaling a distance is not the same as log-scaling an already-computed ellipse.
    p.setPen(QPen(QColor(100, 100, 100, 100), 1, Qt::DashLine));
    constexpr int orbitSegments = 90;
    for (const auto& planet : planets) {
        double smaAU = planet.getSemiMajorAxis();
        double e = planet.getEccentricity();
        QVector3D pHat = planet.getPeriapsisDirection();
        QVector3D qHat = planet.getPerpendicularDirection();
        
        if (smaAU > 0) {
            QPointF prevPt;
            bool havePrev = false;
            for (int i = 0; i <= orbitSegments; ++i) {
                double t = qDegreesToRadians(360.0 * i / orbitSegments);
                // True-anomaly polar orbit equation, measured from the real periapsis direction.
                double rAU = smaAU * (1.0 - e * e) / (1.0 + e * std::cos(t));
                double rPixels = screenAuDistance(rAU);
                QVector3D orbitDir = pHat * std::cos(t) + qHat * std::sin(t);
                QVector3D orbitPoint = orbitDir * rPixels;
                QVector3D rotated = rot.map(orbitPoint);
                QPointF screenPt = QPointF(rotated.x(), rotated.y()) * zoomFactor
                                  + center - QPointF(cameraOffset.x(), cameraOffset.y());
                if (havePrev && isValidPoint(prevPt) && isValidPoint(screenPt)) {
                    p.drawLine(prevPt, screenPt);
                }
                prevPt = screenPt;
                havePrev = true;
            }
        }
    }
    p.setPen(oldPen);
    
    // Painter's Algorithm: Sort sun and planets by depth (z-coordinate) for proper occlusion
    struct CelestialRenderData {
        const AstronomicalBody* body;
        bool isSun;                 // Track if this is the sun
        bool isSatellite;           // Track if this is a satellite
        QVector3D rotatedPos;       // Position after 3D rotation
        double z_depth;             // Z-coordinate for sorting (higher = farther)
    };
    
    std::vector<CelestialRenderData> renderQueue;
    
    // Add sun to render queue
    QVector3D sunRotated = rot.map(sun.getPosition() - QVector3D(width()/2, height()/2, 0));
    renderQueue.push_back({&sun, true, false, sunRotated, sunRotated.z()});
    
    // Add all planets to render queue
    for (const auto& planet : planets) {
        QVector3D ppos = rot.map(planet.getPosition() - QVector3D(width()/2, height()/2, 0));
        renderQueue.push_back({&planet, false, false, ppos, ppos.z()});
    }
    
    // Add all satellites to render queue (unless the user hid them)
    if (showSatellites) {
        for (const auto& satellite : satellites) {
            QVector3D spos = rot.map(satellite.getPosition() - QVector3D(width()/2, height()/2, 0));
            renderQueue.push_back({&satellite, false, true, spos, spos.z()});
        }
    }
    
    // Sort by depth: farther objects (higher z) first, closer objects (lower z) last
    std::sort(renderQueue.begin(), renderQueue.end(),
              [](const CelestialRenderData& a, const CelestialRenderData& b) {
                  return a.z_depth > b.z_depth;  // Descending order: farthest first
              });
    
    // Draw all celestial bodies in sorted order (Painter's Algorithm)
    for (const auto& renderData : renderQueue) {
        const AstronomicalBody& body = *renderData.body;
        QVector3D pos = renderData.rotatedPos;
        QPointF body2D = QPointF(pos.x(), pos.y()) * zoomFactor + QPointF(width()/2, height()/2) - QPointF(cameraOffset.x(), cameraOffset.y());
        double depth = 1.0 / std::max(0.001, 1.0 + 0.002 * pos.z());
        
        double radius;
        if (renderData.isSun) {
            // Scaled to stay proportionate to Mercury's orbit; see updateCelestialPositions().
            radius = sunDisplayRadius * zoomFactor * depth;
        } else if (renderData.isSatellite) {
            // Satellites: Use even smaller radius than planets
            double radiusRatio = body.getRadius() / SolarSimConstants::PLANET_RADIUS;
            double scaledRadius = scalePlanetRadius(radiusRatio) * 0.5;  // Half the planet size for visibility
            radius = scaledRadius * zoomFactor * depth;
        } else {
            // Planets use scaled radius
            double radiusRatio = body.getRadius() / SolarSimConstants::PLANET_RADIUS;
            double scaledRadius = scalePlanetRadius(radiusRatio);
            radius = scaledRadius * zoomFactor * depth;
        }
        
        // Skip if point or radius is invalid
        if (!isValidPoint(body2D) || !isValidRadius(radius)) continue;

        // Draw sprite if loaded, otherwise draw colored circle
        if (body.getSprite().isLoaded()) {
            QPixmap pixmap = body.getSprite().getPixmap();
            
            // For sun: apply transparency to black pixels to create circular appearance
            if (renderData.isSun) {
                pixmap = makeBlackTransparent(pixmap);
            }
            
            QPixmap scaled = pixmap.scaledToWidth(static_cast<int>(radius * 2), Qt::SmoothTransformation);
            p.drawPixmap(body2D.x() - radius, body2D.y() - radius, scaled);
        } else {
            if (renderData.isSun) {
                // Draw sun with brightness based on depth
                QColor sunColor = body.getColor();
                sunColor = sunColor.lighter(100 + int(-pos.z()));
                p.setBrush(sunColor);
                p.drawEllipse(body2D, radius, radius);
            } else {
                // Draw planet with radial gradient
                QPointF sun2DPos = QPointF(sun.getPosition().x(), sun.getPosition().y());
                QPointF bodyPos = QPointF(body.getPosition().x(), body.getPosition().y());
                QPointF dir = sun2DPos - bodyPos;
                double len = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
                QPointF gradCenter = body2D;
                if (len > 1e-3) {
                    QPointF offset = dir / len * radius * 0.5;
                    gradCenter = body2D + offset;
                }
                // Validate gradient center before use
                if (!isValidPoint(gradCenter)) gradCenter = body2D;
                
                double gradRadius = std::max(1.0, radius);
                QRadialGradient grad(gradCenter, gradRadius, gradCenter);
                grad.setColorAt(0.0, body.getColor());
                grad.setColorAt(1.0, Qt::black);
                p.setBrush(grad);
                p.drawEllipse(body2D, radius, radius);
            }
        }
        
        // Draw body name label (only for planets, not sun)
        if (!renderData.isSun) {
            QString bodyName = body.getName();
            if (!bodyName.isEmpty()) {
                QFont font = p.font();
                font.setPointSize(8);
                font.setBold(true);
                p.setFont(font);
                
                // Position text below the body
                QPointF textPos = body2D + QPointF(0, radius + 12);
                
                // Validate text position
                if (isValidPoint(textPos)) {
                    // Draw semi-transparent background for readability
                    QFontMetrics fm(font);
                    int textWidth = fm.horizontalAdvance(bodyName);
                    int textHeight = fm.height();
                    QRectF textBg(textPos.x() - textWidth/2 - 2, textPos.y() - textHeight/2, 
                                 textWidth + 4, textHeight);
                    
                    // Draw background
                    QColor bgColor(0, 0, 0, 180);  // Semi-transparent black
                    p.fillRect(textBg, bgColor);
                    
                    // Draw text
                    p.setPen(Qt::white);
                    p.drawText(textPos.x() - textWidth/2, textPos.y() + textHeight/3, bodyName);
                }
            }
        }
    }
    // (end of paintEvent)
    p.restore();
}


void SolarSystem::advance() {
    // Advance simulated time: at 1.0x speed, 1 simulated day passes per real second.
    simDaysElapsed += speedMultiplier * (SolarSimConstants::TIMER_INTERVAL_MS / 1000.0)
                      / SolarSimConstants::STEPS_PER_FRAME;
    updateCelestialPositions();

    elapsed += 16;
    update();
}

double SolarSystem::screenAuDistance(double au) const {
    double maxDisplayDist = ((width() / 2) - SolarSimConstants::MARGIN);
    double maxAU = 30.0; // Neptune's semi-major axis
    double displayScale = maxDisplayDist / std::log(1.0 + maxAU);
    return std::log(1.0 + au) * displayScale;
}

void SolarSystem::updateCelestialPositions() {
    QPointF center(width() / 2, height() / 2);

    // The Sun sits at the heliocentric origin; keep it pinned to the live window
    // center so it tracks layout/resize changes instead of the size at construction time.
    sun.setPosition(QVector3D(center.x(), center.y(), 0));

    // Cap the Sun's visual size relative to Mercury's (innermost planet's) orbit radius so it
    // doesn't visually swallow the first orbit; also keeps it correctly scaled on resize.
    double mercuryOrbitPx = screenAuDistance(planetTable().front().semiMajorAxis_AU);
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
        double pixelR = screenAuDistance(rAu);

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

void SolarSystem::keyPressEvent(QKeyEvent *event) {
    handleKeyPress(event);
}
void SolarSystem::handleKeyPress(QKeyEvent *event) {
    if (controller) {
        controller->handleKeyPress(event);
    }
}

void SolarSystem::initBodies() {
    sun = Sun(QVector3D(width()/2, height()/2, 0), SolarSimConstants::SUN_MASS, SolarSimConstants::SUN_RADIUS, Qt::yellow);
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

    updateCelestialPositions();
}

QDateTime SolarSystem::getSimulationDateTime() const {
    astro_time_t simTime = Astronomy_AddDays(epochTime, simDaysElapsed);
    astro_utc_t utc = Astronomy_UtcFromTime(simTime);
    return QDateTime(QDate(utc.year, utc.month, utc.day),
                      QTime(utc.hour, utc.minute, static_cast<int>(utc.second)),
                      QTimeZone::UTC);
}


void SolarSystem::mouseReleaseEvent(QMouseEvent *event) {
    handleMouseRelease(event);
}
void SolarSystem::handleMouseRelease(QMouseEvent *event) {
    if (controller) {
        controller->handleMouseRelease(event);
    }
}
