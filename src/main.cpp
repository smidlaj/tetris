#include <iostream>
#include <gtkmm.h>
#include <vector>
#include <cstdlib>
#include <limits>
#include <cairomm/context.h>
#include <Color.h>
#include <Shape.h>
#include <Texts.h>

class MainWindow: public Gtk::Window {
public:
    MainWindow() {
        set_title("Tetris");
        set_default_size(800, 700);

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
        std::vector<Tile> row(columns, Tile(Tile::TYPE::NOTHING, 0, 0));
        m_tiles.resize(rows, row);

        initShapes();
        nextShape();
        m_timer = Glib::signal_timeout().connect(
                    sigc::mem_fun(*this, &MainWindow::timerTimeout), m_currentSpeed );
    }

private:
    std::vector<std::vector<std::pair<int, int>>> m_shapes;
    unsigned int m_currentShapeIndex;
    unsigned int m_nextShapeIndex;
    Shape m_currentShape;
    Shape m_nextShape;
    Gtk::DrawingArea m_drawArea;
    Gtk::Box m_box;
    std::vector<std::vector<Tile>> m_tiles;
    sigc::connection m_timer;
    unsigned int m_currentSpeed = 1000;


    unsigned int m_rows;
    unsigned int m_columns;
    const unsigned int m_squareIndex = 4;

