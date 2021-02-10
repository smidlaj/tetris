#ifndef SHAPE_H
#define SHAPE_H

#include <cairomm/cairomm.h>
#include <vector>
#include <functional>
#include <Tile.h>

class Shape {
public:
    Shape(const std::vector<std::pair<int, int>> & offsets,
          Tile::TYPE tileType, int x, int y = -1);
    Shape();
    void rotateRight();
    void draw(const Cairo::RefPtr<Cairo::Context> &cr,
              int offsetX, int offsetY, float tileSize);
    void drawGeometry(float tileSize, int * width, int * height,
                      int * startX, int * startY);
    void forallTile(std::function<void(int, int, Tile &)> func);
    void moveDown();
    void moveLeft();
    void moveRight();
    bool empty() const;
    void setX(int x);
private:
    std::vector<Tile> m_tiles;
    int m_x;
    int m_y;
};

#endif // SHAPE_H
