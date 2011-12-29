#-------------------------------------------------
#
# Project created by QtCreator 2011-12-25T12:36:03
#
#-------------------------------------------------

QT       += core gui

TARGET = ro3
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x -fopenmp

LIBS += -lgomp

SOURCES += main.cpp \
	GaussianFilter.cpp \
	EdgeDetector.cpp \
	EllipseExtractor.cpp \
    DilationFilter.cpp \
    ThresholdFilter.cpp \
    BinaryOpFilter.cpp

HEADERS += \
	GaussianFilter.h \
	EdgeDetector.h \
	EllipseExtractor.h \
    DilationFilter.h \
    ThresholdFilter.h \
    BinaryOpFilter.h
