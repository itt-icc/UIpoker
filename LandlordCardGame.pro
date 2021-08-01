#-------------------------------------------------
#
# Project created by QtCreator 2014-12-14T13:21:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LandlordCardGame
TEMPLATE = app

RC_FILE = LandlordCardGame.rc

SOURCES += main.cpp\
    CallLord.cpp \
    CardPicture.cpp \
    LordControl.cpp \
    LordPanel.cpp \
        MainFrame.cpp \
    MarkBoard.cpp \
    Player.cpp \
    Robot.cpp \
    UserPlayer.cpp \
    Cards.cpp \
    Strategy.cpp \
    PlayHandThread.cpp \
    Hand.cpp

HEADERS  += MainFrame.h \
    CallLord.h \
    CardPicture.h \
    LordControl.h \
    LordPanel.h \
    MarkBoard.h \
    Player.h \
    Robot.h \
    UserPlayer.h \
    Cards.h \
    Strategy.h \
    PlayHandThread.h \
    Hand.h

FORMS    += MainFrame.ui \
    MarkBoard.ui

TRANSLATIONS += zh.ts

RESOURCES += \
    LandlordCardGame.qrc

OTHER_FILES += \
    zh.ts \
    LandlordCardGame.rc \
    stylesheet.qss

