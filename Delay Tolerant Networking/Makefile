#################################################
##              In the name of GOD
##
## Copyright © 2009-2014 Behnam Momeni.
## All rights reserved.
##
## This file is part of the Partov project.
##
#################################################

CXX = g++
FLAGS = -Wall -g -Ibase -Iuser-common
LIBS = -lpthread

TARGET_BUS = bus.out
TARGET_CITY = city.out
TARGET = ${TARGET_BUS} ${TARGET_CITY}

BASE_SOURCES = base/main.cpp \
	base/cf.cpp \
	base/interface.cpp \
	base/machine.cpp \
	base/frame.cpp

SUB_NAME = PA2

# DO NOT LIST .H FILES HERE! JUST .C AND .CPP FILES
USER_COMMON_SOURCES = user-common/message.cpp \
	user-common/utils.cpp \
	user-common/ip.cpp \
	user-common/udp.cpp

USER_BUS_SOURCES = user-bus/sm.cpp \
	user-bus/facade.cpp \
	user-bus/bus.cpp \
	user-bus/utils-bus.cpp 

USER_CITY_SOURCES = user-city/sm.cpp \
	user-city/facade.cpp \
	user-city/city.cpp \
	user-city/utils-city.cpp

BASE_OBJ = ${BASE_SOURCES:.cpp=.o}
BASE_OBJECTS = ${BASE_OBJ:.c=.o}

USER_COMMON_OBJ = ${USER_COMMON_SOURCES:.cpp=.o}
USER_COMMON_OBJECTS = ${USER_COMMON_OBJ:.c=.o}
USER_BUS_OBJ = ${USER_BUS_SOURCES:.cpp=.o}
USER_BUS_OBJECTS = ${USER_BUS_OBJ:.c=.o}
USER_CITY_OBJ = ${USER_CITY_SOURCES:.cpp=.o}
USER_CITY_OBJECTS = ${USER_CITY_OBJ:.c=.o}

OBJECTS_BUS = ${BASE_OBJECTS} ${USER_COMMON_OBJECTS} ${USER_BUS_OBJECTS}
OBJECTS_CITY = ${BASE_OBJECTS} ${USER_COMMON_OBJECTS} ${USER_CITY_OBJECTS}

OBJECTS = ${BASE_OBJECTS} ${USER_COMMON_OBJECTS} ${USER_BUS_OBJECTS} ${USER_CITY_OBJECTS}

all: ${TARGET_BUS} ${TARGET_CITY}

${TARGET_BUS}: ${OBJECTS_BUS}
	${CXX} ${FLAGS} -o ${TARGET_BUS} ${OBJECTS_BUS} ${LIBS}

${TARGET_CITY}: ${OBJECTS_CITY}
	${CXX} ${FLAGS} -o ${TARGET_CITY} ${OBJECTS_CITY} ${LIBS}

clean:
	tmpfolder=`echo /tmp/CF-clean-$$$$.removed` && mkdir -p $$tmpfolder && date >a && files="a `for a in ${OBJECTS} ${TARGET} *~ */*~; do if [ -e $$a ]; then echo $$a; fi; done`" && tar cf $$tmpfolder/backup.tar $$files && rm -f $$files

submit:
	zip -r ${SUB_NAME}_`date +%s`.zip user*/*.c user*/*.cpp user*/*.h Makefile

.cpp.o:
	${CXX} ${FLAGS} -c $< -o $@
.c.o:
	${CXX} ${FLAGS} -c $< -o $@

