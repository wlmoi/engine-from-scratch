render=src/engine/render/render.c src/engine/render/render_init.c src/engine/render/render_util.c
io=src/engine/io/io.c
config=src/engine/config/config.c
input=src/engine/input/input.c
time=src/engine/time/time.c
physics=src/engine/physics/physics.c
array_list=src/engine/array_list/array_list.c
entity=src/engine/entity/entity.c
animation=src/engine/animation/animation.c
audio=src/engine/audio/audio.c
files=deps/src/glad.c src/main.c src/engine/global.c $(render) $(io) $(config) $(input) $(time) $(physics) $(array_list) $(entity) $(animation) $(audio)

# Windows-compatible SDL2 and GLFW flags (adjust paths as needed)
SDL2_CFLAGS=-IC:/msys64/mingw64/include/SDL2
SDL2_LIBS=-LC:/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer
GLFW_LIBS=-LC:/msys64/mingw64/lib -lglfw3 -lopengl32 -lgdi32

libs=-lm $(SDL2_CFLAGS) $(SDL2_LIBS) $(GLFW_LIBS)

build:
	gcc -g3 -O0 -I./deps/include $(files) $(libs) -o mygame.exe
