
# The atom_types.h file is written to the build directory
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

DECLARE_GUILE_CONFIG_TARGET(SCM_CONFIG "opencog attention-config" "ATTENTION_TEST")

# The build order used here is loosely in terms of
# dependencies: the later parts depend on, or may
# someday depend on the earlier parts.
#
ADD_SUBDIRECTORY (attentionbank)

IF (HAVE_COGSERVER)
	# attention builds *after* cogserver.
	ADD_SUBDIRECTORY (attention)
ENDIF(HAVE_COGSERVER)

WRITE_GUILE_CONFIG(${GUILE_BIN_DIR}/opencog/attention-config.scm SCM_CONFIG TRUE)
WRITE_GUILE_CONFIG(${GUILE_BIN_DIR}/opencog/attention-config-installable.scm SCM_CONFIG FALSE)
INSTALL(FILES ${GUILE_BIN_DIR}/opencog/attention-config-installable.scm DESTINATION ${GUILE_SITE_DIR}/opencog RENAME attention-config.scm)

