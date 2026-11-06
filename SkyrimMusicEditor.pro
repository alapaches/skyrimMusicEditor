QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

CONFIG += c++17 resources_big

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audioconverter.cpp \
    main.cpp \
    mainwindow.cpp \
    musiccatalog.cpp \
    skyriminstall.cpp

HEADERS += \
    audioconverter.h \
    changelogdialog.h \
    mainwindow.h \
    musiccatalog.h \
    skyriminstall.h \
    shoutrplacedialog.h \
    trackdelegate.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc \
    styles.qrc \
    translations.qrc

DISTFILES += \
    skyrim_theme.qss

TRANSLATIONS += \
    translation_en.ts \
    translation_fr.ts \
    translation_ru.ts

audio_files.path = $$OUT_PWD
audio_files.files = $$PWD/audio
INSTALLS += audio_files