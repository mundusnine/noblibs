/*
    For git repo: https://github.com/raylib-extras/rlImGui.git
    git commit:  aba659ea
*/

#ifndef NOB_H_
#define NOB_STRIP_PREFIX
#include "nob.h"
#endif

#include "noblib_raylib.c"

//BEGIN dependencies path
#ifndef RAYLIB_PATH
#define RAYLIB_PATH "Libraries/raylib"
#endif

#ifndef IMGUI_PATH
#define IMGUI_PATH "Libraries/imgui"
#endif
//End dependencies path

#ifndef RLIMGUI_PATH
#define RLIMGUI_PATH "Libraries/rlImGui"
#endif

const char* RLIMGUI_FILE = __FILE__;
#define RLIMGUI_LFLAGS "-lstdc++"
#define RLIMGUI_INCLUDES IMGUI_PATH, RLIMGUI_PATH

bool build_rlImGui(File_Paths* end_cmd)
{
    bool result = true;
    Cmd cmd = {0};
    size_t checkpoint = nob_temp_save();

    if (!mkdir_if_not_exists("build/"IMGUI_PATH)) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/"RLIMGUI_PATH)) {
        return_defer(false);
    }
    
    const char* build_folder = "build";
    File_Paths files = {0};
    nob_cc_inputs(&files,
        //UI
        IMGUI_PATH"/imgui.cpp",
        IMGUI_PATH"/imgui_draw.cpp",
        IMGUI_PATH"/imgui_demo.cpp",
        IMGUI_PATH"/imgui_tables.cpp",
        IMGUI_PATH"/imgui_widgets.cpp",
        RLIMGUI_PATH"/rlImGui.cpp"
        //------------------------------
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
            RAYLIB_INCLUDES,
            IMGUI_PATH,
            RLIMGUI_PATH
        );

        cmd_append(&cmd,"-ggdb3");
        cmd_append(&cmd,"-fPIC");
        cmd_append(&cmd,"-mcrc32");
        
        cmd_append(&cmd,"-c", infile);
        cmd_append(&cmd,"-o",temp);
        if(!nob_cmd_run_sync_and_reset(&cmd)) return_defer(false);
        
    }

defer:
    nob_temp_rewind(checkpoint);
    cmd_free(cmd);
    return result;
}
