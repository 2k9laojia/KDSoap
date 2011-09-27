SET(CPACK_PACKAGE_NAME "KDSoap")
SET(CPACK_PACKAGE_NAME_SIMPLIFIED "kdsoap")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0-r0")
SET(CPACK_INSTALL_DIRECTORY "/Users/mike/Documents/KDAB/Products/SVN/kdsoap/autogen")
SET(CPACK_PACKAGE_SOURCE @CPACK_PACKAGE_SOURCE@)

SET(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
GET_FILENAME_COMPONENT(CPACK_INSTALLED_DIRECTORIES "${CPACK_INSTALL_DIRECTORY}" REALPATH)
LIST(APPEND CPACK_INSTALLED_DIRECTORIES ".")

SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME_SIMPLIFIED}-${CPACK_PACKAGE_VERSION}-source")
SET(CPACK_PACKAGE_NAME_AND_VERSION "${CPACK_PACKAGE_NAME} ${CPACK_PACKAGE_VERSION} Source Code")

IF(WIN32)
	SET(CPACK_GENERATOR "NSIS")
	SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME_AND_VERSION}")
	SET(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME_AND_VERSION}")
	SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${CPACK_PACKAGE_NAME_AND_VERSION}")
ELSEIF(APPLE)
	SET(CPACK_GENERATOR "ZIP")
	SET(CPACK_SYSTEM_NAME "OSX")
ELSE()
	SET(CPACK_GENERATOR "TBZ2")
ENDIF()

SET(CPACK_TOPLEVEL_TAG "${CPACK_SYSTEM_NAME}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_FILE_NAME}")
SET(CPACK_RESOURCE_FILE_LICENSE "/Users/mike/Documents/KDAB/Products/SVN/kdsoap/autogen/LICENSE.txt")
SET(CPACK_IGNORE_FILES "/\\\\.svn/;/\\\\.git/;\\\\.cache$;/[\\\\._]moc/;/[\\\\._]obj/;[\\\\._]ui/;[\\\\._]qrc/;\\\\.pdb$;\\\\.zip$;\\\\.exe$;\\\\.tar;Makefile;CPack;autogen.py$;\\\\.license\\\\.accepted$;/bin/;/lib/;/debug/;/release/;-source/")
SET(CPACK_PACKAGE_DESCRIPTION "")