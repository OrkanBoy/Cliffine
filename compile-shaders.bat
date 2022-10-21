cd Sandbox\shaders
FOR %%i IN (*.spv) DO DEL %%i
FOR %%i IN (*) DO ..\..\Dependencies\Vulkan\glslc.exe %%i -o %%i.spv
PAUSE