#include <cstdlib>
#include <sstream>
#include <iostream>
#include <filesystem>

int main(int argc, char** argv)
{
  std::string solutionDir{ argv[1] };
  std::string projectDir{ argv[2] };

  std::filesystem::path const solutionPath{ solutionDir };
  std::filesystem::path const projectPath{ projectDir };

  std::filesystem::recursive_directory_iterator const pathIt{ projectPath / "Shaders" };

  if (std::filesystem::exists(projectPath / "Compiled"))
    std::filesystem::remove_all(projectPath / "Compiled");

  std::filesystem::create_directory(projectPath / "Compiled");

  for (auto const& file : pathIt)
  {
    if (file.is_directory())
      continue;

    std::string const outputDir{ projectDir + "Compiled\\" + file.path().parent_path().filename().string() };

    if (!std::filesystem::exists(outputDir))
      std::filesystem::create_directories(outputDir);

    std::ostringstream oss{};

    oss << solutionDir << "Binary\\glslangValidator.exe -G100 "
        << file.path().string()
        << " -o " << outputDir << "\\" << file.path().filename().string();

    std::system(oss.str().c_str());
  }

  return 0;
}