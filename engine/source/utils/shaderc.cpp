//
// Created by beimingxianyu on 23-7-23.
//

#include "utils/shaderc.h"

MM::Utils::ExecuteResult MM::Utils::PreprocessShader(
    std::string &preprocess_data, const char *source_name,
    shaderc_shader_kind kind, const char *source, std::uint64_t source_size) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  shaderc::PreprocessedSourceCompilationResult result =
      compiler.PreprocessGlsl(source, source_size, kind, source_name, options);

  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    return MM::Utils::ExecuteResult::UNDEFINED_ERROR;
  }

  preprocess_data = std::string(result.cbegin(), result.cend());
  return MM::Utils::ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult MM::Utils::PreprocessShader(
    std::string &preprocess_data, const std::string &source_name,
    shaderc_shader_kind kind, const char *source, std::uint64_t source_size) {
  return PreprocessShader(preprocess_data, source_name.c_str(), kind, source,
                          source_size);
}

MM::Utils::ExecuteResult MM::Utils::CompileShaderToAssembly(
    std::string &assembly_data, const char *source_name,
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
    return MM::Utils::ExecuteResult::UNDEFINED_ERROR;
  }

  assembly_data = std::string(result.cbegin(), result.cend());
  return MM::Utils::ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult MM::Utils::CompileShaderToAssembly(
    std::string &assembly_data, const std::string &source_name,
    const std::string &entry_name, shaderc_shader_kind kind, const char *source,
    std::uint64_t source_size, bool optimize,
    shaderc_optimization_level optimization_level) {
  return CompileShaderToAssembly(assembly_data, source_name.c_str(), entry_name,
                                 kind, source, source_size, optimize,
                                 optimization_level);
}

MM::Utils::ExecuteResult MM::Utils::CompileShader(
    std::vector<char> &spv_data, const char *source_name,
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
    return MM::Utils::ExecuteResult::UNDEFINED_ERROR;
  }

  std::uint64_t module_size = (module.cend() - module.cbegin()) * 4;
  spv_data.reserve(module_size);
  memcpy(spv_data.data(), module.cbegin(), module_size);

  return MM::Utils::ExecuteResult::SUCCESS;
}

MM::Utils::ExecuteResult MM::Utils::CompileShader(
    std::vector<char> &spv_data, const std::string &source_name,
    const std::string &entry_name, shaderc_shader_kind kind, const char *source,
    std::uint64_t source_size, bool optimize,
    shaderc_optimization_level optimization_level) {
  return CompileShader(spv_data, source_name.c_str(), entry_name, kind, source,
                       source_size, optimize, optimization_level);
}
