#ifndef TILE_H
#define TILE_H

#include <cairomm/cairomm.h>

class Tile {
public:
    enum class TYPE: int {
        NOTHING = -1,
        RED,
        GREEN,
        BLUE,
        YELLOW,
        CYAN,
        LAST_TYPE
    };
    Tile(TYPE type, int offsetX, int offsetY);

    void draw(const Cairo::RefPtr<Cairo::Context> &cr,
              int x, int y, int width, int height);
    void setOffsets(int offsetX, int offsetY);
    int offsetX() const;
    int offsetY() const;
    void rotateRight();
    TYPE type() const;

private:
    TYPE m_type;
    int m_offsetX;
    int m_offsetY;
};

#endif // TILE_H
