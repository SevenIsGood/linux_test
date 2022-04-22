TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../netcard.cpp

include(deployment.pri)
qtcAddDeployment()

