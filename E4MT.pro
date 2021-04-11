################################################################################
#   QBuildSystem
#
#   Copyright(c) 2021 by Targoman Intelligent Processing <http://tip.co.ir>
#
#   Redistribution and use in source and binary forms are allowed under the
#   terms of BSD License 2.0.
################################################################################
include ($$QBUILD_PATH/templates/appConfigs.pri)

# +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
HEADERS += \
    src/appE4MT.h \
    src/Configs.h \
    src/XMLReader.h \
    3rdParty/fastText/src/args.h \
    3rdParty/fastText/src/autotune.h \
    3rdParty/fastText/src/densematrix.h \
    3rdParty/fastText/src/dictionary.h \
    3rdParty/fastText/src/fasttext.h \
    3rdParty/fastText/src/loss.h \
    3rdParty/fastText/src/matrix.h \
    3rdParty/fastText/src/meter.h \
    3rdParty/fastText/src/model.h \
    3rdParty/fastText/src/productquantizer.h \
    3rdParty/fastText/src/quantmatrix.h \
    3rdParty/fastText/src/real.h \
    3rdParty/fastText/src/utils.h \
    3rdParty/fastText/src/vector.h \
    src/clsFormalityChecker.h

# +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#
SOURCES += \
    src/main.cpp \
    src/appE4MT.cpp \
    src/Configs.cpp \
    src/XMLReader.cpp \
    3rdParty/fastText/src/args.cc \
    3rdParty/fastText/src/autotune.cc \
    3rdParty/fastText/src/densematrix.cc \
    3rdParty/fastText/src/dictionary.cc \
    3rdParty/fastText/src/fasttext.cc \
    3rdParty/fastText/src/loss.cc \
    3rdParty/fastText/src/matrix.cc \
    3rdParty/fastText/src/meter.cc \
    3rdParty/fastText/src/model.cc \
    3rdParty/fastText/src/productquantizer.cc \
    3rdParty/fastText/src/quantmatrix.cc \
    3rdParty/fastText/src/utils.cc \
    3rdParty/fastText/src/vector.cc \
    src/clsFormalityChecker.cpp

OTHER_FILES += \
    README.md \
    INSTALL \
    LICENSE \
    LICENSE.note

# to fix buggy installation location of libxml2 on ubunut and mint
DependencySearchPaths+=/usr/lib/x86_64-linux-gnu/
INCLUDEPATH+=/usr/include/libxml2/
