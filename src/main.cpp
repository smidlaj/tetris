#include <iostream>
#include <gtkmm.h>
#include <vector>
#include <cstdlib>
#include <limits>
#include <cairomm/context.h>
#include "Color.h"

class MainWindow: public Gtk::Window {
public:
    MainWindow() {
        set_title("Tetris");
        set_default_size(500, 700);

        m_drawArea.signal_draw().connect(
                    sigc::mem_fun(*this, &MainWindow::draw));
        m_drawArea.set_can_focus();
        m_drawArea.signal_event().connect(
                            sigc::mem_fun(*this, &MainWindow::inputEvent));

        m_box.pack_start(m_drawArea);
        add(m_box);

        show_all();

    }

    void startGame(unsigned int rows, unsigned int columns) {
        srand(static_cast<unsigned int>(time(nullptr)));
        m_rows = rows;
        m_columns = columns;
        m_tiles.clear();
        std::vector<TILE> row(columns, TILE::NOTHING);
        m_tiles.resize(rows, row);

        initShapes();
        nextShape();
        m_timer = Glib::signal_timeout().connect(
                    sigc::mem_fun(*this, &MainWindow::timerTimeout), m_currentSpeed );
    }

private:

    struct Coord {
        int m_xOffset;
        int m_yOffset;
    };

    enum class TILE: int {
        NOTHING = -1,
        RED,
        GREEN,
        BLUE,
        YELLOW,
        CYAN,
        LAST_TYPE
    };
    typedef std::vector<Coord> Shape;

    std::vector<Shape> m_shapes;
    unsigned int m_currentShapeIndex;
    unsigned int m_nextShapeIndex;
    Shape m_currentShape;
    Shape m_nextShape;
    int m_shapeX;
    int m_shapeY;
    TILE m_currentType;
    TILE m_nextType;
    Gtk::DrawingArea m_drawArea;
    Gtk::Box m_box;
    std::vector<std::vector<TILE>> m_tiles;
    sigc::connection m_timer;
    unsigned int m_currentSpeed = 1000;


    unsigned int m_rows;
    unsigned int m_columns;
    const unsigned int m_squareIndex = 4;

    void initShapes() {
        m_shapes = std::vector<Shape>({
            { { 0, -1 },  { 0, 0 },   { -1, 0 },  { -1, 1 } },
              { { 0, -1 },  { 0, 0 },   { 1, 0 },   { 1, 1 } },
              { { 0, -1 },  { 0, 0 },   { 0, 1 },   { 0, 2 } },
              { { -1, 0 },  { 0, 0 },   { 1, 0 },   { 0, 1 } },
              { { 0, 0 },   { 1, 0 },   { 0, 1 },   { 1, 1 } },
              { { -1, -1 }, { 0, -1 },  { 0, 0 },   { 0, 1 } },
              { { 1, -1 },  { 0, -1 },  { 0, 0 },   { 0, 1 } }
        });
    }

    void nextShape() {
        if (m_currentShape.empty()) {
            m_currentShapeIndex = static_cast<unsigned int>(rand()) % m_shapes.size();
            m_currentShape = m_shapes[m_currentShapeIndex];
            m_currentType = static_cast<TILE>((rand() % static_cast<int>(TILE::LAST_TYPE)));
        } else {
            m_currentShapeIndex = m_nextShapeIndex;
            m_currentShape = m_nextShape;
            m_currentType = m_nextType;
        }
        m_nextShapeIndex = static_cast<unsigned int>(rand()) % m_shapes.size();
        m_nextShape = m_shapes[m_nextShapeIndex];
        m_nextType = static_cast<TILE>((rand() % static_cast<int>(TILE::LAST_TYPE)));
        m_shapeX = m_columns / 2;
        m_shapeY = std::numeric_limits<int>::max();
        for (unsigned int i = 0; i < m_currentShape.size(); i++) {
            m_shapeY = std::min(m_shapeY, m_currentShape[i].m_yOffset);
        }
        m_shapeY *= -1;
    }

    bool collison(const Shape & shape, int x, int y) {
        for (unsigned int i = 0; i < shape.size(); i++) {
            int row = shape[i].m_yOffset + y;
            int col = shape[i].m_xOffset + x;
            if (row < 0 or static_cast<unsigned int>(row) >= m_rows) {
                return true;
            }
            if (col < 0 or static_cast<unsigned int>(col) >= m_columns) {
                return true;
            }
            if (m_tiles[static_cast<unsigned int>(row)][static_cast<unsigned int>(col)] != TILE::NOTHING) {
                return true;
            }
        }
        return false;
    }

    void placeShape(const Shape & shape, TILE type, int x, int y) {
        for (unsigned int i = 0; i < shape.size(); i++) {
            int row = shape[i].m_yOffset + y;
            int col = shape[i].m_xOffset + x;
            m_tiles[static_cast<unsigned int>(row)][static_cast<unsigned int>(col)] = type;
        }
    }

