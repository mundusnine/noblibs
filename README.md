# noblibs
This project is mainly aimed at researching how we could easily add building libraries for projects that use [nob.h](https://github.com/tsoding/nob.h).

### Show me the code
For raylib:
```sh
wget https://github.com/mundusnine/noblibs/noblib_raylib.c
touch ./nob.c
```
In your nob.c file:
```c
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

// This is optionnal, will default to "build"
// #define BUILD_FOLDER "path/to/build/folder"
// This is optionnal, will default to "Libraries/raylib"
// #define RAYLIB_PATH "path/to/my/cloned/raylib"
// The file will include the implementation by default, if making another noblib_*
// that uses raylib(exemple: rlImGui) and needs it's includes and flags, do :
// #define RAYLIB_NOIMPLEMENTATION 
#include "noblib_raylib.c"

int main(){
  NOB_GO_REBUILD_URSELF_PLUS(argc, argv,RAYLIB_FILE);
  // ^--- Only needed if you intend on changing the build file, use NOB_GO_REBUILD_URSELF otherwise
  if (!mkdir_if_not_exists("build")){ return 1;}
  if (!mkdir_if_not_exists("build/Libraries")) {return 1; }
  // ^---- UPDATE THE PATH IF LIBRARIES ARE SOMEWHERE ELSE
  File_Paths o_files = {0};
  if(!build_raylib(o_files)) return 1;

  Cmd cmd = {0};

  nob_cc(&cmd);
  nob_cc_inputs(&cmd, "./src/main.c");
  cmd_append(&cmd,RAYLIB_INCLUDES);
  for(int i =0; i < o_files.count;++i){
      cmd_append(&cmd,o_files.items[i]);
  }
  nob_cc_output(&cmd, "./my_exe");
  cmd_append(&cmd,RAYLIB_LFLAGS);
  if(!cmd_run_sync_and_reset(&cmd)) return 1;
}
```

### Why ?
As I used `nob.h`, I routinely would have different projects that used, for exemple, raylib. What I would do is copy-paste my code from other build scripts of old projects to my new projects. This can be tedious and error prone, so I wanted a new way to keep the building steps in a centralised place where I can  simply `wget` the build steps and include them in my nobuild script with ease. This issue can also be seen in how tsoding always builds raylib during his streams. Basically, he uses the library releases from raylib instead of building everything from source. But when you want to version your dependencies using git submodules, this method won't suffice. 

### When making noblib files
- Always use `.c` for the extension, as we want to infer that the file will include the implementation for the nob.c file
- Always add a header guard, to forward declare if need be:
```c
#ifndef NOB_H_
#define NOB_STRIP_PREFIX
#include "nob.h"
#endif
```
- Always define `LIBRARYNAME_LFLAGS` and `LIBRARYNAME_INCLUDES` to expose what the end nob.c script needs.
- Automatically create build subdirectories, so users can delete the folder in the build folder for a fresh rebuild of only your library
- When developing a noblib_* always validate that files need a rebuild using: `NOB_GO_REBUILD_URSELF_PLUS`. To enable this to work, add in your noblib_*.c file this:
```c
const char* LIBRARYNAME_FILE = __FILE__;
```
so that your build file looks like: 
```c
int main(int argc, char** argv){
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, LIBRARYNAME_FILE);
}
```
