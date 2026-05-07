DSXpp = {
	source = path.join(dependencies.basePath, "DSXpp"),
}

function DSXpp.import()
	links { "DSXpp" }
	DSXpp.includes()
end

function DSXpp.includes()
	includedirs {
		path.join(DSXpp.source, "include"),
	}
end

function DSXpp.project()
	project "DSXpp"
		language "C++"

		DSXpp.includes()

		files {
			path.join(DSXpp.source, "src/*.h"),
			path.join(DSXpp.source, "src/*.cpp"),
            path.join(DSXpp.source, "src/nlohmann/*.hpp"),
		}
		
		warnings "Off"
		kind "StaticLib"
end

table.insert(dependencies, DSXpp)
