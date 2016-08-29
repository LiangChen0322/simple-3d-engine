
#DIR_INC=./include
#DIR_SRC=./src
#DIR_OBJ=./obj
#DIR_BIN=./bin

CXX=g++
RM=rm -f
CXXFLAGS=-std=c++11 `pkg-config gtkmm-3.0 --cflags` -I${DIR_INC}
LDFLAGS=`pkg-config gtkmm-3.0 --libs`

DEPFILE=.depend
DEPFILE_TEST=.depend-test

#SRCS=$(wildcard ${DIR_SRC}/*.c)
#OBJS=$(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))
SRCS=Vec.cpp Mat.cpp Quaternion.cpp Mesh.cpp World.cpp main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

#SRCS_TEST=${DIR_SRC}/test.cpp
#OBJS_TEST=$(subst %.c,${DIR_OBJ}/%.o,$(notdir ${SRCS_TEST}))
SRCS_TEST=test.cpp
OBJS_TEST=$(subst .cpp,.o,$(SRCS_TEST))

all: run

run: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

test: Vec.o Mat.o Quaternion.o $(OBJS_TEST)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.depend: $(SRCS)
	$(RM) ./$(DEPFILE)
	$(CXX) $(CXXFLAGS) -MM $^ > ./$(DEPFILE)

.depend-test: $(SRCS_TEST)
	$(RM) ./$(DEPFILE_TEST)
	$(CXX) $(CXXFLAGS) -MM $^ > ./$(DEPFILE_TEST)

clean:
	$(RM) $(OBJS) $(DEPFILE) $(OBJS_TEST) $(DEPFILE_TEST) run test

sinclude $(DEPFILE)
sinclude $(DEPFILE_TEST)
