#pragma once
#include <QVector3D>
#include <QColor>
#include <QString>
#include "sprite.h"

// Abstract base class for all astronomical objects
class AstronomicalBody {
public:
    virtual ~AstronomicalBody() = default;
    
    // Getters
    QVector3D getPosition() const { return pos; }
    double getMass() const { return mass; }
    double getRadius() const { return radius; }
    QColor getColor() const { return color; }
    const Sprite& getSprite() const { return sprite; }
    QString getName() const { return name; }
    
    // Setters
    void setPosition(const QVector3D& position) { pos = position; }
    void setMass(double m) { mass = m; }
    void setSprite(const QString& imagePath) { sprite.loadImage(imagePath); }
    void setName(const QString& n) { name = n; }
    
protected:
    AstronomicalBody(const QVector3D& pos, double mass, double radius, QColor color, const QString& bodyName = "")
        : pos(pos), mass(mass), radius(radius), color(color), name(bodyName) {}
    
    QVector3D pos;
    double mass;
    double radius;
    QColor color;
    QString name;
    Sprite sprite;
};

// Concrete class for planets with velocity
class Planet : public AstronomicalBody {
public:
    Planet() : AstronomicalBody(QVector3D(), 0, 0, Qt::white, "") {}
    Planet(const QVector3D& pos, const QVector3D& vel, double mass, double radius, QColor color, const QString& name = "")
        : AstronomicalBody(pos, mass, radius, color, name), vel(vel) {}
    
    QVector3D getVelocity() const { return vel; }
    void setVelocity(const QVector3D& velocity) { vel = velocity; }
    
    // Osculating orbit shape/orientation (set every frame for visualization), derived from
    // the current real position+velocity so the drawn orbit always passes through the body.
    void setOrbitalParams(double sma, double ecc, const QVector3D& periapsisDir, const QVector3D& perpDir) {
        semiMajorAxis = sma;
        eccentricity = ecc;
        periapsisDirection = periapsisDir;
        perpendicularDirection = perpDir;
    }
    double getSemiMajorAxis() const { return semiMajorAxis; }
    double getEccentricity() const { return eccentricity; }
    QVector3D getPeriapsisDirection() const { return periapsisDirection; }
    QVector3D getPerpendicularDirection() const { return perpendicularDirection; }
    
private:
    QVector3D vel;
    double semiMajorAxis = 0;
    double eccentricity = 0;
    QVector3D periapsisDirection = QVector3D(1, 0, 0);
    QVector3D perpendicularDirection = QVector3D(0, 1, 0);
};

// Concrete class for the sun - static, no velocity
class Sun : public AstronomicalBody {
public:
    Sun() : AstronomicalBody(QVector3D(), 0, 0, Qt::yellow, "Sun") {}
    Sun(const QVector3D& pos, double mass, double radius, QColor color)
        : AstronomicalBody(pos, mass, radius, color, "Sun") {}
};

// Concrete class for satellites (moons) orbiting around a planet
class Satellite : public AstronomicalBody {
public:
    Satellite() : AstronomicalBody(QVector3D(), 0, 0, Qt::gray, ""), parentPlanet(nullptr) {}
    Satellite(const QVector3D& pos, const QVector3D& vel, double mass, double radius, QColor color, 
              Planet* parent, const QString& name = "")
        : AstronomicalBody(pos, mass, radius, color, name), vel(vel), parentPlanet(parent) {}
    
    QVector3D getVelocity() const { return vel; }
    void setVelocity(const QVector3D& velocity) { vel = velocity; }
    
    // Get the parent planet this satellite orbits
    Planet* getParentPlanet() const { return parentPlanet; }
    void setParentPlanet(Planet* planet) { parentPlanet = planet; }
    
    // Orbital parameters (relative to parent planet)
    void setOrbitalParams(double sma, double ecc, double incl, double planetPosX) {
        semiMajorAxis = sma;
        eccentricity = ecc;
        inclination = incl;
        parentPlanetX = planetPosX;
    }
    double getSemiMajorAxis() const { return semiMajorAxis; }
    double getEccentricity() const { return eccentricity; }
    double getInclination() const { return inclination; }
    double getParentPlanetX() const { return parentPlanetX; }
    
private:
    QVector3D vel;
    Planet* parentPlanet;  // Reference to the parent planet
    double semiMajorAxis = 0;
    double eccentricity = 0;
    double inclination = 0;
    double parentPlanetX = 0;  // X-coordinate of parent planet (for visualization)
};

