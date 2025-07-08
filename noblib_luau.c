#ifndef NOB_H_
#define NOB_STRIP_PREFIX
#include "nob.h"
#endif

//@TODO: Luau as many modules that can be used, in our case we only use what is specified from the readme
// to have a basic runtime with compilation. In the futur we should evaluate

#ifndef LUAU_PATH
#define LUAU_PATH "Libraries/luau"
#endif

const char* LUAU_FILE = __FILE__;

#define LUAU_LFLAGS "-lstdc++"

#define LUAU_INCLUDES LUAU_PATH "/VM/include", LUAU_PATH "/Compiler/include"

bool build_luau(File_Paths* end_cmd)
{
    bool result = true;
    Cmd cmd = {0};
    size_t checkpoint = nob_temp_save();

    if (!mkdir_if_not_exists("build/" LUAU_PATH)) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/" LUAU_PATH "/VM")) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/" LUAU_PATH "/Ast")) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/" LUAU_PATH "/Compiler")) {
        return_defer(false);
    }

    if (!mkdir_if_not_exists("build/" LUAU_PATH "/VM/src")) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/" LUAU_PATH "/Ast/src")) {
        return_defer(false);
    }
    if (!mkdir_if_not_exists("build/" LUAU_PATH "/Compiler/src")) {
        return_defer(false);
    }

    const char* build_folder = "build";
    File_Paths files = {0};
    nob_cc_inputs(&files,
        /*----------------VM START------------*/
        LUAU_PATH"/VM/src/lapi.cpp",
        LUAU_PATH"/VM/src/laux.cpp",
        LUAU_PATH"/VM/src/lbaselib.cpp",
        LUAU_PATH"/VM/src/lbitlib.cpp",
        LUAU_PATH"/VM/src/lbuffer.cpp",
        LUAU_PATH"/VM/src/lbuflib.cpp",
        LUAU_PATH"/VM/src/lbuiltins.cpp",
        LUAU_PATH"/VM/src/lcorolib.cpp",
        LUAU_PATH"/VM/src/ldblib.cpp",
        LUAU_PATH"/VM/src/ldebug.cpp",
        LUAU_PATH"/VM/src/ldo.cpp",
        LUAU_PATH"/VM/src/lfunc.cpp",
        LUAU_PATH"/VM/src/lgc.cpp",
        LUAU_PATH"/VM/src/lgcdebug.cpp",
        LUAU_PATH"/VM/src/linit.cpp",
        LUAU_PATH"/VM/src/lmathlib.cpp",
        LUAU_PATH"/VM/src/lmem.cpp",
        LUAU_PATH"/VM/src/lnumprint.cpp",
        LUAU_PATH"/VM/src/lobject.cpp",
        LUAU_PATH"/VM/src/loslib.cpp",
        LUAU_PATH"/VM/src/lperf.cpp",
        LUAU_PATH"/VM/src/lstate.cpp",
        LUAU_PATH"/VM/src/lstring.cpp",
        LUAU_PATH"/VM/src/lstrlib.cpp",
        LUAU_PATH"/VM/src/ltable.cpp",
        LUAU_PATH"/VM/src/ltablib.cpp",
        LUAU_PATH"/VM/src/ltm.cpp",
        LUAU_PATH"/VM/src/ludata.cpp",
        LUAU_PATH"/VM/src/lutf8lib.cpp",
        LUAU_PATH"/VM/src/lveclib.cpp",
        LUAU_PATH"/VM/src/lvmexecute.cpp",
        LUAU_PATH"/VM/src/lvmload.cpp",
        LUAU_PATH"/VM/src/lvmutils.cpp",
        /*----------------VM END------------*/
        LUAU_PATH"/Ast/src/Allocator.cpp",
        LUAU_PATH"/Ast/src/Ast.cpp",
        LUAU_PATH"/Ast/src/Confusables.cpp",
        LUAU_PATH"/Ast/src/Cst.cpp",
        LUAU_PATH"/Ast/src/Lexer.cpp",
        LUAU_PATH"/Ast/src/Location.cpp",
        LUAU_PATH"/Ast/src/Parser.cpp",
        LUAU_PATH"/Ast/src/StringUtils.cpp",
        LUAU_PATH"/Ast/src/TimeTrace.cpp",
        /*----------------Ast END------------*/
        LUAU_PATH"/Compiler/src/BuiltinFolding.cpp",
        LUAU_PATH"/Compiler/src/Builtins.cpp",
        LUAU_PATH"/Compiler/src/BytecodeBuilder.cpp",
        LUAU_PATH"/Compiler/src/Compiler.cpp",
        LUAU_PATH"/Compiler/src/ConstantFolding.cpp",
        LUAU_PATH"/Compiler/src/CostModel.cpp",
        LUAU_PATH"/Compiler/src/lcode.cpp",
        LUAU_PATH"/Compiler/src/TableShape.cpp",
        LUAU_PATH"/Compiler/src/Types.cpp",
        LUAU_PATH"/Compiler/src/ValueTracking.cpp"
        /*----------------Compiler END------------*/
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
            LUAU_PATH"/Common/include",
            LUAU_PATH"/VM/include",
            LUAU_PATH"/VM/src",
            LUAU_PATH"/Ast/include",
            LUAU_PATH"/Compiler/include"
        );

        cmd_append(&cmd,"-fPIC");
        cmd_append(&cmd,"-std=c++17");
        cmd_append(&cmd,"-DLUAU");
        cmd_append(&cmd,"-DLUA_VECTOR_SIZE=4");
        
        cmd_append(&cmd,"-c", infile);
        cmd_append(&cmd,"-o",temp);
        if(!nob_cmd_run_sync_and_reset(&cmd)) return_defer(false);
        
    }

defer:
    nob_temp_rewind(checkpoint);
    cmd_free(cmd);
    return result;
}

