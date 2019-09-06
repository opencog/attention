
include(CMakePackageConfigHelpers)

#export(EXPORT AttentionBankTargets
#	FILE "${CMAKE_CURRENT_BINARY_DIR}/AttentionBank/AttentionBankTargets.cmake"
#)

set(ConfigPackageLocation lib/cmake/AttentionBank)

install(EXPORT AttentionBankTargets
	FILE AttentionBankTargets.cmake
	DESTINATION ${ConfigPackageLocation}
)

configure_package_config_file(AttentionBankConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/AttentionBankConfig.cmake
	INSTALL_DESTINATION ${ConfigPackageLocation}
	PATH_VARS CMAKE_INSTALL_PREFIX
)

SET(SEMANTIC_VERSION 1.0.0)

write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/AttentionBankConfigVersion.cmake"
	VERSION ${SEMANTIC_VERSION}
	COMPATIBILITY SameMajorVersion
)

INSTALL(FILES
	${CMAKE_CURRENT_BINARY_DIR}/AttentionBankConfigVersion.cmake
	${CMAKE_CURRENT_BINARY_DIR}/AttentionBankConfig.cmake
	DESTINATION ${ConfigPackageLocation}
)
