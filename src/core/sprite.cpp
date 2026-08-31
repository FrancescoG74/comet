#include "core/sprite.h"

namespace {
// Pixels darker than this are treated as background and keyed out.
constexpr int kBlackLuminanceThreshold = 30;
// Zooming produces a new width every frame, so bound the cache instead of letting it grow.
constexpr int kMaxCachedScales = 64;
}

Sprite::Sprite() : loaded(false) {
}

Sprite::Sprite(const QString& imagePath) {
    loadImage(imagePath);
}

bool Sprite::loadImage(const QString& imagePath) {
    this->imagePath = imagePath;
    invalidateCache();

    if (image.load(imagePath)) {
        pixmap = QPixmap::fromImage(image);
        loaded = true;
        return true;
    }

    pixmap = QPixmap();
    loaded = false;
    return false;
}

void Sprite::setImagePath(const QString& path) {
    loadImage(path);
}

QPixmap Sprite::scaledPixmap(int width, bool alphaKeyBlack) const {
    if (!loaded || width <= 0) return QPixmap();

    if (alphaKeyBlack != cacheAlphaKeyed) {
        scaledCache.clear();
        cacheAlphaKeyed = alphaKeyBlack;
    }

    const auto cached = scaledCache.constFind(width);
    if (cached != scaledCache.constEnd()) return cached.value();

    if (scaledCache.size() >= kMaxCachedScales) scaledCache.clear();

    const QPixmap& source = alphaKeyBlack ? alphaKeyedSource() : pixmap;
    QPixmap scaled = source.scaledToWidth(width, Qt::SmoothTransformation);
    scaledCache.insert(width, scaled);
    return scaled;
}

const QPixmap& Sprite::alphaKeyedSource() const {
    if (!alphaKeyedPixmap.isNull() || pixmap.isNull()) return alphaKeyedPixmap;

    QImage keyed = image.convertToFormat(QImage::Format_ARGB32);
    for (int y = 0; y < keyed.height(); ++y) {
        QRgb* line = reinterpret_cast<QRgb*>(keyed.scanLine(y));
        for (int x = 0; x < keyed.width(); ++x) {
            const QRgb px = line[x];
            const int luminance = (qRed(px) * 299 + qGreen(px) * 587 + qBlue(px) * 114) / 1000;
            if (luminance < kBlackLuminanceThreshold) line[x] = qRgba(0, 0, 0, 0);
        }
    }

    alphaKeyedPixmap = QPixmap::fromImage(keyed);
    return alphaKeyedPixmap;
}

void Sprite::invalidateCache() {
    scaledCache.clear();
    alphaKeyedPixmap = QPixmap();
    cacheAlphaKeyed = false;
}
