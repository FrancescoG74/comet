#pragma once
#include <QVector3D>
#include <QColor>
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
    
    // Setters
    void setPosition(const QVector3D& position) { pos = position; }
    void setMass(double m) { mass = m; }
    void setSprite(const QString& imagePath) { sprite.loadImage(imagePath); }
    
protected:
    AstronomicalBody(const QVector3D& pos, double mass, double radius, QColor color)
        : pos(pos), mass(mass), radius(radius), color(color) {}
    
    QVector3D pos;
    double mass;
    double radius;
    QColor color;
    Sprite sprite;
};

// Concrete class for planets with velocity
class Planet : public AstronomicalBody {
public:
    Planet() : AstronomicalBody(QVector3D(), 0, 0, Qt::white) {}
    Planet(const QVector3D& pos, const QVector3D& vel, double mass, double radius, QColor color)
        : AstronomicalBody(pos, mass, radius, color), vel(vel) {}
    
    QVector3D getVelocity() const { return vel; }
    void setVelocity(const QVector3D& velocity) { vel = velocity; }
    
private:
    QVector3D vel;
};

// Concrete class for the sun - static, no velocity
class Sun : public AstronomicalBody {
public:
    Sun() : AstronomicalBody(QVector3D(), 0, 0, Qt::yellow) {}
    Sun(const QVector3D& pos, double mass, double radius, QColor color)
        : AstronomicalBody(pos, mass, radius, color) {}
};

