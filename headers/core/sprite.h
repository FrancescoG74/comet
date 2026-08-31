#pragma once
#include <QString>
#include <QPixmap>
#include <QImage>
#include <QHash>

class Sprite {
public:
    Sprite();
    explicit Sprite(const QString& imagePath);
    ~Sprite() = default;
    
    // Copy semantics (Qt objects are copyable and shareable)
    Sprite(const Sprite&) = default;
    Sprite& operator=(const Sprite&) = default;
    
    // Move semantics
    Sprite(Sprite&&) noexcept = default;
    Sprite& operator=(Sprite&&) noexcept = default;
    
    // Load image from file
    bool loadImage(const QString& imagePath);
    
    // Getters
    const QString& getImagePath() const { return imagePath; }
    const QPixmap& getPixmap() const { return pixmap; }
    const QImage& getImage() const { return image; }
    bool isLoaded() const { return loaded; }

    // Render-ready pixmap scaled to `width`, optionally with near-black pixels keyed out.
    // Results are memoised so repaints at a stable zoom level cost a hash lookup.
    QPixmap scaledPixmap(int width, bool alphaKeyBlack = false) const;

    int cachedScaleCount() const { return static_cast<int>(scaledCache.size()); }

    // Setters
    void setImagePath(const QString& path);
    
private:
    const QPixmap& alphaKeyedSource() const;
    void invalidateCache();

    QString imagePath;
    QPixmap pixmap;
    QImage image;
    bool loaded = false;

    mutable QHash<int, QPixmap> scaledCache;
    mutable QPixmap alphaKeyedPixmap;
    mutable bool cacheAlphaKeyed = false;
};
