//
// Created by beimingxianyu on 23-7-23.
//
#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <shaderc/shaderc.hpp>
#include <string>
#include <vector>

#include "utils/error.h"

namespace MM {
namespace Utils {
using ShadercShaderKind = shaderc_shader_kind;

// Returns GLSL shader source text after preprocessing.
Result<std::string, ErrorResult> PreprocessShader(const char* source_name,
                                          shaderc_shader_kind kind,
                                          const char* source,
                                          std::uint64_t source_size);

// Returns GLSL shader source text after preprocessing.
Result<std::string, ErrorResult> PreprocessShader(
                                          const std::string& source_name,
                                          shaderc_shader_kind kind,
                                          const char* source,
                                          std::uint64_t source_size);

// Compiles a shader to SPIR-V assembly. Returns the assembly text
// as a string.
Result<std::string, ErrorResult> CompileShaderToAssembly(
    const char* source_name,
    const std::string& entry_name, shaderc_shader_kind kind, const char* source,
    std::uint64_t source_size, bool optimize = false,
    shaderc_optimization_level optimization_level =
        shaderc_optimization_level_performance);

// Compiles a shader to SPIR-V assembly. Returns the assembly text
// as a string.
Result<std::string, ErrorResult> CompileShaderToAssembly(
    const std::string& source_name,
    const std::string& entry_name, shaderc_shader_kind kind, const char* source,
    std::uint64_t source_size, bool optimize = false,
    shaderc_optimization_level optimization_level =
        shaderc_optimization_level_performance);

// Compiles a shader to a SPIR-V binary. Returns the binary as
// a vector of 8-bit words.
Result<std::vector<char>, ErrorResult> CompileShader(
    const char* source_name,
    const std::string& entry_name, shaderc_shader_kind kind, const char* source,
    std::uint64_t source_size, bool optimize = false,
    shaderc_optimization_level optimization_level =
        shaderc_optimization_level_performance);

// Compiles a shader to a SPIR-V binary. Returns the binary as
// a vector of 8-bit words.
Result<std::vector<char>, ErrorResult> CompileShader(
    const std::string& source_name,
    const std::string& entry_name, shaderc_shader_kind kind, const char* source,
    std::uint64_t source_size, bool optimize = false,
    shaderc_optimization_level optimization_level =
        shaderc_optimization_level_performance);
}  // namespace Utils
}  // namespace MM
