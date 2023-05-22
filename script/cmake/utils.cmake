function(PrintMessage print_data)
    message("[MiniMirror] -- ${print_data}")
endfunction()

function(AddModule module_name module_path)
    file(GLOB_RECURSE __module_header_files__ "${module_path}/*.h")
    file(GLOB_RECURSE __module_source_files__ "${module_path}/*.cpp")
    add_library(${module_name} SHARED ${__module_header_files__} ${__module_source_files__})
endfunction()

function(AddExecutable executable_name executable_path)
    file(GLOB_RECURSE __executable_header_files__ "${executable_path}/*.h")
    file(GLOB_RECURSE __executable_source_files__ "${executable_path}/*.cpp")
    add_library(${executable_name} SHARED ${__executable_header_files__} ${__executable_source_files__})
endfunction()

function(CopyDirSpecialFileRecurse from_dir dest_dir)
    if (DEFINED ARGV2)
        set(__regular__ "${ARGV2}")
    else ()
        set(__regular__ "*")
    endif ()
    file(GLOB_RECURSE __copy_files__ "${from_dir}/${__regular__}")
    file(COPY ${__copy_files__} DESTINATION ${dest_dir})
endfunction()

function(CopyDirSpecialFile from_dir dest_dir)
    if (DEFINED ARGV2)
        set(__regular__ "${ARGV2}")
    else ()
        set(__regular__ "*")
    endif ()
    file(GLOB __copy_files__ "${from_dir}/${__regular__}")
    file(COPY ${__copy_files__} DESTINATION ${dest_dir})
endfunction()

function(CopyDir from_dir dest_dir)
    file(COPY ${from_dir} DESTINATION ${dest_dir})
endfunction()

function(CopyFileToDestDirPostBuild build_target source_file dest_dir)
    add_custom_command(TARGET ${build_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${source_file} ${dest_dir})
endfunction()

function(CopySharedLibraryToDestDirPostBuild build_target source_dir lib_name dest_dir)
    if (WIN32)
        add_custom_command(TARGET ${build_target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${source_dir}/${lib_name}.dll" ${dest_dir})
    elseif ()
        add_custom_command(TARGET ${build_target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy "${source_dir}/lib${lib_name}.so" ${dest_dir})
    endif ()
endfunction()
