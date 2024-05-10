BUILD_DIR := ./build
SRC_DIRS := ./src

main: src/main.cpp src/renderer.cpp src/shader.cpp res/shaders/container.shader res/shaders/circle.shader src/container.cpp
	g++ -o main -std=c++20 src/main.cpp src/renderer.cpp src/shader.cpp src/container.cpp src/glad.c -lglfw -lGL -lXrandr -lXi -ldl -lX11 -lpthread

clean:
	rm -f main
