QT += core gui
QT += network
QT += printsupport

INCLUDEPATH += $$[QT_INSTALL_PREFIX]/src/3rdparty/zlib

LIBS += -lz

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(lib/QuaZIP/quazip/quazip.pri)
include(lib/SingleApplication/singleapplication.pri)

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QUAZIP_STATIC
DEFINES += QAPPLICATION_CLASS=QApplication

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES += dist/common/assets/qdarkstyle/style.qrc

SOURCES += \
	src/ksz/kszsignature.cpp \
    src/notetag.cpp \
	src/settings.cpp \
	src/gui/widget/collectionitem.cpp \
	src/main.cpp \
	src/collection.cpp \
	src/serializer.cpp \
	src/utils.cpp \
	src/danbooru/danbooruapi.cpp \
	src/danbooru/danbooruurl.cpp \
	src/download/downloader.cpp \
	src/download/downloadquery.cpp \
	src/ksz/ksz.cpp \
	src/ksz/kszreader.cpp \
	src/ksz/kszwriter.cpp \
	src/data/note.cpp \
	src/data/post.cpp \
	src/gui/window/mainwindow.cpp \
	src/gui/window/collectionwindow.cpp \
	src/gui/window/downloadwindow.cpp \
	src/gui/window/aboutwindow.cpp \
	src/gui/widget/notelabel.cpp \
	src/gui/widget/tooltiplabel.cpp \

HEADERS += \
	src/collection.h \
	src/ksz/kszsignature.h \
    src/notetag.h \
	src/settings.h \
	src/gui/widget/collectionitem.h \
	src/main.h \
	src/serializer.h \
	src/utils.h \
	src/danbooru/danbooruapi.h \
	src/danbooru/danbooruurl.h \
	src/download/downloader.h \
	src/download/downloadquery.h \
	src/ksz/ksz.h \
	src/ksz/kszreader.h \
	src/ksz/kszwriter.h \
	src/data/note.h \
	src/data/post.h \
	src/gui/window/mainwindow.h \
	src/gui/window/collectionwindow.h \
	src/gui/window/downloadwindow.h \
	src/gui/window/aboutwindow.h \
	src/gui/widget/notelabel.h \
	src/gui/widget/tooltiplabel.h \

FORMS += \
	src/gui/window/aboutwindow.ui \
	src/gui/window/collectionwindow.ui \
	src/gui/window/downloadwindow.ui \
	src/gui/window/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Icon
win32: RC_ICONS += suzu.ico
