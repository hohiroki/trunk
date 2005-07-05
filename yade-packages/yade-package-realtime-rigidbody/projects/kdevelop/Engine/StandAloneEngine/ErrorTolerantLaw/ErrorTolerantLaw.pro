# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./Engine/StandAloneEngine/ErrorTolerantLaw
# Target is a library:  

HEADERS += ErrorTolerantLaw.hpp 
SOURCES += ErrorTolerantLaw.cpp 
LIBS += -lErrorTolerantContactModel \
        -lRigidBodyParameters \
        -lyade-lib-wm3-math \
        -lyade-lib-serialization \
        -lyade-lib-factory \
        -lyade-lib-multimethods \
        -rdynamic 
QMAKE_LIBDIR = ../../../../../bin \
               ../../../../../bin \
               /usr/local/lib/yade/yade-libs/ 
QMAKE_CXXFLAGS_RELEASE += -lpthread \
                          -pthread 
QMAKE_CXXFLAGS_DEBUG += -lpthread \
                        -pthread 
DESTDIR = ../../../../../bin 
CONFIG += debug \
          warn_on \
          dll 
TEMPLATE = lib 