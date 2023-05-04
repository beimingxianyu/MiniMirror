function(PrintMessage print_data)
	message("[MiniMirror] -- ${print_data}")
endfunction()

function(AddModule module_name module_path)
	file(GLOB_RECURSE __module_header_files__ "${module_path}/*.h")
	file(GLOB_RECURSE __module_source_files__ "${module_path}/*.cpp")
	add_library(${module_name} SHARED ${__module_header_files__} ${__module_source_files__})
endfunction()

function(CopyDirSpecialFileRecurse from_dir dest_dir)
	if (DEFINED ARGV2)
		set(__regular__ "${ARGV2}")
	else()
		set(__regular__ "*")
	endif()
	file(GLOB_RECURSE __copy_files__ "${from_dir}/${__regular__}")
	file(COPY ${__copy_files__} DESTINATION ${dest_dir})
endfunction()

function(CopyDirSpecialFile from_dir dest_dir)
	if (DEFINED ARGV2)
		set(__regular__ "${ARGV2}")
	else()
		set(__regular__ "*")
	endif()
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

function(CopySharedLibraryToDestDirPostBuild build_target lib_name dest_dir)
	if (WIN32)
		add_custom_command(TARGET ${build_target} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${source_file}.dll" ${dest_dir})
	elseif()
		add_custom_command(TARGET ${build_target} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${source_file}.so" ${dest_dir})
	endif()
endfunction()
