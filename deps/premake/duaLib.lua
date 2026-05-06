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
		path.join(duaLib.source, "src/thirdparty/hidapi/hidapi")
	}
end

function duaLib.project()
	project "duaLib"
		language "C++"

		duaLib.includes()

		files {
			path.join(duaLib.source, "src/include/**.h"),
			path.join(duaLib.source, "src/source/**.c"),
			path.join(duaLib.source, "src/include/**.hpp"),
			path.join(duaLib.source, "src/source/**.cpp"),
		}

		warnings "Off"
		kind "StaticLib"

		defines { "DUALIB_EXPORTS" }
end

table.insert(dependencies, duaLib)
