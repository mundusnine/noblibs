/*
    For git repo: https://github.com/sssooonnnggg/cppdap.git
    git commit:  6eca4dd
*/
#ifndef NOB_H_
#define NOB_STRIP_PREFIX
#include "nob.h"
#endif

const char* CPPDAP_FILE = __FILE__;

#define CPPDAP_PATH "Libraries/cppdap"
#define CPPDAP_INCLUDES CPPDAP_PATH"/include",CPPDAP_PATH"/src",CPPDAP_PATH"/third_party/json/include"
#define CPPDAP_LFLAGS "-lstdc++","-ljsoncpp"
int build_cppdap(File_Paths* end_cmd){
    
    bool result = true;
    Cmd cmd = {0};
    size_t checkpoint = nob_temp_save();

    // @TODO: Add a check to update submodules if they aren't present in the cppdap lib
    // git submodule update --init

    if (!mkdir_if_not_exists("build/" CPPDAP_PATH)) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/" CPPDAP_PATH "/src")) {
        return_defer(false);
    }

    const char* build_folder = "build";
    File_Paths files = {0};
    nob_cc_inputs(&files,
        CPPDAP_PATH "/src/content_stream.cpp",
        CPPDAP_PATH "/src/io.cpp",
//        CPPDAP_PATH"/src/jsoncpp_json_serializer.cpp",
        CPPDAP_PATH "/src/network.cpp",
        CPPDAP_PATH "/src/nlohmann_json_serializer.cpp",
        CPPDAP_PATH "/src/null_json_serializer.cpp",
        CPPDAP_PATH "/src/protocol_events.cpp",
        CPPDAP_PATH "/src/protocol_requests.cpp",
        CPPDAP_PATH "/src/protocol_response.cpp",
        CPPDAP_PATH "/src/protocol_types.cpp",
        CPPDAP_PATH "/src/session.cpp",
        CPPDAP_PATH "/src/socket.cpp",
        CPPDAP_PATH "/src/typeinfo.cpp",
        CPPDAP_PATH "/src/typeof.cpp"
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
            CPPDAP_INCLUDES
        );

        cmd_append(&cmd,"-ggdb3");
        cmd_append(&cmd,"-DCPPDAP_JSON_NLOHMANN");
        cmd_append(&cmd,"-std=c++17");
        
        cmd_append(&cmd,"-c", infile);
        cmd_append(&cmd,"-o",temp);
        if(!nob_cmd_run_sync_and_reset(&cmd)) return_defer(false);
        
    }

defer:
    nob_temp_rewind(checkpoint);
    cmd_free(cmd);
    return result;
}
