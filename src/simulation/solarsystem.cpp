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
    setFixedSize(SolarSimConstants::WINDOW_WIDTH, SolarSimConstants::WINDOW_HEIGHT);
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
        axes2D[i] = QPointF(p3.x(), p3.y()) * zoomFactor + QPointF(width()/2, height()/2);
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
    // Draw orbital paths (faint ellipses)
    p.setPen(QPen(QColor(100, 100, 100, 100), 1, Qt::DashLine));
    for (const auto& planet : planets) {
        double a = planet.getSemiMajorAxis();
        double e = planet.getEccentricity();
        double b = a * std::sqrt(1.0 - e*e);
        double c = e * a;
        
        if (a > 0 && b > 0) {
            // Orbital ellipse center (sun at left focus)
            double ellipseCenterX = planet.getSunX() + c;
            QPointF ellipseCenter = QPointF(ellipseCenterX, center.y());
            
            // Draw ellipse (note: QRect takes top-left corner)
            QRectF ellipseRect(ellipseCenter.x() - a * zoomFactor, 
                              ellipseCenter.y() - b * zoomFactor,
                              2 * a * zoomFactor, 
                              2 * b * zoomFactor);
            p.drawEllipse(ellipseRect);
        }
    }
    p.setPen(oldPen);
    
    // Painter's Algorithm: Sort sun and planets by depth (z-coordinate) for proper occlusion
    struct CelestialRenderData {
        const AstronomicalBody* body;
        bool isSun;                 // Track if this is the sun or a planet
        QVector3D rotatedPos;       // Position after 3D rotation
        double z_depth;             // Z-coordinate for sorting (farther = lower z)
    };
    
    std::vector<CelestialRenderData> renderQueue;
    
    // Add sun to render queue
    QVector3D sunRotated = rot.map(sun.getPosition() - QVector3D(width()/2, height()/2, 0));
    renderQueue.push_back({&sun, true, sunRotated, sunRotated.z()});
    
    // Add all planets to render queue
    for (const auto& planet : planets) {
        QVector3D ppos = rot.map(planet.getPosition() - QVector3D(width()/2, height()/2, 0));
        renderQueue.push_back({&planet, false, ppos, ppos.z()});
    }
    
    // Sort by depth: farther objects (lower z) first, closer objects (higher z) last
    std::sort(renderQueue.begin(), renderQueue.end(),
              [](const CelestialRenderData& a, const CelestialRenderData& b) {
                  return a.z_depth < b.z_depth;  // Ascending order: farthest first
              });
    
    // Draw all celestial bodies in sorted order (Painter's Algorithm)
    for (const auto& renderData : renderQueue) {
        const AstronomicalBody& body = *renderData.body;
        QVector3D pos = renderData.rotatedPos;
        QPointF body2D = QPointF(pos.x(), pos.y()) * zoomFactor + QPointF(width()/2, height()/2);
        double depth = 1.0 / std::max(0.001, 1.0 + 0.002 * pos.z());
        
        double radius;
        if (renderData.isSun) {
            // Sun uses its astronomical radius directly
            radius = body.getRadius() * SolarSimConstants::DISPLAY_SCALE * zoomFactor * depth;
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
            const QPixmap& pixmap = body.getSprite().getPixmap();
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
    for (auto& planet : planets) {
        QVector3D r = sun.getPosition() - planet.getPosition();
        double dist = r.length();
        if (dist < 1) dist = 1;
        double force = SolarSimConstants::G * sun.getMass() * planet.getMass() / (dist * dist);
        QVector3D acc = r.normalized() * (force / planet.getMass());
        // Apply speed multiplier to acceleration
        QVector3D newVel = planet.getVelocity() + acc * SolarSimConstants::TIME_STEP * speedMultiplier;
        planet.setVelocity(newVel);
        QVector3D newPos = planet.getPosition() + newVel * SolarSimConstants::TIME_STEP * speedMultiplier;
        planet.setPosition(newPos);
    }
    elapsed += 16;
    update();
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
    planets.clear();
    
    // Display scale: Use logarithmic scaling for better distribution of inner/outer planets
    // Linear scale compresses inner planets. Logarithmic scale spreads them out.
    // displayDist = log(1 + distance_AU) * scale
    double maxDisplayDist = ((width()/2) - SolarSimConstants::MARGIN);
    double maxAU = 30.0; // Neptune's semi-major axis
    double displayScale = maxDisplayDist / std::log(1.0 + maxAU); // scale factor for log scale
    
    auto logScale = [displayScale](double au) {
        return std::log(1.0 + au) * displayScale;
    };
    
    QPointF center(width()/2, height()/2);
    
    struct PlanetParams {
        // Orbital parameters (real solar system values)
        double semiMajorAxis_AU;  // Semi-major axis in Astronomical Units
        double eccentricity;       // Orbital eccentricity
        double inclination_deg;    // Orbital inclination (degrees)
        double meanAnomaly_deg;    // Mean anomaly at epoch (starting angle)
        
        // Physical properties (relative to Earth)
        double massEarthRatios;    // Mass relative to Earth
        double radiusEarthRatios;  // Radius relative to Earth
        
        QColor color;
        QString name;
    };
    
    // Real solar system data
    std::vector<PlanetParams> planetParams = {
        // Mercury: 0.387 AU, high eccentricity, gray
        { 0.387, 0.206, 7.0,   0,   0.055, 0.383, QColor(169, 169, 169), "Mercury" },
        // Venus: 0.723 AU, low eccentricity, yellowish
        { 0.723, 0.007, 3.39,  45,  0.815, 0.949, QColor(255, 200, 100), "Venus" },
        // Earth: 1.0 AU, reference, blue
        { 1.0,   0.017, 0.0,   90,  1.0,   1.0,   QColor(70, 120, 255),  "Earth" },
        // Mars: 1.524 AU, moderate eccentricity, reddish
        { 1.524, 0.093, 1.85,  135, 0.107, 0.532, QColor(200, 100, 50),  "Mars" },
        // Jupiter: 5.203 AU, large and massive
        { 5.203, 0.049, 2.86,  180, 317.8, 10.97, QColor(180, 140, 80),  "Jupiter" },
        // Saturn: 9.537 AU, very large
        { 9.537, 0.056, 2.75,  225, 95.2,  9.14,  QColor(220, 200, 100), "Saturn" },
        // Uranus: 19.191 AU, cyan
        { 19.191, 0.047, 0.77, 270, 14.5,  3.98,  QColor(100, 180, 200), "Uranus" },
        // Neptune: 30.069 AU, deep blue, low eccentricity
        { 30.069, 0.009, 1.77, 315, 17.1,  3.86,  QColor(50, 100, 200),  "Neptune" }
    };
    
    double M = sun.getMass();
    for (const auto& p : planetParams) {
        // Convert AU to display pixels using logarithmic scale
        // This compresses inner planets while spreading outer planets
        double a = logScale(p.semiMajorAxis_AU);
        double e = p.eccentricity;
        
        // Calculate semi-minor axis from eccentricity: b = a * sqrt(1 - e²)
        double b = a * std::sqrt(1.0 - e*e);
        
        // Convert angles to radians
        double inc_rad = qDegreesToRadians(p.inclination_deg);
        double anom_rad = qDegreesToRadians(p.meanAnomaly_deg);
        
        // Position in orbital plane (ellipse): x = a*cos(θ), y = b*sin(θ)
        // With eccentricity applied: distance from center varies
        double cos_anom = std::cos(anom_rad);
        double sin_anom = std::sin(anom_rad);
        
        // Elliptical position
        double x_orbit = a * cos_anom;
        double y_orbit = b * sin_anom;
        
        // Apply orbital inclination (rotate around x-axis for inclination)
        // This tilts the orbital plane relative to the ecliptic
        double x_inclined = x_orbit;
        double y_inclined = y_orbit * std::cos(inc_rad);
        double z_inclined = y_orbit * std::sin(inc_rad);
        
        // Position relative to sun (sun at one focus)
        double c = e * a;  // distance from center to focus
        QVector3D pos = QVector3D(center.x() + x_inclined - c, center.y() + y_inclined, z_inclined);
        
        // Calculate orbital velocity using vis-viva equation
        // r is distance from sun to planet
        double r = std::sqrt(std::pow(pos.x() - sun.getPosition().x(), 2) + 
                            std::pow(pos.y() - sun.getPosition().y(), 2) + 
                            std::pow(pos.z() - sun.getPosition().z(), 2));
        
        // v = sqrt(GM * (2/r - 1/a))
        double v_virial = 2.0/r - 1.0/a;
        double v = 0.0;
        if (v_virial > 0) {
            v = std::sqrt(SolarSimConstants::G * M * v_virial);
        } else {
            v = std::sqrt(SolarSimConstants::G * M / r);
        }
        
        // Tangent vector in orbital plane (perpendicular to radius)
        double tx = -b * sin_anom;
        double ty =  a * cos_anom * std::cos(inc_rad);
        double tz =  a * cos_anom * std::sin(inc_rad);
        double norm = std::sqrt(tx*tx + ty*ty + tz*tz);
        
        if (norm > 1e-6) {
            QVector3D tangent(tx/norm, ty/norm, tz/norm);
            QVector3D tangentNorm = tangent.normalized();
            QVector3D vel = tangentNorm * v;
            
            // Calculate realistic mass and radius
            double mass = p.massEarthRatios * SolarSimConstants::PLANET_MASS;
            double radius = p.radiusEarthRatios * SolarSimConstants::PLANET_RADIUS;
            
            Planet planet(pos, vel, mass, radius, p.color, p.name);
            // Store orbital parameters for visualization
            planet.setOrbitalParams(a, e, inc_rad, sun.getPosition().x());
            planets.append(planet);
        }
    }
}

void SolarSystem::mouseReleaseEvent(QMouseEvent *event) {
    handleMouseRelease(event);
}
void SolarSystem::handleMouseRelease(QMouseEvent *event) {
    if (controller) {
        controller->handleMouseRelease(event);
    }
}
