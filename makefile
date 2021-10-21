CXX=g++
includePath="./include"

CDEFFLAGS=-std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wunused-variable -mwindows -I$(includePath)
CDEBFLAGS=-g -O0
CFLAGS=-O3 -Wl,--strip-all,--build-id=none,--gc-sections -fno-ident
LIB=-municode -lgdi32 -static

OBJ=obj
DOBJ=objd
SRC=src

TARGET=GameOfLife

default: debug

$(OBJ):
	mkdir $(OBJ)
$(DOBJ):
	mkdir $(DOBJ)

$(OBJ)/%.cpp.o: $(SRC)/%.cpp
	$(CXX) $(CDEFFLAGS) -c $^ -o $@ $(CFLAGS)
$(OBJ)/resources.rc.o: $(SRC)/resources.rc $(SRC)/xpstyles.manifest
	windres -i $(SRC)/resources.rc -o $@


$(DOBJ)/%.cpp.o: $(SRC)/%.cpp
	$(CXX) $(CDEFFLAGS) -c $^ -o $@ $(CDEBFLAGS)
$(DOBJ)/resources.rc.o: $(SRC)/resources.rc $(SRC)/xpstyles.manifest
	windres -i $(SRC)/resources.rc -o $@

srcs = $(wildcard $(SRC)/*.cpp)
srcs += $(wildcard $(SRC)/*.rc)
srcs := $(subst $(SRC)/,,$(srcs))
objs_d = $(srcs:%=$(DOBJ)/%.o)
objs_r = $(srcs:%=$(OBJ)/%.o)

debug_obj: $(objs_d)
	$(CXX) $^ -o deb$(TARGET).exe $(CDEBFLAGS) $(LIB)

release_obj: $(objs_r)
	$(CXX) $^ -o $(TARGET).exe $(CFLAGS) $(LIB)

debug: $(DOBJ) debug_obj
release: $(OBJ) release_obj


clean.o:
	IF EXIST $(OBJ) rd /s /q $(OBJ)
	IF EXIST $(DOBJ) rd /s /q $(DOBJ)

clean: clean.o
	del *.exe
