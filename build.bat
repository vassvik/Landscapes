cl /nologo /Iinclude /std:c++latest /EHsc /MP /Zi ^
	src\main.cpp ^
	src\utils.cpp ^
	src\imgui_impl_glfw_gl3.cpp ^
	include\glad\glad.c ^
	/DEBUG lib\glfw3.lib gdi32.lib user32.lib libcmt.lib shell32.lib lib\imgui.lib