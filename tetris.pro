TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += link_pkgconfig
PKGCONFIG += gtkmm-3.0

SOURCES += \
        ./src/main.cpp \
        ./src/Color.cpp \
        src/Shape.cpp \
        src/Texts.cpp \
        src/Tile.cpp

HEADERS += \
    ./headers/Color.h \
    ./headers/Tile.h \
    headers/Shape.h \
    headers/Texts.h

QMAKE_CXXFLAGS += -Werror
INCLUDEPATH += ./headers/
