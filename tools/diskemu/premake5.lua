project "DiskEMU"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++20"

	includedirs
	{
		"%{prj.location}/src",
	}
	
	files
	{
		"src/**.h",
		"src/**.cpp",
	}
	

	filter "configurations:Debug"
		defines { "DEBUG" }
	
	filter "configurations:Release"
		defines { "RELEASE" }
