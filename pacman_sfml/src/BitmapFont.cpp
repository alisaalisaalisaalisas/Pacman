#include "BitmapFont.h"

#include <SFML/Graphics/VertexArray.hpp>

#include <array>

namespace {
// Each row is 5 bits (MSB ignored). Bit 4 is leftmost pixel.
using Glyph = std::array<unsigned char, 7>;

constexpr unsigned char b5(unsigned char v) { return static_cast<unsigned char>(v & 0x1F); }

constexpr Glyph G_blank{0, 0, 0, 0, 0, 0, 0};

// Digits 0-9
constexpr Glyph G_0{b5(0b01110), b5(0b10001), b5(0b10011), b5(0b10101), b5(0b11001), b5(0b10001), b5(0b01110)};
constexpr Glyph G_1{b5(0b00100), b5(0b01100), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b01110)};
constexpr Glyph G_2{b5(0b01110), b5(0b10001), b5(0b00001), b5(0b00010), b5(0b00100), b5(0b01000), b5(0b11111)};
constexpr Glyph G_3{b5(0b01110), b5(0b10001), b5(0b00001), b5(0b00110), b5(0b00001), b5(0b10001), b5(0b01110)};
constexpr Glyph G_4{b5(0b00010), b5(0b00110), b5(0b01010), b5(0b10010), b5(0b11111), b5(0b00010), b5(0b00010)};
constexpr Glyph G_5{b5(0b11111), b5(0b10000), b5(0b11110), b5(0b00001), b5(0b00001), b5(0b10001), b5(0b01110)};
constexpr Glyph G_6{b5(0b00110), b5(0b01000), b5(0b10000), b5(0b11110), b5(0b10001), b5(0b10001), b5(0b01110)};
constexpr Glyph G_7{b5(0b11111), b5(0b00001), b5(0b00010), b5(0b00100), b5(0b01000), b5(0b01000), b5(0b01000)};
constexpr Glyph G_8{b5(0b01110), b5(0b10001), b5(0b10001), b5(0b01110), b5(0b10001), b5(0b10001), b5(0b01110)};
constexpr Glyph G_9{b5(0b01110), b5(0b10001), b5(0b10001), b5(0b01111), b5(0b00001), b5(0b00010), b5(0b01100)};

// Letters A-Z
constexpr Glyph G_A{b5(0b01110), b5(0b10001), b5(0b10001), b5(0b11111), b5(0b10001), b5(0b10001), b5(0b10001)};
constexpr Glyph G_B{b5(0b11110), b5(0b10001), b5(0b10001), b5(0b11110), b5(0b10001), b5(0b10001), b5(0b11110)};
constexpr Glyph G_C{b5(0b01110), b5(0b10001), b5(0b10000), b5(0b10000), b5(0b10000), b5(0b10001), b5(0b01110)};
constexpr Glyph G_D{b5(0b11110), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b11110)};
constexpr Glyph G_E{b5(0b11111), b5(0b10000), b5(0b10000), b5(0b11110), b5(0b10000), b5(0b10000), b5(0b11111)};
constexpr Glyph G_F{b5(0b11111), b5(0b10000), b5(0b10000), b5(0b11110), b5(0b10000), b5(0b10000), b5(0b10000)};
constexpr Glyph G_G{b5(0b01110), b5(0b10001), b5(0b10000), b5(0b10111), b5(0b10001), b5(0b10001), b5(0b01111)};
constexpr Glyph G_H{b5(0b10001), b5(0b10001), b5(0b10001), b5(0b11111), b5(0b10001), b5(0b10001), b5(0b10001)};
constexpr Glyph G_I{b5(0b01110), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b01110)};
constexpr Glyph G_J{b5(0b00001), b5(0b00001), b5(0b00001), b5(0b00001), b5(0b10001), b5(0b10001), b5(0b01110)};
constexpr Glyph G_K{b5(0b10001), b5(0b10010), b5(0b10100), b5(0b11000), b5(0b10100), b5(0b10010), b5(0b10001)};
constexpr Glyph G_L{b5(0b10000), b5(0b10000), b5(0b10000), b5(0b10000), b5(0b10000), b5(0b10000), b5(0b11111)};
constexpr Glyph G_M{b5(0b10001), b5(0b11011), b5(0b10101), b5(0b10101), b5(0b10001), b5(0b10001), b5(0b10001)};
constexpr Glyph G_N{b5(0b10001), b5(0b11001), b5(0b10101), b5(0b10011), b5(0b10001), b5(0b10001), b5(0b10001)};
constexpr Glyph G_O{b5(0b01110), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b01110)};
constexpr Glyph G_P{b5(0b11110), b5(0b10001), b5(0b10001), b5(0b11110), b5(0b10000), b5(0b10000), b5(0b10000)};
constexpr Glyph G_Q{b5(0b01110), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10101), b5(0b10010), b5(0b01101)};
constexpr Glyph G_R{b5(0b11110), b5(0b10001), b5(0b10001), b5(0b11110), b5(0b10100), b5(0b10010), b5(0b10001)};
constexpr Glyph G_S{b5(0b01111), b5(0b10000), b5(0b10000), b5(0b01110), b5(0b00001), b5(0b00001), b5(0b11110)};
constexpr Glyph G_T{b5(0b11111), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b00100)};
constexpr Glyph G_U{b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b01110)};
constexpr Glyph G_V{b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10001), b5(0b01010), b5(0b00100)};
constexpr Glyph G_W{b5(0b10001), b5(0b10001), b5(0b10001), b5(0b10101), b5(0b10101), b5(0b10101), b5(0b01010)};
constexpr Glyph G_X{b5(0b10001), b5(0b10001), b5(0b01010), b5(0b00100), b5(0b01010), b5(0b10001), b5(0b10001)};
constexpr Glyph G_Y{b5(0b10001), b5(0b10001), b5(0b01010), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b00100)};
constexpr Glyph G_Z{b5(0b11111), b5(0b00001), b5(0b00010), b5(0b00100), b5(0b01000), b5(0b10000), b5(0b11111)};

