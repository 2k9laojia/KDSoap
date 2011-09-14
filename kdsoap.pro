TEMPLATE = subdirs
SUBDIRS  = src kdwsdl2cpp features include/KDSoap.pro
unittests: SUBDIRS += testtools unittests
SUBDIRS += examples
CONFIG   += ordered
MAJOR_VERSION = 1 ### extract from $$VERSION

unix:DEFAULT_INSTALL_PREFIX = /usr/local/KDAB/KDSoap-$$VERSION
win32:DEFAULT_INSTALL_PREFIX = "C:\KDAB\KDSoap"-$$VERSION

# for backw. compat. we still allow manual invocation of qmake using PREFIX:
isEmpty( KDSOAP_INSTALL_PREFIX ): KDSOAP_INSTALL_PREFIX=$$PREFIX

# if still empty we use the default:
isEmpty( KDSOAP_INSTALL_PREFIX ): KDSOAP_INSTALL_PREFIX=$$DEFAULT_INSTALL_PREFIX

# if the default was either set by configure or set by the line above:
equals( KDSOAP_INSTALL_PREFIX, $$DEFAULT_INSTALL_PREFIX ){
    INSTALL_PREFIX=$$DEFAULT_INSTALL_PREFIX
    unix:message( "No install prefix given, using default of" $$DEFAULT_INSTALL_PREFIX (use configure.sh -prefix DIR to specify))
    !unix:message( "No install prefix given, using default of" $$DEFAULT_INSTALL_PREFIX (use configure -prefix DIR to specify))
} else {
    INSTALL_PREFIX=\"$$KDSOAP_INSTALL_PREFIX\"
}

DEBUG_SUFFIX=""
VERSION_SUFFIX=""
CONFIG(debug, debug|release) {
  !unix: DEBUG_SUFFIX = d
}
!unix:!mac:!staticlib:VERSION_SUFFIX=$$MAJOR_VERSION

KDSOAPLIB = kdsoap$$DEBUG_SUFFIX$$VERSION_SUFFIX
KDSOAPSERVERLIB = kdsoap-server$$DEBUG_SUFFIX$$VERSION_SUFFIX


message(Install prefix is $$INSTALL_PREFIX)
message(This is KD Soap version $$VERSION)

unix:MESSAGE = '\\'$$LITERAL_HASH' KDAB qmake cache file: following lines autogenerated during qmake run'
!unix:MESSAGE = $$LITERAL_HASH' KDAB qmake cache file: following lines autogenerated during qmake run'
# Delete existing .qmake.cache if there is no .confqmake.cache to copy.
system('echo $${MESSAGE} > .qmake.cache')
# An existing cache file is replaced by the one created by the configure script
unix:system('cp ".confqmake.cache" ".qmake.cache"')
!unix:system('copy .confqmake.cache .qmake.cache')
system('echo $${MESSAGE} >> .qmake.cache')
# store PREFIX:
system('echo INSTALL_PREFIX=$$INSTALL_PREFIX >> .qmake.cache')
system('echo KDSOAPLIB=$$KDSOAPLIB >> .qmake.cache')
system('echo KDSOAPSERVERLIB=$$KDSOAPSERVERLIB >> .qmake.cache')

# forward make test calls to unittests:
test.target=test
unittests {
unix:!macx:test.commands=export LD_LIBRARY_PATH=\"$$PWD/lib\":$$(LD_LIBRARY_PATH); (cd unittests && make test)
macx:test.commands=export DYLD_LIBRARY_PATH=\"$$PWD/lib\":$$(DYLD_LIBRARY_PATH); (cd unittests && make test)
win32:test.commands=(cd unittests && $(MAKE) test)
}
test.depends = first
QMAKE_EXTRA_TARGETS += test

# install licenses: 
licenses.files = LICENSE.GPL.txt LICENSE.US.txt LICENSE.txt
licenses.path = $$INSTALL_PREFIX
INSTALLS += licenses

readme.files = README.KDSoap
readme.path = $$INSTALL_PREFIX
INSTALLS += readme

prifiles.files = kdsoap.pri kdwsdl2cpp.pri
prifiles.path = $$INSTALL_PREFIX
INSTALLS += prifiles

OTHER_FILES += configure.sh configure.bat kdsoap.pri kdwsdl2cpp.pri
