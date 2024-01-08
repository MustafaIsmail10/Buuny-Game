all:
	g++ main.cpp -o main -g -lglfw -lpthread -lX11 -ldl `pkg-config --cflags --libs freetype2` -lXrandr -lGLEW -lGL -DGL_SILENCE_DEPRECATION -DGLM_ENABLE_EXPERIMENTAL -I.

run: all
	./main

test:
	g++ test.cpp -o main -g -lglfw -lpthread -lX11 -ldl -lXrandr -lGLEW -lGL -DGL_SILENCE_DEPRECATION -DGLM_ENABLE_EXPERIMENTAL -I.
	./test
