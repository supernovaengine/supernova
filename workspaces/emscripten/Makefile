PROJECT_PATH = ../../project
CORE_PLATFORM_PATH = .
CORE_PATH = ../../engine/core
LIBS_PATH = ../../engine/libs

#CFLAGS = -O2 -I. -I../../core -I../common -I../../3rdparty/libpng -I../../3rdparty/zlib -I../../3rdparty/linmath -Wall -Wextra
#LDFLAGS = --llvm-lto 1 --closure 1 --embed-file ../../../assets@/ --compression $(EMSCRIPTEN_ROOT)/third_party/lzma.js/lzma-native,$(EMSCRIPTEN_ROOT)/third_party/lzma.js/lzma-decoder.js,LZMA.decompress
#CFLAGS = -I. -I$(LIBS_PATH)/zlib/ -include web_macros.h -Wall -Wextra
CFLAGS = -I$(CORE_PLATFORM_PATH) -I$(CORE_PATH) -I$(LIBS_PATH)/lua -I$(LIBS_PATH)/luaintf -I$(LIBS_PATH)/libpng -I$(LIBS_PATH)/zlib -I$(LIBS_PATH)/tinyobjloader -include web_macros.h -Wall -Wextra
CXXFLAGS = $(CFLAGS) -std=c++11
ASSETS = --embed-file ../../assets@/
LDFLAGS = $(ASSETS) -s USE_GLFW=3 -s DISABLE_EXCEPTION_CATCHING=0
#LDFLAGS = $(ASSETS)

PROJECT_PATH := $(realpath $(PROJECT_PATH))
CORE_PLATFORM_PATH := $(realpath $(CORE_PLATFORM_PATH))
CORE_PATH := $(realpath $(CORE_PATH))
LIBS_PATH := $(realpath $(LIBS_PATH))


SOURCES := $(wildcard $(CORE_PLATFORM_PATH)/*.cpp)

SOURCES += $(wildcard $(PROJECT_PATH)/*.cpp)
SOURCES += $(wildcard $(PROJECT_PATH)/*/*.cpp)
SOURCES += $(wildcard $(PROJECT_PATH)/*/*/*.cpp)
SOURCES += $(wildcard $(PROJECT_PATH)/*.c)
SOURCES += $(wildcard $(PROJECT_PATH)/*/*.c)
SOURCES += $(wildcard $(PROJECT_PATH)/*/*/*.c)

SOURCES += $(wildcard $(CORE_PATH)/Light.cpp)
SOURCES += $(wildcard $(CORE_PATH)/Mesh.cpp)
SOURCES += $(wildcard $(CORE_PATH)/*.cpp)
SOURCES += $(wildcard $(CORE_PATH)/*/*.cpp)
SOURCES += $(wildcard $(CORE_PATH)/*/*/*.cpp)

SOURCES += $(wildcard $(LIBS_PATH)/*/*.c)
SOURCES += $(wildcard $(LIBS_PATH)/*/*.cc)
SOURCES += $(wildcard $(LIBS_PATH)/*/*.cpp)
SOURCES := $(filter-out $(LIBS_PATH)/lua/lua.c, $(SOURCES))
SOURCES := $(filter-out $(LIBS_PATH)/lua/luac.c, $(SOURCES))

#SOURCES += $(wildcard $(LIBS_PATH)/libpng/*.c)
SOURCES := $(filter-out $(LIBS_PATH)/libpng/pngtest.c, $(SOURCES))

#SOURCES += $(wildcard $(LIBS_PATH)/zlib/*.c)
SOURCES := $(filter-out $(LIBS_PATH)/zlib/gzwrite.c, $(SOURCES))
SOURCES := $(filter-out $(LIBS_PATH)/zlib/gzread.c, $(SOURCES))
SOURCES := $(filter-out $(LIBS_PATH)/zlib/gzlib.c, $(SOURCES))

OBJECTS := $(addprefix obj, $(addsuffix .o,$(basename $(SOURCES))))


TARGET = supernova.html
#TARGET = supernova.js

# Targets start here.
#all: $(TARGET)

$(TARGET): $(OBJECTS)
#	$(CC) $(CXXFLAGS) -o $@ $(LDFLAGS) $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

$(addprefix obj, $(CORE_PLATFORM_PATH))/%.o: $(CORE_PLATFORM_PATH)/%.cpp
		@[ -d $(@D) ] || mkdir -p $(@D)
		$(CXX) -c $< $(CXXFLAGS) -o $@

$(addprefix obj, $(PROJECT_PATH))/%.o: $(PROJECT_PATH)/%.cpp
		@[ -d $(@D) ] || mkdir -p $(@D)
		$(CXX) -c $(CXXFLAGS) $< -o $@

$(addprefix obj, $(PROJECT_PATH))/%.o: $(PROJECT_PATH)/%.c
		@[ -d $(@D) ] || mkdir -p $(@D)
		$(CXX) -c $(CXXFLAGS) $< -o $@

$(addprefix obj, $(CORE_PATH))/%.o: $(CORE_PATH)/%.cpp
		@[ -d $(@D) ] || mkdir -p $(@D)
		$(CXX) -c $< $(CXXFLAGS) -o $@

$(addprefix obj, $(LIBS_PATH))/lua/%.o: $(LIBS_PATH)/lua/%.c
		@[ -d $(@D) ] || mkdir -p $(@D)
		$(CXX) -c $(CXXFLAGS) -x c++ $< -o $@

$(addprefix obj, $(LIBS_PATH))/%.o: $(LIBS_PATH)/%.c
		@[ -d $(@D) ] || mkdir -p $(@D)
		$(CC) -c $(CFLAGS) $< -o $@

$(addprefix obj, $(LIBS_PATH))/%.o: $(LIBS_PATH)/%.cc
		@[ -d $(@D) ] || mkdir -p $(@D)
		$(CXX) -c $(CXXFLAGS) $< -o $@


clean:
	$(RM) $(TARGET) $(OBJECTS)

depend:
	@$(CC) $(CXXFLAGS) -MM $(SOURCES)


.PHONY:	all clean depend
