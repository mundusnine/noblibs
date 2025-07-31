/*
    For git repo: https://github.com/sssooonnnggg/luau-debugger.git
    git commit:  7aac86a
*/
#ifndef NOB_H_
#define NOB_STRIP_PREFIX
#include "nob.h"
#endif

#include "noblib_luau.c"
#include "noblib_cppdap.c"

const char* LUAUD_FILE = __FILE__;

#define LUAUD_BUILD_PATH "Libraries/luau-debugger"
#define LUAUD_PATH "Libraries/luau-debugger/debugger"
#define LUAUD_INCLUDES LUAUD_PATH"/src",LUAUD_PATH"/include"
#define LUAUD_LFLAGS "-lstdc++"

int build_luau_debugger(File_Paths* end_cmd){
    
    bool result = true;

    if (!mkdir_if_not_exists("build/" LUAUD_BUILD_PATH)) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/" LUAUD_BUILD_PATH "/debugger")) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/" LUAUD_BUILD_PATH "/debugger/src")) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/" LUAUD_BUILD_PATH "/debugger/src/internal")) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/" LUAUD_BUILD_PATH "/debugger/src/internal/utils")) {
        return_defer(false);
    }
    if(!build_cppdap(end_cmd)){
        return_defer(false);
    }
    
    Cmd cmd = {0};
    const char* build_folder = "build";
    File_Paths files = {0};
    nob_cc_inputs(&files,
        LUAUD_PATH"/src/debugger.cpp",
        LUAUD_PATH"/src/internal/breakpoint.cpp",
        LUAUD_PATH"/src/internal/file.cpp",
        LUAUD_PATH"/src/internal/debug_bridge.cpp",
        LUAUD_PATH"/src/internal/lua_statics.cpp",
        LUAUD_PATH"/src/internal/variable.cpp",
        LUAUD_PATH"/src/internal/variable_registry.cpp",
        LUAUD_PATH"/src/internal/vm_registry.cpp",
        LUAUD_PATH"/src/internal/utils/lua_utils.cpp",
        LUAUD_PATH"/src/internal/utils/lua_types.cpp",
    );

    da_foreach(const char*, x, &files) {
        const char* infile = temp_sprintf("./%s",*x);
        const char* filepath = temp_strdup(*x); 
        char* temp = strstr(filepath,".");
        temp[0] = '\0';
        temp = temp_sprintf("%s/%s.o",build_folder,filepath);
        cmd_append(end_cmd,strdup(temp));
        if(!nob_needs_rebuild1(temp,infile)){
            continue;
        }
        nob_cc(&cmd,true);
        nob_cc_includes(&cmd,
            LUAUD_INCLUDES,
            CPPDAP_INCLUDES,
            LUAU_INCLUDES
        );
        cmd_append(&cmd,"-ggdb3");
        cmd_append(&cmd,"-std=c++20");
        
        cmd_append(&cmd,"-c", infile);
        cmd_append(&cmd,"-o",temp);
        if(!nob_cmd_run_sync_and_reset(&cmd)) return_defer(false);
        
    }


defer:
    cmd_free(cmd);
    return result;
}
