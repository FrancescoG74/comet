#include <catch2/catch_test_macros.hpp>

#include <QImage>
#include <QTemporaryDir>

#include "core/sprite.h"

namespace {

// Half black, half red: the black half is what alpha keying must remove.
QString writeTestImage(const QTemporaryDir& dir) {
    QImage image(8, 8, QImage::Format_ARGB32);
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            image.setPixelColor(x, y, y < 4 ? QColor(0, 0, 0) : QColor(255, 0, 0));
        }
    }
    const QString path = dir.filePath("sprite.png");
    REQUIRE(image.save(path));
    return path;
}

}

TEST_CASE("Sprite reports failure for a missing image", "[sprite]") {
    Sprite sprite;
    REQUIRE_FALSE(sprite.loadImage("/nonexistent/path/to/image.png"));
    REQUIRE_FALSE(sprite.isLoaded());
    REQUIRE(sprite.scaledPixmap(32).isNull());
}

TEST_CASE("Sprite scales to the requested width", "[sprite]") {
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    Sprite sprite(writeTestImage(dir));
    REQUIRE(sprite.isLoaded());

    const QPixmap scaled = sprite.scaledPixmap(32);
    REQUIRE(scaled.width() == 32);
    REQUIRE(scaled.height() == 32);
}

TEST_CASE("Sprite rejects non-positive widths", "[sprite]") {
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    Sprite sprite(writeTestImage(dir));

    REQUIRE(sprite.scaledPixmap(0).isNull());
    REQUIRE(sprite.scaledPixmap(-10).isNull());
    REQUIRE(sprite.cachedScaleCount() == 0);
}

TEST_CASE("Sprite caches one entry per requested width", "[sprite]") {
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    Sprite sprite(writeTestImage(dir));

    sprite.scaledPixmap(16);
    sprite.scaledPixmap(16);
    sprite.scaledPixmap(16);
    REQUIRE(sprite.cachedScaleCount() == 1);

    sprite.scaledPixmap(24);
    REQUIRE(sprite.cachedScaleCount() == 2);
}

TEST_CASE("Sprite cache stays bounded while zooming", "[sprite]") {
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    Sprite sprite(writeTestImage(dir));

    for (int width = 1; width <= 300; ++width) {
        sprite.scaledPixmap(width);
    }
    REQUIRE(sprite.cachedScaleCount() <= 64);
}

TEST_CASE("Sprite drops cached scales when a new image is loaded", "[sprite]") {
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    Sprite sprite(writeTestImage(dir));

    sprite.scaledPixmap(16);
    REQUIRE(sprite.cachedScaleCount() == 1);

    sprite.loadImage(writeTestImage(dir));
    REQUIRE(sprite.cachedScaleCount() == 0);
}

TEST_CASE("Sprite alpha keying turns near-black pixels transparent", "[sprite]") {
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    Sprite sprite(writeTestImage(dir));

    const QImage keyed = sprite.scaledPixmap(8, true).toImage().convertToFormat(QImage::Format_ARGB32);
    REQUIRE(keyed.pixelColor(4, 1).alpha() == 0);
    REQUIRE(keyed.pixelColor(4, 6).alpha() == 255);

    const QImage plain = sprite.scaledPixmap(8, false).toImage().convertToFormat(QImage::Format_ARGB32);
    REQUIRE(plain.pixelColor(4, 1).alpha() == 255);
}

TEST_CASE("Sprite reuses the cache when alpha keying is unchanged", "[sprite]") {
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    Sprite sprite(writeTestImage(dir));

    sprite.scaledPixmap(16, true);
    sprite.scaledPixmap(16, true);
    REQUIRE(sprite.cachedScaleCount() == 1);

    // Switching modes invalidates entries produced for the other mode.
    sprite.scaledPixmap(16, false);
    REQUIRE(sprite.cachedScaleCount() == 1);
}
