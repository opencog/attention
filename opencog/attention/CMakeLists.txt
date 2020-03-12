# To make Boost ublas much more efficient.
ADD_DEFINITIONS(-DNDEBUG)
#ADD_DEFINITIONS(-DLOG_AV_STAT)

# --------------------------------------------------

# AttentionModule
ADD_LIBRARY(attention SHARED
	AttentionModule
	AttentionParamQuery
 	AttentionUtils
	Neighbors

	ImportanceDiffusionBase
	AFImportanceDiffusionAgent
	WAImportanceDiffusionAgent

	RentCollectionBaseAgent
	AFRentCollectionAgent
	WARentCollectionAgent

	ForgettingAgent
	HebbianCreationAgent
	HebbianUpdatingAgent

	#scm/StimulationAgent
)

# TARGET_LINK_DIRECTORIES(attention
#	PUBLIC ${CMAKE_INSTALL_PREFIX}/lib${LIB_DIR_SUFFIX}/opencog/modules
#)

TARGET_LINK_LIBRARIES(attention
	attention-types
	attentionbank
	${COGSERVER_LIBRARIES}
	${ATOMSPACE_LIBRARIES})

INSTALL (TARGETS attention
	LIBRARY DESTINATION "lib${LIB_DIR_SUFFIX}/opencog"
)

ADD_GUILE_MODULE (FILES
	attention.scm
	default-param-values.scm
	MODULE_DESTINATION "${GUILE_SITE_DIR}/opencog/attention"
	DEPENDS attention_atom_types
)

INSTALL (FILES
	AttentionModule.h
	AttentionStat.h
	ForgettingAgent.h
	HebbianUpdatingAgent.h
	DESTINATION "include/opencog/${PROJECT_NAME}/"
)
