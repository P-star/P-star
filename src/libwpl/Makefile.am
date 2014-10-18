AM_CPPFLAGS = -I$(top_srcdir)/include -include $(top_srcdir)/config.h
AM_CXXFLAGS = -std=c++11 -rdynamic

lib_LTLIBRARIES = libpstar.la
libpstar_la_LDFLAGS =  -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -lmimetic -lboost_regex
libpstar_la_SOURCES = \
	program.cpp parser.cpp pragma.cpp \
	module_loader.cpp io.cpp state.cpp \
	expression.cpp matcher.cpp identifier.cpp shunting_yard.cpp \
	variable.cpp namespace.cpp runable.cpp runable_operator.cpp \
	block.cpp block_parser.cpp \
	block_conditional.cpp block_if.cpp block_while.cpp block_for.cpp block_foreach.cpp \
	scene.cpp string.cpp object.cpp \
	types.cpp type.cpp type_template.cpp type_parse_signals.cpp \
	function.cpp user_function.cpp internal_function.cpp \
	value.cpp operator.cpp operator_special.cpp \
	text.cpp text_chunk_type.cpp \
	modifier.cpp global.cpp parse_and_run.cpp \
	value_array.cpp value_pointer.cpp value_struct.cpp value_hash.cpp \
	value_unresolved.cpp value_function_ptr.cpp value_list.cpp \
	value_string.cpp value_vstring.cpp value_regex.cpp \
	value_env.cpp value_get.cpp value_post.cpp value_stdin.cpp \
	value_output_trigger.cpp value_time.cpp \
	range.cpp \
	type_file.cpp file.cpp value_line.cpp \
	array.cpp pointer.cpp hash.cpp struct.cpp template.cpp \
	namespace_session.cpp exception.cpp \
	block_state.cpp block_intermediate_state.cpp expression_state.cpp function_state.cpp \
	pragma_state.cpp text_state.cpp scene_state.cpp \
	output_json.cpp


