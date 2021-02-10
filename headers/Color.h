#ifndef COLOR_H
#define COLOR_H

#include <algorithm>
#include <cairomm/cairomm.h>

class Color
{
public:
    Color();
    Color(float r, float g, float b);
    static Color createRed();
    static Color createGreen();
    static Color createBlue();
    static Color createYellow();
    static Color createCyan();
    static Color createWhite();
    static Color createBlack();
    inline float red() const {
        return std::min(std::max(m_red, 0.0f), 1.0f);
    }
    inline float green() const {
        return std::min(std::max(m_green, 0.0f), 1.0f);
    }
    inline float blue() const {
        return std::min(std::max(m_blue, 0.0f), 1.0f);
    }
    void multiply(float ratio);
    void lighter(float value);
    void darker(float value);
    void apply(const Cairo::RefPtr<Cairo::Context> &cr);
private:
    float m_red = 0.0;
    float m_green = 0.0;
    float m_blue = 0.0;
};

#endif // COLOR_H
