CONFIG += link_pkgconfig
PKGCONFIG += opencv

SOURCES += \
    main.cpp \
    trackdetection.cpp \
    tramdetection.cpp

HEADERS += \
    trackdetection.h \
    auxiliary.h \
    component.h \
    framefeatures.h \
    featureexception.h \
    tramdetection.h

profile {
    QMAKE_CXXFLAGS_DEBUG += -pg
    QMAKE_LFLAGS_DEBUG += -pg
}
