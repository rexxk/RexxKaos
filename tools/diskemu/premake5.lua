project "DiskEMU"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++20"

	
	files
	{
		"src/**.h",
		"src/**.cpp",
	}
	

	filter "configurations:Debug"
		defines { "DEBUG" }
	
	filter "configurations:Release"
		defines { "RELEASE" }
