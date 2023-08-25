//
// Created by beimingxianyu on 23-7-23.
//

#include "utils/shaderc.h"

MM::Result<std::string, MM::ErrorResult> MM::Utils::PreprocessShader(
    const char *source_name,
    shaderc_shader_kind kind, const char *source, std::uint64_t source_size) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    shaderc::PreprocessedSourceCompilationResult result =
            compiler.PreprocessGlsl(source, source_size, kind, source_name, options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        return Result<std::string, MM::ErrorResult>{st_execute_error, ErrorCode::UNDEFINED_ERROR};
    }

    return Result<std::string, MM::ErrorResult>{st_execute_success, std::string(result.cbegin(), result.cend())};

}

MM::Result<std::string, MM::ErrorResult> MM::Utils::PreprocessShader(
    const std::string &source_name,
    shaderc_shader_kind kind, const char *source, std::uint64_t source_size) {
  return PreprocessShader(source_name.c_str(), kind, source,
                          source_size);
}

MM::Result<std::string, MM::ErrorResult> MM::Utils::CompileShaderToAssembly(
    const char *source_name,
    const std::string &entry_name, shaderc_shader_kind kind, const char *source,
    std::uint64_t source_size, bool optimize,
    shaderc_optimization_level optimization_level) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  options.SetTargetEnvironment(shaderc_target_env_vulkan,
                               shaderc_env_version_vulkan_1_3);
  options.SetTargetSpirv(shaderc_spirv_version_1_6);

  if (optimize) options.SetOptimizationLevel(optimization_level);

  shaderc::AssemblyCompilationResult result = compiler.CompileGlslToSpvAssembly(
      source, source_size, kind, source_name, entry_name.c_str(), options);

  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
     return Result<std::string, MM::ErrorResult>{st_execute_error, MM::ErrorCode::UNDEFINED_ERROR};
  }

  return Result<std::string, MM::ErrorResult>{st_execute_success, std::string(result.cbegin(), result.cend())};
}

MM::Result<std::string, MM::ErrorResult> MM::Utils::CompileShaderToAssembly(
    const std::string &source_name,
    const std::string &entry_name, shaderc_shader_kind kind, const char *source,
    std::uint64_t source_size, bool optimize,
    shaderc_optimization_level optimization_level) {
  return CompileShaderToAssembly(source_name.c_str(), entry_name,
                                 kind, source, source_size, optimize,
                                 optimization_level);
}

MM::Result<std::vector<char>, MM::ErrorResult> MM::Utils::CompileShader(
    const char *source_name,
    const std::string &entry_name, shaderc_shader_kind kind, const char *source,
    std::uint64_t source_size, bool optimize,
    shaderc_optimization_level optimization_level) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  options.SetTargetEnvironment(shaderc_target_env_vulkan,
                               shaderc_env_version_vulkan_1_3);
  options.SetTargetSpirv(shaderc_spirv_version_1_6);

  if (optimize) options.SetOptimizationLevel(optimization_level);

  shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
      source, source_size, kind, source_name, entry_name.c_str(), options);

  if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
     return Result<std::vector<char>, ErrorResult>(st_execute_error, MM::ErrorCode::UNDEFINED_ERROR);
  }

  std::uint64_t module_size = (module.cend() - module.cbegin()) * 4;
  std::vector<char> spv_data(module_size);
  memcpy(spv_data.data(), module.cbegin(), module_size);

  return Result<std::vector<char>, ErrorResult>(st_execute_success, std::move(spv_data));
}

MM::Result<std::vector<char>, MM::ErrorResult>MM::Utils::CompileShader(
    const std::string &source_name,
    const std::string &entry_name, shaderc_shader_kind kind, const char *source,
    std::uint64_t source_size, bool optimize,
    shaderc_optimization_level optimization_level) {
  return CompileShader(source_name.c_str(), entry_name, kind, source,
                       source_size, optimize, optimization_level);
}
