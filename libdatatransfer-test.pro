TEMPLATE = app
CONFIG += console c++11 thread
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

include(deployment.pri)
qtcAddDeployment()

INCLUDEPATH += libdatatransfer/include \
                Catch/include \
                /usr/include/eigen3

HEADERS += \
    ../../include/datatransfer/p2p_connector.hpp \
    ../../include/datatransfer/serialization_read_policy.hpp \
    ../../include/datatransfer/serialization.hpp \
    ../../include/datatransfer/serialization_write_policy.hpp \
    mock/uart.hpp \
    mock/communicationlink.hpp \
    serialization_policy.hpp \
    dto.h \
    dto.h

