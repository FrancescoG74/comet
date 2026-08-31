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

void SolarSystem::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    // Re-center the Sun/planets/moons on the new size even while the simulation is paused.
    model.updateCelestialPositions(width(), height());
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
        if (model.isSimulationActive()) {
            for (int i = 0; i < SolarSimConstants::STEPS_PER_FRAME; ++i) model.advance(width(), height());
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
    // Rendering only reads simulation state from the model; it never mutates it.
    const Sun& sun = model.getSun();
    const QVector<Planet>& planets = model.getPlanets();
    const QVector<Satellite>& satellites = model.getSatellites();
    const bool showSatellites = model.getShowSatellites();
    const double sunDisplayRadius = model.getSunDisplayRadius();

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

void SolarSystem::keyPressEvent(QKeyEvent *event) {
    handleKeyPress(event);
}
void SolarSystem::handleKeyPress(QKeyEvent *event) {
    if (controller) {
        controller->handleKeyPress(event);
    }
}

void SolarSystem::initBodies() {
    model.initBodies(width(), height());
}

void SolarSystem::mouseReleaseEvent(QMouseEvent *event) {
    handleMouseRelease(event);
}
void SolarSystem::handleMouseRelease(QMouseEvent *event) {
    if (controller) {
        controller->handleMouseRelease(event);
    }
}
