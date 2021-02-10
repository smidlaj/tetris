#include <Shape.h>

Shape::Shape(const std::vector<std::pair<int, int> > &offsets,
             Tile::TYPE tileType, int x, int y):
    m_x(x), m_y(y)
{
    for (auto & coord: offsets) {
        m_tiles.push_back(Tile(tileType, coord.first, coord.second));
    }
    if (m_y < 0) {
        m_y = std::numeric_limits<int>::max();
        for (const auto & tile: m_tiles) {
            m_y = std::min(m_y, tile.offsetY());
        }
        m_y *= -1;
        /*for (unsigned int i = 0; i < m_currentShape.size(); i++) {
            m_shapeY = std::min(m_shapeY, m_currentShape[i].m_yOffset);
        }
        m_shapeY *= -1;*/
    }
}

Shape::Shape(): m_x(-1), m_y(-1)
{

}

void Shape::rotateRight()
{
    forallTile([](int, int, Tile & tile)->void{
        tile.rotateRight();
    });
}

void Shape::draw(const Cairo::RefPtr<Cairo::Context> &cr,
                 int offsetX, int offsetY, float tileSize)
{
    forallTile([&](int row, int col, Tile & tile)->void{
        int y = static_cast<int>(row * tileSize);
        int x = static_cast<int>(col * tileSize);
        int height = static_cast<int>((row + 1) * tileSize) - y;
        int width = static_cast<int>((col + 1) * tileSize) - x;
        tile.draw(cr, x + offsetX, y + offsetY, width, height);
    });
}

void Shape::drawGeometry(float tileSize, int *width, int *height,
                         int * startX, int * startY)
{
    *width = 0;
    *height = 0;
    *startX = std::numeric_limits<int>::max();
    *startY = std::numeric_limits<int>::max();
    int endX = std::numeric_limits<int>::min();
    int endY = std::numeric_limits<int>::min();
    forallTile([&](int row, int col, Tile &)->void{
        int y = static_cast<int>(row * tileSize);
        int x = static_cast<int>(col * tileSize);
        *startX = std::min(*startX, x);
        *startY = std::min(*startX, y);
        int height = static_cast<int>((row + 1) * tileSize) - y;
        int width = static_cast<int>((col + 1) * tileSize) - x;
        endX = std::max(endX, x + width);
        endY = std::max(endY, y + height);
    });
    *width = endX - *startX;
    *height = endY - *startY;
}

void Shape::forallTile(std::function<void (int, int, Tile &)> func)
{
    for (auto & tile: m_tiles) {
        int row = tile.offsetY() + m_y;
        int col = tile.offsetX() + m_x;
        func(row, col, tile);
    }
}

void Shape::moveDown()
{
    m_y++;
}

void Shape::moveLeft()
{
    m_x--;
}

void Shape::moveRight()
{
    m_x++;
}

bool Shape::empty() const
{
    return m_tiles.empty();
}

void Shape::setX(int x)
{
    m_x = x;
}
