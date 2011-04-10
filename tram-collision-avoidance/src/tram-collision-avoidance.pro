CONFIG += link_pkgconfig
PKGCONFIG += opencv

SOURCES += \
    main.cpp \
    trackdetection.cpp

HEADERS += \
    trackdetection.h \
    auxiliary.h \
    component.h \
    framefeatures.h
