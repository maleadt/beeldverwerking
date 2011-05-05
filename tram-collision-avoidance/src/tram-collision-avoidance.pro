QT += core gui opengl

CONFIG += link_pkgconfig
PKGCONFIG += opencv

SOURCES += \
    main.cpp \
    trackdetection.cpp \
    glwidget.cpp \
    mainwindow.cpp

HEADERS += \
    trackdetection.h \
    auxiliary.h \
    component.h \
    framefeatures.h \
    featureexception.h \
    glwidget.h \
    mainwindow.h

profile {
    QMAKE_CXXFLAGS_DEBUG += -pg
    QMAKE_LFLAGS_DEBUG += -pg
}

FORMS += \
    mainwindow.ui
