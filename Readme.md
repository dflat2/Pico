# Single Player Commands for ClassiCube

This project is still _work in progress_ and is not fully documented yet (but most `/help <command>` messages are pretty self-explanatory). _Single Player Commands_ aims to support most building features found on server softwares such as MCGalaxy or LegendCraft.

## Compiling

Before compiling, clone the repository. The `--recurse-submodules` option will also clone the `ClassiCube` submodule (required for resolving header files).

```bash
git clone --recurse-submodules https://github.com/dflat2/SinglePlayerCommandsCC.git
cd SinglePlayerCommandsCC
```

### Linux

I did not have the chance to test compiling on Linux yet, but the following command should work.

```bash
cc `find src/ -name '*.c'` -lm -I. -I./src -o SPCPlugin.so -shared -fPIC
```

### Mac

```bash
gcc `find src/ -name '*.c'` -I. -I./src -o SPCPlugin.dylib -shared -undefined dynamic_lookup
```

If your Mac supports `arm` and if you downloaded ClassiCube from <https://classicube.net>, you will probably need to append `-arch x86_64` (otherwise it will compile for `arm`):

```bash
gcc `find src/ -name '*.c'` -I. -I./src -o SPCPlugin.dylib -shared -undefined dynamic_lookup -arch x86_64
```

### Windows

I did not test compiling on Windows yet. But you may follow the instructions documented in the ClassiCube repository: [Using mingw-w64](https://github.com/UnknownShadow200/ClassiCube/blob/master/doc/plugin-dev.md#using-mingw-w64). Then for the _compiling_ part:

```bash
gcc TestPlugin.c -o TestPlugin.dll -s -shared -L . -lClassiCube
gcc `find src/ -name '*.c'` -I. -I./src -o SPCPlugin.dll -shared -L . -lClassiCube
```

## Installing

To install the plugin, move `SPCPlugin.dylib` (or `.dll` or `.so`) in the `ClassiCube/plugins` directory.
