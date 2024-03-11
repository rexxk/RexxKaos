workspace "RxOS"
	configurations { "Debug", "Release" }
	platforms "Win64"
	

targetdir "%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"
objdir "%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"


group "Tools"
	include "tools/diskemu"
	include "tools/debugger"

group "Kernel"
	include "kernel"

