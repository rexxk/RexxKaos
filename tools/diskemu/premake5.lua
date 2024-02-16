project "DiskEMU"
	kind "ConsoleApp"
	language "C++"
	
	
	files
	{
		"**.h",
		"**.cpp",
	}
	

	filter "configurations:Debug"
		defines { "DEBUG" }
	
	filter "configurations:Release"
		defines { "RELEASE" }
