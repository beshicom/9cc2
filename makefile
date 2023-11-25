


all:	SearchInclude.exe SearchInclude_regex.exe



SearchInclude_regex.exe : SearchInclude_regex.obj SearchInclude.res
	cl SearchInclude_regex.obj SearchInclude.res \
					kernel32.lib user32.lib gdi32.lib

SearchInclude_regex.obj : SearchInclude_regex.cpp SearchInclude.h
	cl /c /EHsc SearchInclude_regex.cpp

SearchInclude.exe : SearchInclude.obj SearchInclude.res
	cl SearchInclude.obj SearchInclude.res \
					kernel32.lib user32.lib gdi32.lib

SearchInclude.obj : SearchInclude.cpp SearchInclude.h
	cl /c /EHsc SearchInclude.cpp

SearchInclude.res : SearchInclude.rc SearchInclude.h
	rc SearchInclude.rc