    void checkRows() {
        for (int row = static_cast<int>(m_tiles.size()) - 1; row >= 0; ) {
            unsigned int counter = 0;
            for (unsigned int col = 0; col < m_columns; col++) {
                if (m_tiles[static_cast<unsigned int>(row)][col] != TILE::NOTHING) {
                    counter++;
                }
            }
            if (counter == m_columns) {
                for (int row2 = row - 1; row2 >= 0; row2--) {
                    m_tiles[static_cast<unsigned int>(row2 + 1)] = m_tiles[static_cast<unsigned int>(row2)];
                }
            } else {
                row--;
            }
        }
    }

    void down() {
        // collison detection
        if (not collison(m_currentShape, m_shapeX, m_shapeY+1)) {
            m_shapeY++;
            m_timer.disconnect();
            m_timer = Glib::signal_timeout().connect(
                        sigc::mem_fun(*this, &MainWindow::timerTimeout), m_currentSpeed );
        } else {
            placeShape(m_currentShape, m_currentType, m_shapeX, m_shapeY);
            checkRows();
            nextShape();
        }
        m_drawArea.queue_draw();
    }

    void rotateRight() {
        if (m_currentShapeIndex == m_squareIndex) {
            return;
        }
        Shape rotated;
        for (unsigned int i = 0; i < m_currentShape.size(); i++) {
            rotated.push_back(
                {-m_currentShape[i].m_yOffset,
                  m_currentShape[i].m_xOffset} );
        }
        if (not collison(rotated, m_shapeX, m_shapeY)) {
            m_currentShape = rotated;
        }
        m_drawArea.queue_draw();
    }

    void rotateLeft() {

        m_drawArea.queue_draw();
    }

    void moveRight() {
        if (not collison(m_currentShape, m_shapeX+1, m_shapeY)) {
            m_shapeX++;
        }
        m_drawArea.queue_draw();
    }

    void moveLeft() {
        if (not collison(m_currentShape, m_shapeX-1, m_shapeY)) {
            m_shapeX--;
        }
        m_drawArea.queue_draw();
    }

    bool inputEvent(const GdkEvent * event) {
        if (event->type == GDK_KEY_PRESS) {
            if (event->key.keyval == GDK_KEY_Down) {
                down();
            }
            if (event->key.keyval == GDK_KEY_Up) {
                rotateRight();
            }
            if (event->key.keyval == GDK_KEY_Right) {
                moveRight();
            }
            if (event->key.keyval == GDK_KEY_Left) {
                moveLeft();
            }

        }
        return false;
    }

    bool timerTimeout() {
        down();

        return true;
    }

    void drawTile(const Cairo::RefPtr<Cairo::Context> &cr, int x, int y, int width, int height, TILE tile) {
        int border = static_cast<int>(std::min(width, height) * 0.2);
        Color color;
        switch (tile) {
        case TILE::LAST_TYPE:
        case TILE::NOTHING:
            return;
        case TILE::RED:
            color = Color::createRed();
            break;
        case TILE::GREEN:
            color = Color::createGreen();
            break;
        case TILE::BLUE:
            color = Color::createBlue();
            break;
        case TILE::YELLOW:
            color = Color::createYellow();
            break;
        case TILE::CYAN:
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

    bool draw(const Cairo::RefPtr<Cairo::Context> &cr) {
        const int width = m_drawArea.get_width();
        const int height = m_drawArea.get_height();
        cr->set_source_rgb(0, 0, 0);
        cr->rectangle(0, 0, width, height);
        cr->fill();
        cr->stroke();

        const float tileSize = std::min( static_cast<float>(width) / m_columns, static_cast<float>(height) / m_rows );

        for (unsigned int row = 0; row < m_rows; row++) {
            int y = static_cast<int>(row * tileSize);
            int sizeY = static_cast<int>((row + 1) * tileSize) - y;
            for (unsigned int col = 0; col < m_columns; col++) {
                int x = static_cast<int>(col * tileSize);
                int sizeX = static_cast<int>((col + 1) * tileSize) - x;
                drawTile(cr, x, y, sizeX, sizeY, m_tiles[row][col]);
            }
        }
        for (unsigned int i = 0; i < m_currentShape.size(); i++) {
            int row = m_currentShape[i].m_yOffset + m_shapeY;
            int col = m_currentShape[i].m_xOffset + m_shapeX;
            int y = static_cast<int>(row * tileSize);
            int x = static_cast<int>(col * tileSize);
            int sizeY = static_cast<int>((row + 1) * tileSize) - y;
            int sizeX = static_cast<int>((col + 1) * tileSize) - x;
            drawTile(cr, x, y, sizeX, sizeY, m_currentType);
        }

        return true;
    }
};

int main(int argc, char ** argv)
{
    auto app = Gtk::Application::create("org.smidla.tetris");
    MainWindow window;
    window.startGame(20, 15);
    return app->run(window, argc, argv);
}
