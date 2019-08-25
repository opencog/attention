
# The atom_types.h file is written to the build directory
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

DECLARE_GUILE_CONFIG_TARGET(SCM_CONFIG "opencog oc-config" "OPENCOG_TEST")

# The build order used here is loosely in terms of
# dependencies: the later parts depend on, or may
# someday depend on the earlier parts.
#
ADD_SUBDIRECTORY (attentionbank)

IF (HAVE_SERVER)
	# attention builds *after* cogserver.
	ADD_SUBDIRECTORY (attention)
ENDIF(HAVE_SERVER)
