
INCLUDES        = @ALL_INC@ 

bin_PROGRAMS    =  dump_rhog classify_rhog dump4svmlearn test_library dumpsegd

include_HEADERS = \
		windetectmain.h \
		rawdescio.h \
		formatstream.h \
		segobj.h 

lib_LIBRARIES   = 
check_PROGRAMS  = 
TESTS           = $(check_PROGRAMS)

dump_rhog_SOURCES   = dump_rhog.cpp windetectmain.cpp
dump_rhog_LDADD     = @ALL_LIB@
dump_rhog_LDFLAGS   = @ALL_LIB_DIR@
dump_rhog_DEPENDENCIES = 

classify_rhog_SOURCES   = classify_rhog.cpp windetectmain.cpp
classify_rhog_LDADD     = @ALL_LIB@
classify_rhog_LDFLAGS   = @ALL_LIB_DIR@
classify_rhog_DEPENDENCIES = 

dump4svmlearn_SOURCES   = dump4svmlearn.cpp rawdescio.cpp
dump4svmlearn_LDADD     = @REQ_LIB@
dump4svmlearn_LDFLAGS   = @REQ_LIB_DIR@
dump4svmlearn_DEPENDENCIES = 

dumpsegd_SOURCES   = dumpsegd.cpp 
dumpsegd_LDADD     = @ALL_LIB@
dumpsegd_LDFLAGS   = @ALL_LIB_DIR@
dumpsegd_DEPENDENCIES = 

test_library_SOURCES   = test_library.cpp 
test_library_LDADD     = @ALL_LIB@
test_library_LDFLAGS   = @ALL_LIB_DIR@
test_library_DEPENDENCIES = 
