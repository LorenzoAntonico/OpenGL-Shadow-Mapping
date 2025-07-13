CC = g++
CCFLAGS = -O3 -s -DNDEBUG
TARGET = Project1.exe

ifeq ($(OS),Windows_NT)
    INCLUDEDIRS = -ILibraries/include \
                  -ILibraries/include/GLFW \
                  -ILibraries/include/glad \
                  -Iglm

    LIBDIRS = -LLibraries/lib

    LIBS = -lglfw3 -lopengl32 -lgdi32
else
    LIBS = -lglfw -lGL -ldl
endif

SRCS = Main.cpp camera.cpp Cube.cpp Plane.cpp Shader.cpp texture2D.cpp stb_image.cpp \
       Lights/DirectionalLight.cpp Lights/PointLight.cpp Lights/SpotLight.cpp glad.c

OBJS = $(SRCS:.cpp=.o)
OBJS := $(OBJS:.c=.o)

$(TARGET): $(OBJS) 
	$(CC) $(CCFLAGS) $^ $(LIBDIRS) $(LIBS) -o $@
	@echo "Build completata con successo!"

%.o: %.cpp
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $< -o $@

%.o: %.c
	$(CC) -c $(CCFLAGS) $(INCLUDEDIRS) $< -o $@

.PHONY: clean clean-obj all

clean-obj:
	
	@rm -f $(OBJS)

clean:

	@rm -f $(OBJS) $(TARGET)

all: clean $(TARGET)
