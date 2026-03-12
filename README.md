### Build

```bash
gcc -o nob nob.c
```

### Run

```bash
./nob -run
```

Notes:

- Output binary: `build/main`
- For options, run: `./nob -help`

### Prerequisites

- A C compiler (`gcc`/`clang`/`MSVC`). That's it.
- Technically there are no dependecies for running this project, not even a shell is required. However, since we are displaying a window, we have to link with your window manager at the very least. For this we have raylib, so if the project does not build make sure you satisfies Raylib’s dependencies: https://github.com/raysan5/raylib#build-and-installation
- Note the build system of this project builds Raylib for you, you just need the dependencies installed and exposed via the environment (e.g PATH).
