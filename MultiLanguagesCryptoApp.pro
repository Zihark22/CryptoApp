TEMPLATE = app

QT += qml quick widgets

CONFIG += c++11

SOURCES += main.cpp \
    createfile.cpp \
    cryptage.cpp \
    findfiles.cpp \
    qmltranslator.cpp \
    stegano.cpp

lupdate_only {
    SOURCES += main.qml
}

RESOURCES += qml.qrc \
             images.qrc \
             translations.qrc

TRANSLATIONS += MultiLanguagesCryptoApp_ru_RU.ts \
                MultiLanguagesCryptoApp_en_GB.ts \
                MultiLanguagesCryptoApp_fr_FR.ts\
                MultiLanguagesCryptoApp_es_ES.ts\
                MultiLanguagesCryptoApp_al_AL.ts


HEADERS += \
    createfile.h \
    cryptage.h \
    findfiles.h \
    librairies.h \
    qmltranslator.h \
    stegano.h

DISTFILES += \
    MultiLanguagesCryptoApp_en_GB.ts \
    MultiLanguagesCryptoApp_es_ES.ts \
    MultiLanguagesCryptoApp_fr_FR.ts \
    MultiLanguagesCryptoApp_ru_RU.ts \
    MultiLanguagesCryptoApp_al_AL.ts \
    images/copy.png \
    images/cut.png \
    images/new.png \
    images/open.png \
    images/paste.png \
    images/save.png \
    steganoFonctions.py
