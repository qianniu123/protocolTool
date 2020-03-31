QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += timegm=mktime
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += qcustomplot \

SOURCES += \
    JL/jlprotocol.cpp \
    main.cpp \
    plot.cpp \
    protocol.cpp \
    qcustomplot/qcustomplot.cpp \
    sg/sgprotocol.cpp \
    ublox/minmea.c \
    ublox/ublox.cpp \
    widget.cpp

HEADERS += \
    JL/jlprotocol.h \
    plot.h \
    protocol.h \
    qcustomplot/qcustomplot.h \
    sg/sgprotocol.h \
    ublox/minmea.h \
    ublox/ublox.h \
    widget.h

FORMS += \
    plot.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
