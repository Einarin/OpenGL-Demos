 em++ -std=c++14 -I ../infrastructure ../infrastructure/*.cpp hello_world.cpp -o hello_world.html -lGLEW -s USE_GLFW=3 --preload-file . -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s USE_WEBGL2=1 -g4 --source-map-base http://localhost:6931/