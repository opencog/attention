ADD_LIBRARY (attentionval SHARED
	AttentionValue.cc
	AttentionValueOfLink.cc
)

# Without this, parallel make will race and crap up the generated files.
# ADD_DEPENDENCIES(attentionval opencog_atom_types)

TARGET_LINK_LIBRARIES(attentionval
	attention-types
	value
	${ATOMSPACE_LIBRARIES}
	${COGUTIL_LIBRARY}
)

INSTALL (TARGETS attentionval
	EXPORT AttentionBankTargets
	DESTINATION "lib${LIB_DIR_SUFFIX}/opencog"
)

INSTALL (FILES
	AttentionValue.h
	AttentionValueOfLink.h
	DESTINATION "include/opencog/attentionbank/avalue"
)
