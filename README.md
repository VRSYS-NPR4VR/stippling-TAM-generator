# Tonal Art Map Generator
The Generator takes the approach of the hatching generator presented in the paper Praun, E., Hoppe, H., Webb, M., Finkelstein, A. (2001) and applies it to stippling.

### Usage
The latest version is on the main branch.
#### Standalone application:
For a standalone application only the Generator folder is needed. Execute the WalnutApp.exe in this folder to run the application.

#### Building process with Visual Studio:
##### Prerequisites:
- Vulkan
- OpenCV
- OpenMP
##### Setup:
- Clone the main branch:
  git clone -b main --recursive https://github.com/VRSYS-NPR4VR/stippling-TAM-generator.git
- Run the Setup.bat in the scripts folder
- Open the Visual Studio solution
- Setup OpenCV and OpenMP with Visual Studio
- Build and Run
- WalnutApp.cpp is the main entry point and the TAM generation is done in generator.cpp and generator.h

### 3rd party libaries
- [Walnut](https://github.com/TheCherno/Walnut)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [GLFW](https://github.com/glfw/glfw)
- [stb_image](https://github.com/nothings/stb)
- [GLM](https://github.com/g-truc/glm)
- [OpenCV](https://github.com/opencv/opencv) ([Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0))

### Additional
- The Gui uses the [Roboto](https://fonts.google.com/specimen/Roboto) font ([Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0))
