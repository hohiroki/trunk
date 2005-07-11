# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./Engine/EngineUnit/PolyhedralSweptSphere2AABB
# Target is a library:  

LIBS += -lPolyhedralSweptSphere \
        -lAABB \
        -rdynamic 
INCLUDEPATH += ../../../DataClass/InteractingGeometry/PolyhedralSweptSphere 
QMAKE_LIBDIR = ../../../../../bin \
               /usr/local/lib/yade/yade-package-common/ \
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
HEADERS += PolyhedralSweptSphere2AABB.hpp 
SOURCES += PolyhedralSweptSphere2AABB.cpp 