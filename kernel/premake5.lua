project "kernel"
	kind "ConsoleApp"
	language "C"
--	cppdialect "c++20"

	includedirs
	{
		"%{prj.location}/src",
	}
	
	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	links
	{
--		"../bin/*.o",
--		"bin/entry",
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
	
	filter "configurations:Release"
		defines { "RELEASE" }

	filter "system:linux"
		targetdir "../bin"
		targetname "kernel.sys"
		buildoptions { "-ffreestanding" }
		linkoptions { "-nostdlib -z noexecstack -Tlink.ld ../bin/*.o" }