// Punctuation
constexpr Glyph G_dash{b5(0b00000), b5(0b00000), b5(0b00000), b5(0b11111), b5(0b00000), b5(0b00000), b5(0b00000)};
constexpr Glyph G_colon{b5(0b00000), b5(0b00100), b5(0b00100), b5(0b00000), b5(0b00100), b5(0b00100), b5(0b00000)};
constexpr Glyph G_dot{b5(0b00000), b5(0b00000), b5(0b00000), b5(0b00000), b5(0b00000), b5(0b00100), b5(0b00100)};
constexpr Glyph G_bang{b5(0b00100), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b00100), b5(0b00000), b5(0b00100)};
constexpr Glyph G_slash{b5(0b00001), b5(0b00010), b5(0b00100), b5(0b01000), b5(0b10000), b5(0b00000), b5(0b00000)};
constexpr Glyph G_gt{b5(0b10000), b5(0b01000), b5(0b00100), b5(0b00010), b5(0b00100), b5(0b01000), b5(0b10000)};
constexpr Glyph G_qm{b5(0b01110), b5(0b10001), b5(0b00001), b5(0b00010), b5(0b00100), b5(0b00000), b5(0b00100)};

constexpr const Glyph& glyphFor(unsigned char c) {
    if (c >= '0' && c <= '9') {
        switch (c) {
        case '0': return G_0;
        case '1': return G_1;
        case '2': return G_2;
        case '3': return G_3;
        case '4': return G_4;
        case '5': return G_5;
        case '6': return G_6;
        case '7': return G_7;
        case '8': return G_8;
        case '9': return G_9;
        }
    }

    if (c >= 'A' && c <= 'Z') {
        switch (c) {
        case 'A': return G_A;
        case 'B': return G_B;
        case 'C': return G_C;
        case 'D': return G_D;
        case 'E': return G_E;
        case 'F': return G_F;
        case 'G': return G_G;
        case 'H': return G_H;
        case 'I': return G_I;
        case 'J': return G_J;
        case 'K': return G_K;
        case 'L': return G_L;
        case 'M': return G_M;
        case 'N': return G_N;
        case 'O': return G_O;
        case 'P': return G_P;
        case 'Q': return G_Q;
        case 'R': return G_R;
        case 'S': return G_S;
        case 'T': return G_T;
        case 'U': return G_U;
        case 'V': return G_V;
        case 'W': return G_W;
        case 'X': return G_X;
        case 'Y': return G_Y;
        case 'Z': return G_Z;
        }
    }

    switch (c) {
    case '-': return G_dash;
    case ':': return G_colon;
    case '.': return G_dot;
    case '!': return G_bang;
    case '/': return G_slash;
    case '>': return G_gt;
    case '?': return G_qm;
    case ' ': return G_blank;
    default: return G_qm;
    }
}
}

unsigned char BitmapFont::toUpperAscii(unsigned char c) {
    if (c >= 'a' && c <= 'z') {
        return static_cast<unsigned char>(c - 'a' + 'A');
    }
    return c;
}

unsigned char BitmapFont::glyphRowBits(unsigned char c, int row) {
    c = toUpperAscii(c);
    if (row < 0 || row >= GlyphH) {
        return 0;
    }
    return glyphFor(c)[static_cast<std::size_t>(row)];
}

sf::Vector2f BitmapFont::measure(std::string_view text, int scale) const {
    if (scale < 1) scale = 1;
    const float adv = static_cast<float>((GlyphW + Spacing) * scale);
    const float w = text.empty() ? 0.f : adv * static_cast<float>(text.size()) - static_cast<float>(Spacing * scale);
    const float h = static_cast<float>(GlyphH * scale);
    return {w, h};
}

void BitmapFont::draw(sf::RenderTarget& target,
                      std::string_view text,
                      sf::Vector2f pos,
                      int scale,
                      sf::Color color) const {
    if (scale < 1) scale = 1;

    sf::VertexArray quads(sf::Quads);

    const float pixel = static_cast<float>(scale);
    const float adv = static_cast<float>((GlyphW + Spacing) * scale);

    float x = pos.x;
    const float y0 = pos.y;

    for (unsigned char c : text) {
        for (int row = 0; row < GlyphH; ++row) {
            const unsigned char bits = glyphRowBits(c, row);
            for (int col = 0; col < GlyphW; ++col) {
                const int bit = (GlyphW - 1 - col);
                if (((bits >> bit) & 1u) == 0u) {
                    continue;
                }

                const float px = x + static_cast<float>(col) * pixel;
                const float py = y0 + static_cast<float>(row) * pixel;

                quads.append(sf::Vertex({px, py}, color));
                quads.append(sf::Vertex({px + pixel, py}, color));
                quads.append(sf::Vertex({px + pixel, py + pixel}, color));
                quads.append(sf::Vertex({px, py + pixel}, color));
            }
        }

        x += adv;
    }

    target.draw(quads);
}
