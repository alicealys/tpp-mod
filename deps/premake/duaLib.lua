
hidapi = {
    source = path.join(dependencies.basePath, "duaLib/src/thirdparty/hidapi")
}

function hidapi.import()
    links { "hidapi" }
    hidapi.includes()
end

function hidapi.includes()
    includedirs {
        hidapi.source,
        path.join(hidapi.source, "hidapi"),
        path.join(hidapi.source, "windows")
    }
end

function hidapi.project()
    project "hidapi"
        language "C"
        kind "StaticLib"
        
        hidapi.includes()
        
        files {
            path.join(hidapi.source, "hidapi/hidapi.h"),
            path.join(hidapi.source, "windows/hid.c"),
            path.join(hidapi.source, "windows/hidapi_winapi.h"),
            path.join(hidapi.source, "windows/hidapi_descriptor_reconstruct.c"),
        }

        excludes { 
            "**/version",
            "**/VERSION" 
        }
        
        warnings "Off"
        
        filter "system:windows"
            defines { "HIDAPI_NO_HIDIO" }
            links { "setupapi", "hid" }
        filter {}
end

table.insert(dependencies, hidapi)

duaLib = {
    source = path.join(dependencies.basePath, "duaLib"),
}

function duaLib.import()
    links { "duaLib" }
    duaLib.includes()
end

function duaLib.includes()
    includedirs {
        path.join(duaLib.source, "src/include"),
    }
end

function duaLib.project()
    project "duaLib"
        language "C++"
        kind "StaticLib"
        
        duaLib.includes()
        hidapi.import()
        
        files {
            path.join(duaLib.source, "src/include/**.h"),
            path.join(duaLib.source, "src/include/**.hpp"),
            path.join(duaLib.source, "src/source/**.c"),
            path.join(duaLib.source, "src/source/**.cpp"),
        }
        
        warnings "Off"
        defines { "DUALIB_EXPORTS" }
end

table.insert(dependencies, duaLib)