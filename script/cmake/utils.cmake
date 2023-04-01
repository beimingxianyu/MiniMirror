function(PrintMessage print_data)
	message("[MiniMirror] -- ${print_data}")
endfunction()

function(AddMoudle moudle_name moudle_path)
	file(GLOB_RECURSE __moudle_header_files__ "${moudle_path}/*.h")
	file(GLOB_RECURSE __moudle_source_files__ "${moudle_path}/*.cpp")
	add_library(${moudle_name} SHARED ${__moudle_header_files__} ${__moudle_source_files__})
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

