#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

#include <string_view>

class BitmapFont {
public:
    sf::Vector2f measure(std::string_view text, int scale = 1) const;

    // Draws an all-caps 5x7 bitmap font.
    void draw(sf::RenderTarget& target,
              std::string_view text,
              sf::Vector2f pos,
              int scale,
              sf::Color color) const;

private:
    static constexpr int GlyphW = 5;
    static constexpr int GlyphH = 7;
    static constexpr int Spacing = 1;

    static unsigned char toUpperAscii(unsigned char c);
    static unsigned char glyphRowBits(unsigned char c, int row);
};
