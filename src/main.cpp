#include <iostream>
#include <gtkmm.h>
#include <vector>
#include <cstdlib>
#include <limits>
#include <cairomm/context.h>

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

    void startGame(int rows, int columns) {
        srand(time(0));
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
    int m_currentShapeIndex;
    int m_nextShapeIndex;
    Shape m_currentShape;
    Shape m_nextShape;
    int m_shapeX;
    int m_shapeY;
    TILE m_currentType;
    TILE m_nextType;
    sigc::connection m_timer;
    int m_currentSpeed = 1000;

    Gtk::DrawingArea m_drawArea;
    Gtk::Box m_box;

    int m_rows;
    int m_columns;
    std::vector<std::vector<TILE>> m_tiles;

    const int m_squareIndex = 4;

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
            m_currentShapeIndex = rand() % m_shapes.size();
            m_currentShape = m_shapes[m_currentShapeIndex];
            m_currentType = (TILE)(rand() % (int)TILE::LAST_TYPE);
        } else {
            m_currentShapeIndex = m_nextShapeIndex;
            m_currentShape = m_nextShape;
            m_currentType = m_nextType;
        }
        m_nextShapeIndex = rand() % m_shapes.size();
        m_nextShape = m_shapes[m_nextShapeIndex];
        m_nextType = (TILE)(rand() % (int)TILE::LAST_TYPE);
        m_shapeX = m_columns / 2;
        m_shapeY = std::numeric_limits<int>::min();
        for (int i = 0; i < m_currentShape.size(); i++) {
            m_shapeY = std::max(m_shapeY, m_currentShape[i].m_yOffset);
        }
    }

    bool collison(const Shape & shape, int x, int y) {
        for (int i = 0; i < shape.size(); i++) {
            int row = shape[i].m_yOffset + y;
            int col = shape[i].m_xOffset + x;
            if (row < 0 or row >= m_rows) {
                return true;
            }
            if (col < 0 or col >= m_columns) {
                return true;
            }
            if (m_tiles[row][col] != TILE::NOTHING) {
                return true;
            }
        }
        return false;
    }

    void placeShape(const Shape & shape, TILE type, int x, int y) {
        for (int i = 0; i < shape.size(); i++) {
            int row = shape[i].m_yOffset + y;
            int col = shape[i].m_xOffset + x;
            m_tiles[row][col] = type;
        }
    }

    void checkRows() {
        for (int row = m_tiles.size() - 1; row >= 0; ) {
            int counter = 0;
            for (int col = 0; col < m_columns; col++) {
                if (m_tiles[row][col] != TILE::NOTHING) {
                    counter++;
                }
            }
            if (counter == m_columns) {
                for (int row2 = row - 1; row2 >= 0; row2--) {
                    m_tiles[row2 + 1] = m_tiles[row2];
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
        for (int i = 0; i < m_currentShape.size(); i++) {
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
        switch (tile) {
        case TILE::NOTHING:
            return;
        case TILE::RED:
            cr->set_source_rgb(1, 0, 0);
            break;
        case TILE::GREEN:
            cr->set_source_rgb(0, 1, 0);
            break;
        case TILE::BLUE:
            cr->set_source_rgb(1, 0, 1);
            break;
        case TILE::YELLOW:
            cr->set_source_rgb(1, 1, 0);
            break;
        case TILE::CYAN:
            cr->set_source_rgb(0, 1, 1);
            break;
        }
        cr->rectangle(x, y, width, height);
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

        const float tileSize = std::min( (float)width / m_columns, (float)height / m_rows );

        for (int row = 0; row < m_rows; row++) {
            int y = row * tileSize;
            int sizeY = (row + 1) * tileSize - y;
            for (int col = 0; col < m_columns; col++) {
                int x = col * tileSize;
                int sizeX = (col + 1) * tileSize - x;
                drawTile(cr, x, y, sizeX, sizeY, m_tiles[row][col]);
            }
        }
        for (int i = 0; i < m_currentShape.size(); i++) {
            int row = m_currentShape[i].m_yOffset + m_shapeY;
            int col = m_currentShape[i].m_xOffset + m_shapeX;
            int y = row * tileSize;
            int x = col * tileSize;
            int sizeY = (row + 1) * tileSize - y;
            int sizeX = (col + 1) * tileSize - x;
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
