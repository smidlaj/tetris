#include <Tile.h>
#include <Color.h>

Tile::Tile(Tile::TYPE type, int offsetX, int offsetY):
    m_type(type), m_offsetX(offsetX), m_offsetY(offsetY)
{

}

void Tile::draw(const Cairo::RefPtr<Cairo::Context> &cr, int x, int y,
                int width, int height)
{
    int border = static_cast<int>(std::min(width, height) * 0.2);
    Color color;
    switch (m_type) {
    case TYPE::LAST_TYPE:
    case TYPE::NOTHING:
        return;
    case TYPE::RED:
        color = Color::createRed();
        break;
    case TYPE::GREEN:
        color = Color::createGreen();
        break;
    case TYPE::BLUE:
        color = Color::createBlue();
        break;
    case TYPE::YELLOW:
        color = Color::createYellow();
        break;
    case TYPE::CYAN:
        color = Color::createCyan();
        break;
    }
    color.apply(cr);
    cr->rectangle(x, y, width, height);
    cr->fill();
    cr->stroke();

    // left border
    color.lighter(0.25f);
    color.apply(cr);
    cr->move_to(x, y);
    cr->line_to(x + border, y + border);
    cr->line_to(x + border, y + height - border);
    cr->line_to(x, y + height);
    cr->line_to(x, y);
    cr->fill();
    cr->stroke();

    // upper border
    color.lighter(0.25f);
    color.apply(cr);
    cr->move_to(x, y);
    cr->line_to(x + border, y + border);
    cr->line_to(x + width - border, y + border);
    cr->line_to(x + width, y);
    cr->line_to(x, y);
    cr->fill();
    cr->stroke();

    // right border
    color.darker(0.75f);
    color.apply(cr);
    cr->move_to(x + width, y);
    cr->line_to(x + width - border, y + border);
    cr->line_to(x + width - border, y + height - border);
    cr->line_to(x + width, y + height);
    cr->line_to(x + width, y);
    cr->fill();
    cr->stroke();

    // bottom border
    color.darker(0.25f);
    color.apply(cr);
    cr->move_to(x, y + height);
    cr->line_to(x + border, y + height - border);
    cr->line_to(x + width - border, y + height - border);
    cr->line_to(x + width, y + height);
    cr->line_to(x, y + height);
    cr->fill();
    cr->stroke();
}

void Tile::setOffsets(int offsetX, int offsetY)
{
    m_offsetX = offsetX;
    m_offsetY = offsetY;
}

int Tile::offsetX() const
{
    return m_offsetX;
}

int Tile::offsetY() const
{
    return m_offsetY;
}

void Tile::rotateRight()
{
    setOffsets(-m_offsetY, m_offsetX);
}

Tile::TYPE Tile::type() const
{
    return m_type;
}
