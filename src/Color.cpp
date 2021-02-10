#include "Color.h"

Color::Color()
{

}

Color::Color(float r, float g, float b):
    m_red(r), m_green(g), m_blue(b)
{

}

Color Color::createRed()
{
    return Color(1, 0, 0);
}

Color Color::createGreen()
{
    return Color(0, 1, 0);
}

Color Color::createBlue()
{
    return Color(0, 0, 1);
}

Color Color::createYellow()
{
    return Color(1, 1, 0);
}

Color Color::createCyan()
{
    return Color(0, 1, 1);
}

Color Color::createWhite()
{
    return Color(1, 1, 1);
}

Color Color::createBlack()
{
    return Color(0, 0, 0);
}

void Color::multiply(float ratio)
{
    m_red *= ratio;
    m_green *= ratio;
    m_blue *= ratio;
}

void Color::lighter(float value)
{
    m_red += value;
    m_green += value;
    m_blue += value;
}

void Color::darker(float value)
{
    m_red -= value;
    m_green -= value;
    m_blue -= value;
}

void Color::apply(const Cairo::RefPtr<Cairo::Context> &cr)
{
    cr->set_source_rgb(m_red, m_green, m_blue);
}
