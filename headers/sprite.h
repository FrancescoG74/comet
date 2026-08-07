#pragma once
#include <QString>
#include <QPixmap>
#include <QImage>

class Sprite {
public:
    Sprite();
    explicit Sprite(const QString& imagePath);
    
    // Load image from file
    bool loadImage(const QString& imagePath);
    
    // Getters
    const QString& getImagePath() const { return imagePath; }
    const QPixmap& getPixmap() const { return pixmap; }
    const QImage& getImage() const { return image; }
    bool isLoaded() const { return loaded; }
    
    // Setters
    void setImagePath(const QString& path);
    
private:
    QString imagePath;
    QPixmap pixmap;
    QImage image;
    bool loaded = false;
};