    void initShapes() {
        m_shapes = std::vector<std::vector<std::pair<int, int>>>({
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
            Tile::TYPE currentType = static_cast<Tile::TYPE>((rand() % static_cast<int>(Tile::TYPE::LAST_TYPE)));
            m_currentShape = Shape(m_shapes[m_currentShapeIndex], currentType, m_columns / 2);
        } else {
            m_currentShapeIndex = m_nextShapeIndex;
            m_currentShape = m_nextShape;
            m_currentShape.setX(m_columns / 2);
        }
        m_nextShapeIndex = static_cast<unsigned int>(rand()) % m_shapes.size();
        Tile::TYPE m_nextType = static_cast<Tile::TYPE>((rand() % static_cast<int>(Tile::TYPE::LAST_TYPE)));
        m_nextShape = Shape(m_shapes[m_nextShapeIndex], m_nextType, 0);
    }

    bool collison(Shape & shape, int offsetX, int offsetY) {
        bool detected = false;
        shape.forallTile([&](int row, int col, Tile &){
            row += offsetY;
            col += offsetX;
            if (row < 0 or static_cast<unsigned int>(row) >= m_rows) {
                detected = true;
                return;
            }
            if (col < 0 or static_cast<unsigned int>(col) >= m_columns) {
                detected = true;
                return;
            }
            if (m_tiles[static_cast<unsigned int>(row)][static_cast<unsigned int>(col)].type() != Tile::TYPE::NOTHING) {
                detected = true;
                return;
            }
        });
        return detected;
    }

    void placeShape(Shape & shape) {
        shape.forallTile([&](int row, int col, Tile & tile){
            Tile copyTile = tile;
            tile.setOffsets(0, 0);
            m_tiles[static_cast<unsigned int>(row)][static_cast<unsigned int>(col)] = copyTile;
        });
    }

    void checkRows() {
        for (int row = static_cast<int>(m_tiles.size()) - 1; row >= 0; ) {
            unsigned int counter = 0;
            for (unsigned int col = 0; col < m_columns; col++) {
                if (m_tiles[static_cast<unsigned int>(row)][col].type() != Tile::TYPE::NOTHING) {
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
        if (not collison(m_currentShape, 0, 1)) {
            m_currentShape.moveDown();
            m_timer.disconnect();
            m_timer = Glib::signal_timeout().connect(
                        sigc::mem_fun(*this, &MainWindow::timerTimeout), m_currentSpeed );
        } else {
            placeShape(m_currentShape);
            checkRows();
            nextShape();
        }
        m_drawArea.queue_draw();
    }

    void rotateRight() {
        if (m_currentShapeIndex == m_squareIndex) {
            return;
        }
        Shape rotated = m_currentShape;
        rotated.rotateRight();
        if (not collison(rotated, 0, 0)) {
            m_currentShape = rotated;
        }
        m_drawArea.queue_draw();
    }

    void rotateLeft() {

        m_drawArea.queue_draw();
    }

    void moveRight() {
        if (not collison(m_currentShape, 1, 0)) {
            m_currentShape.moveRight();
        }
        m_drawArea.queue_draw();
    }

    void moveLeft() {
        if (not collison(m_currentShape, -1, 0)) {
            m_currentShape.moveLeft();
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

    void drawTiles(const Cairo::RefPtr<Cairo::Context> &cr,
                   int startX, int startY, int width, int height) {

        const float tileSize = std::min( static_cast<float>(width) / m_columns, static_cast<float>(height) / m_rows );

        for (unsigned int row = 0; row < m_rows; row++) {
            int y = static_cast<int>(row * tileSize);
            int sizeY = static_cast<int>((row + 1) * tileSize) - y;
            for (unsigned int col = 0; col < m_columns; col++) {
                int x = static_cast<int>(col * tileSize);
                int sizeX = static_cast<int>((col + 1) * tileSize) - x;
                m_tiles[row][col].draw(cr, x + startX, y + startY, sizeX, sizeY);
            }
        }
        m_currentShape.draw(cr, startX, startY, tileSize);

    }

    void drawNext(const Cairo::RefPtr<Cairo::Context> &cr,
                   int startX, int startY,
                  int width, int height, float tileSize) {
        const int borderWidth = 3;
        cr->set_source_rgb(1, 1, 1);
        cr->set_line_width(borderWidth);
        cr->rectangle(startX - borderWidth,
                      startY - borderWidth,
                      width + 2 * borderWidth,
                      height + 2 * borderWidth);
        cr->stroke();

        int shapeWidth;
        int shapeHeight;
        int offsetX;
        int offsetY;
        m_nextShape.drawGeometry(tileSize, &shapeWidth, &shapeHeight,
                         &offsetX, &offsetY);
        m_nextShape.draw(cr, startX - offsetX + (width - shapeWidth) / 2,
                         startY - offsetY + (height - shapeHeight) / 2, tileSize);

        Cairo::TextExtents extents;
        cr->set_font_size( get_width() * 0.03 );
        cairo_select_font_face(cr->cobj(), "Purisa",
           CAIRO_FONT_SLANT_NORMAL,
           CAIRO_FONT_WEIGHT_BOLD);
        cr->get_text_extents(Texts::nextShape(), extents);

        cr->set_source_rgb(1, 1, 1);
        cr->move_to(startX, startY - extents.height);
        cr->show_text(Texts::nextShape());
        cr->stroke();
    }

    bool draw(const Cairo::RefPtr<Cairo::Context> &cr) {
        const int width = m_drawArea.get_width();
        const int height = m_drawArea.get_height();

        // background
        cr->set_source_rgb(0, 0, 0);
        cr->rectangle(0, 0, width, height);
        cr->fill();
        cr->stroke();

        int borderWidth = 3;
        cr->set_source_rgb(1, 1, 1);
        cr->set_line_width(borderWidth);
        int playAreaWidth = static_cast<int>(width * 0.5);
        const float tileSize = static_cast<float>(playAreaWidth) / m_columns;
        int playAreaHeight = static_cast<int>(m_rows * tileSize);
        int playAreaX = static_cast<int>(width * 0.1);
        int playAreaY = static_cast<int>(height * 0.1);
        cr->rectangle(playAreaX - borderWidth,
                      playAreaY - borderWidth,
                      playAreaWidth + 2 * borderWidth,
                      playAreaHeight + 2 * borderWidth);
        cr->stroke();


        drawTiles(cr, playAreaX, playAreaY, playAreaWidth, playAreaHeight);

        int nextAreaX = static_cast<int>(width * 0.7);
        int nextAreaY = static_cast<int>(height * 0.15);
        int nextAreaWidth = static_cast<int>(width * 0.2);
        int nextAreaHeight = static_cast<int>(tileSize * 5);
        drawNext(cr, nextAreaX, nextAreaY, nextAreaWidth, nextAreaHeight, tileSize);

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
