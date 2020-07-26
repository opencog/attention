LINK_LIBRARIES (
	attention-types
	attention
	${ATOMSPACE_LIBRARY}
)

ADD_CXXTEST(AttentionParamQueryUTest)
ADD_CXXTEST(ImportanceDiffusionUTest)
ADD_CXXTEST(HebbianCreationModuleUTest)
