INCLUDES = $(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/sdk/wxscintilla/include \
			-I$(top_srcdir)/src/sdk/tinyxml \
			-I$(top_srcdir)/src/sdk \
			-I$(top_srcdir)/src/sdk/as/include

lib_LTLIBRARIES = libangelscript.la

libangelscript_la_LDFLAGS = -version-info @CODEBLOCKS_VERSION_INFO@

libangelscript_la_LIBADD =  $(WX_LIBS)

libangelscript_la_SOURCES = $(srcdir)/bindings/sc_io.cpp \
$(srcdir)/bindings/sc_wxarraystring.cpp \
$(srcdir)/bindings/sc_wxstring.cpp \
$(srcdir)/bindings/scriptbindings.cpp \
$(srcdir)/source/as_anyobject.cpp \
$(srcdir)/source/as_arrayobject.cpp \
$(srcdir)/source/as_builder.cpp \
$(srcdir)/source/as_bytecode.cpp \
$(srcdir)/source/as_c.cpp \
$(srcdir)/source/as_callfunc.cpp \
$(srcdir)/source/as_callfunc_sh4.cpp \
$(srcdir)/source/as_callfunc_x86.cpp \
$(srcdir)/source/as_compiler.cpp \
$(srcdir)/source/as_configgroup.cpp \
$(srcdir)/source/as_context.cpp \
$(srcdir)/source/as_datatype.cpp \
$(srcdir)/source/as_gcobject.cpp \
$(srcdir)/source/as_generic.cpp \
$(srcdir)/source/as_module.cpp \
$(srcdir)/source/as_objecttype.cpp \
$(srcdir)/source/as_outputbuffer.cpp \
$(srcdir)/source/as_parser.cpp \
$(srcdir)/source/as_restore.cpp \
$(srcdir)/source/as_scriptcode.cpp \
$(srcdir)/source/as_scriptengine.cpp \
$(srcdir)/source/as_scriptfunction.cpp \
$(srcdir)/source/as_scriptnode.cpp \
$(srcdir)/source/as_scriptstruct.cpp \
$(srcdir)/source/as_string.cpp \
$(srcdir)/source/as_string_util.cpp \
$(srcdir)/source/as_thread.cpp \
$(srcdir)/source/as_tokenizer.cpp \
$(srcdir)/source/as_typeinfo.cpp \
$(srcdir)/source/as_variablescope.cpp

noinst_HEADERS = $(srcdir)/bindings/sc_io.h \
$(srcdir)/bindings/sc_wxarraystring.h \
$(srcdir)/bindings/sc_wxstring.h \
$(srcdir)/bindings/scriptbindings.h \
$(srcdir)/include/angelscript.h \
$(srcdir)/source/as_anyobject.h \
$(srcdir)/source/as_array.h \
$(srcdir)/source/as_arrayobject.h \
$(srcdir)/source/as_builder.h \
$(srcdir)/source/as_bytecode.h \
$(srcdir)/source/as_bytecodedef.h \
$(srcdir)/source/as_callfunc.h \
$(srcdir)/source/as_compiler.h \
$(srcdir)/source/as_config.h \
$(srcdir)/source/as_configgroup.h \
$(srcdir)/source/as_context.h \
$(srcdir)/source/as_datatype.h \
$(srcdir)/source/as_debug.h \
$(srcdir)/source/as_gcobject.h \
$(srcdir)/source/as_generic.h \
$(srcdir)/source/as_map.h \
$(srcdir)/source/as_module.h \
$(srcdir)/source/as_objecttype.h \
$(srcdir)/source/as_outputbuffer.h \
$(srcdir)/source/as_parser.h \
$(srcdir)/source/as_property.h \
$(srcdir)/source/as_restore.h \
$(srcdir)/source/as_scriptcode.h \
$(srcdir)/source/as_scriptengine.h \
$(srcdir)/source/as_scriptfunction.h \
$(srcdir)/source/as_scriptnode.h \
$(srcdir)/source/as_scriptstruct.h \
$(srcdir)/source/as_string.h \
$(srcdir)/source/as_string_util.h \
$(srcdir)/source/as_structs.h \
$(srcdir)/source/as_texts.h \
$(srcdir)/source/as_thread.h \
$(srcdir)/source/as_tokendef.h \
$(srcdir)/source/as_tokenizer.h \
$(srcdir)/source/as_typeinfo.h \
$(srcdir)/source/as_types.h \
$(srcdir)/source/as_variablescope.h
