AM_CPPFLAGS = -I$(top_srcdir)/include -include $(top_srcdir)/config.h
AM_CXXFLAGS = -std=c++11 -rdynamic

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libwpl-1.1.pc

lib_LTLIBRARIES = libwpl-1.1.la
libwpl_1_1_la_LDFLAGS = -rpath $(libdir) -lmimetic $(BOOST_REGEX_LDFLAGS) $(BOOST_REGEX_LIBS)
libwpl_1_1_la_SOURCES = \
	program.cpp parser.cpp block.cpp pragma.cpp \
	module_loader.cpp io.cpp state.cpp \
	expression.cpp matcher.cpp identifier.cpp \
	variable.cpp namespace.cpp runable.cpp \
	shunting_yard.cpp block_if.cpp block_while.cpp \
	block_for.cpp \
	scene.cpp string.cpp object.cpp types.cpp \
	function.cpp user_function.cpp internal_function.cpp \
	value.cpp operator.cpp text.cpp modifier.cpp \
	value_array.cpp value_struct.cpp value_hash.cpp \
	value_unresolved.cpp value_function_ptr.cpp value_list.cpp \
	value_string.cpp value_vstring.cpp value_regex.cpp \
	value_env.cpp value_get.cpp value_post.cpp value_stdin.cpp \
	value_output_trigger.cpp value_time.cpp \
	type_file.cpp file.cpp value_line.cpp \
	array.cpp hash.cpp struct.cpp template.cpp \
	namespace_session.cpp exception.cpp \
	block_state.cpp expression_state.cpp function_state.cpp \
	pragma_state.cpp text_state.cpp scene_state.cpp \
	output_json.cpp
