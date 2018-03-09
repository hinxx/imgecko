#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#   apt-get install libglfw-dev
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
#

#CXX = g++
#CXX = clang++

EXE = imgecko
SOURCES = main.cpp imgui_impl_glfw_gl3.cpp
SOURCES += usbgecko.cpp
SOURCES += ./imgui/imgui.cpp ./imgui/imgui_demo.cpp ./imgui/imgui_draw.cpp
SOURCES += ./imgui/gl3w/GL/gl3w.c
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))

UNAME_S := $(shell uname -s)


ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS = -lGL `pkg-config --static --libs glfw3`
#	LIBS += -L ./libftd2xx/build -lftd2xx
	LIBS += ../lib/libftd2xx.a

	CXXFLAGS = -lpthread -lrt -Wall -Wextra
	CXXFLAGS += -ggdb3 -O0
	CXXFLAGS += -I .
	CXXFLAGS += -I ./libftd2xx
	CXXFLAGS += -I ./imgui -I ./imgui/gl3w `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

# TODO: Other platforms?
#ifeq ($(UNAME_S), Darwin) #APPLE
#	ECHO_MESSAGE = "Mac OS X"
#	LIBS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
#	#LIBS += -L/usr/local/lib -lglfw3
#	LIBS += -L/usr/local/lib -lglfw
#
#	CXXFLAGS = -I../../ -I../libs/gl3w -I/usr/local/include
#	CXXFLAGS += -Wall -Wformat
#	CFLAGS = $(CXXFLAGS)
#endif
#
#ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
#   ECHO_MESSAGE = "Windows"
#   LIBS = -lglfw3 -lgdi32 -lopengl32 -limm32
#
#   CXXFLAGS = -I../../ -I../libs/gl3w `pkg-config --cflags glfw3`
#   CXXFLAGS += -Wall -Wformat
#   CFLAGS = $(CXXFLAGS)
#endif


%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:./imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:./imgui/gl3w/GL/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)
