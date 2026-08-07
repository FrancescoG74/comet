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
    initBodies();
}

// Helper to validate drawing parameters
static inline bool isValidPoint(const QPointF& pt) {
    return std::isfinite(pt.x()) && std::isfinite(pt.y());
}

static inline bool isValidRadius(double r) {
    return std::isfinite(r) && r > 0;
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
    // Draw Sun (3D->2D projection with depth)
    QVector3D spos = rot.map(sun.getPosition() - QVector3D(width()/2, height()/2, 0));
    QPointF sun2D = QPointF(spos.x(), spos.y()) * zoomFactor + QPointF(width()/2, height()/2);
    double sunDepth = 1.0 / std::max(0.001, 1.0 + 0.002 * spos.z());
    double sunRadius = sun.getRadius() * zoomFactor * sunDepth;
    
    // Validate before drawing
    if (isValidPoint(sun2D) && isValidRadius(sunRadius)) {
        // Draw sprite if loaded, otherwise draw colored circle
        if (sun.getSprite().isLoaded()) {
            const QPixmap& pixmap = sun.getSprite().getPixmap();
            QPixmap scaled = pixmap.scaledToWidth(static_cast<int>(sunRadius * 2), Qt::SmoothTransformation);
            p.drawPixmap(sun2D.x() - sunRadius, sun2D.y() - sunRadius, scaled);
        } else {
            QColor sunColor = sun.getColor();
            sunColor = sunColor.lighter(100 + int(-spos.z()));
            p.setBrush(sunColor);
            p.drawEllipse(sun2D, sunRadius, sunRadius);
        }
    }
    // Draw Planets (3D->2D projection with depth and gradient)
    for (const auto& planet : planets) {
        QVector3D ppos = rot.map(planet.getPosition() - QVector3D(width()/2, height()/2, 0));
        QPointF planet2D = QPointF(ppos.x(), ppos.y()) * zoomFactor + QPointF(width()/2, height()/2);
        double depth = 1.0 / std::max(0.001, 1.0 + 0.002 * ppos.z());
        double pradius = planet.getRadius() * zoomFactor * depth;
        
        // Skip if point or radius is invalid
        if (!isValidPoint(planet2D) || !isValidRadius(pradius)) continue;

        // Draw sprite if loaded, otherwise draw colored circle
        if (planet.getSprite().isLoaded()) {
            const QPixmap& pixmap = planet.getSprite().getPixmap();
            QPixmap scaled = pixmap.scaledToWidth(static_cast<int>(pradius * 2), Qt::SmoothTransformation);
            p.drawPixmap(planet2D.x() - pradius, planet2D.y() - pradius, scaled);
        } else {
            // Direction from planet to sun in 2D (screen space)
            QPointF sun2D = QPointF(sun.getPosition().x(), sun.getPosition().y());
            QPointF planet2Dpos = QPointF(planet.getPosition().x(), planet.getPosition().y());
            QPointF dir = sun2D - planet2Dpos;
            double len = std::sqrt(dir.x()*dir.x() + dir.y()*dir.y());
            QPointF gradCenter = planet2D;
            if (len > 1e-3) {
                QPointF offset = dir / len * pradius * 0.5;
                gradCenter = planet2D + offset;
            }
            // Validate gradient center before use
            if (!isValidPoint(gradCenter)) gradCenter = planet2D;
            
            double gradRadius = std::max(1.0, pradius);
            QRadialGradient grad(gradCenter, gradRadius, gradCenter);
            grad.setColorAt(0.0, planet.getColor());
            grad.setColorAt(1.0, Qt::black);
            p.setBrush(grad);
            p.drawEllipse(planet2D, pradius, pradius);
        }
        
        // Draw planet name label
        QString planetName = planet.getName();
        if (!planetName.isEmpty()) {
            QFont font = p.font();
            font.setPointSize(8);
            font.setBold(true);
            p.setFont(font);
            
            // Position text below the planet
            QPointF textPos = planet2D + QPointF(0, pradius + 12);
            
            // Validate text position
            if (isValidPoint(textPos)) {
                // Draw semi-transparent background for readability
                QFontMetrics fm(font);
                int textWidth = fm.horizontalAdvance(planetName);
                int textHeight = fm.height();
                QRectF textBg(textPos.x() - textWidth/2 - 2, textPos.y() - textHeight/2, 
                             textWidth + 4, textHeight);
                
                // Draw background
                QColor bgColor(0, 0, 0, 180);  // Semi-transparent black
                p.fillRect(textBg, bgColor);
                
                // Draw text
                p.setPen(Qt::white);
                p.drawText(textPos.x() - textWidth/2, textPos.y() + textHeight/3, planetName);
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
        QVector3D newVel = planet.getVelocity() + acc * SolarSimConstants::TIME_STEP;
        planet.setVelocity(newVel);
        QVector3D newPos = planet.getPosition() + newVel * SolarSimConstants::TIME_STEP;
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
    // Orbital parameters
    double a = (width()/2) - SolarSimConstants::MARGIN; // semi-major axis
    double b = (height()/2) - SolarSimConstants::MARGIN; // semi-minor axis
    double e = std::sqrt(1.0 - (b*b)/(a*a)); // eccentricity
    // Sun in one of the foci
    QPointF center(width()/2, height()/2);
    double c = e * a;
    sun.setPosition(QVector3D(center.x() - c, center.y(), 0)); // left focus
    struct PlanetParams {
        double a, b, angle_deg, z, vz;
        QColor color;
        double mass, radius;
        QString name;
    };
    std::vector<PlanetParams> planetParams = {
        { a * 0.25, b * 0.25,  0,   0,   0.5, QColor(169, 169, 169), SolarSimConstants::PLANET_MASS * 0.38, SolarSimConstants::PLANET_RADIUS * 0.38, "Mercury" },
        { a * 0.45, b * 0.45, 45,  20,  -0.3, QColor(255, 200, 100), SolarSimConstants::PLANET_MASS * 0.95, SolarSimConstants::PLANET_RADIUS * 0.95, "Venus" },
        { a * 0.65, b * 0.65, 90, -40,   0.2, QColor(70, 120, 255),  SolarSimConstants::PLANET_MASS,        SolarSimConstants::PLANET_RADIUS,        "Earth" },
        { a * 0.80, b * 0.80, 135, 50,   0.4, QColor(200, 100, 50),  SolarSimConstants::PLANET_MASS * 0.53, SolarSimConstants::PLANET_RADIUS * 0.53, "Mars" },
        { a * 1.05, b * 1.05, 180, -60,  -0.2, QColor(180, 140, 80), SolarSimConstants::PLANET_MASS * 318,  SolarSimConstants::PLANET_RADIUS * 11,   "Jupiter" },
        { a * 1.30, b * 1.30, 225, 70,   0.3, QColor(220, 200, 100), SolarSimConstants::PLANET_MASS * 95,   SolarSimConstants::PLANET_RADIUS * 9,    "Saturn" },
        { a * 1.50, b * 1.50, 270, -80,  -0.15, QColor(100, 180, 200), SolarSimConstants::PLANET_MASS * 14,   SolarSimConstants::PLANET_RADIUS * 4,    "Uranus" },
        { a * 1.70, b * 1.70, 315, 40,   0.25, QColor(50, 100, 200), SolarSimConstants::PLANET_MASS * 17,   SolarSimConstants::PLANET_RADIUS * 3.9,  "Neptune" }
    };
    double M = sun.getMass();
    for (const auto& p : planetParams) {
        double rad = qDegreesToRadians(p.angle_deg);
        QVector3D pos = QVector3D(center.x() + p.a * std::cos(rad), center.y() + p.b * std::sin(rad), p.z);
        double r = std::sqrt(std::pow(pos.x() - sun.getPosition().x(), 2) + std::pow(pos.y() - sun.getPosition().y(), 2) + std::pow(pos.z() - sun.getPosition().z(), 2));
        
        // Calculate orbital velocity using vis-viva equation: v = sqrt(GM * (2/r - 1/a))
        // If this produces NaN (when 2/r - 1/a < 0), use circular orbit velocity instead
        double v_virial = 2.0/r - 1.0/p.a;
        double v = 0.0;
        if (v_virial > 0) {
            v = std::sqrt(SolarSimConstants::G * M * v_virial);
        } else {
            // Fallback: use circular orbit velocity v = sqrt(GM/r)
            v = std::sqrt(SolarSimConstants::G * M / r);
        }
        
        double tx = -p.a * std::sin(rad);
        double ty =  p.b * std::cos(rad);
        double tz = 0;
        double norm = std::sqrt(tx*tx + ty*ty + tz*tz);
        QVector3D tangent(tx/norm, ty/norm, p.vz);
        QVector3D tangentNorm = tangent.normalized();
        QVector3D vel = tangentNorm * v;
        planets.append(Planet(pos, vel, p.mass, p.radius, p.color, p.name));
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
