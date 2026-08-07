#include "../headers/sprite.h"

Sprite::Sprite() : loaded(false) {
}

Sprite::Sprite(const QString& imagePath) {
    loadImage(imagePath);
}

bool Sprite::loadImage(const QString& imagePath) {
    this->imagePath = imagePath;
    
    if (image.load(imagePath)) {
        pixmap = QPixmap::fromImage(image);
        loaded = true;
        return true;
    }
    
    loaded = false;
    return false;
}

void Sprite::setImagePath(const QString& path) {
    loadImage(path);
}
