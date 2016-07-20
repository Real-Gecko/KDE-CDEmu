cmake_minimum_required(VERSION 2.6)
project(kde_cdemu)

configure_file(kdecdemuversion.h.in ${PROJECT_BINARY_DIR}/kdecdemuversion.h)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

set(kde_cdemu_SRCS main.cpp kdecdemu.cpp mainwindow.cpp cdemu.cpp messagebox.cpp)

ki18n_wrap_ui(kde_cdemu_SRCS mainwindow.ui)
add_executable(kde_cdemu ${kde_cdemu_SRCS})

target_link_libraries(kde_cdemu KF5::KDELibs4Support KF5::KIOCore Qt5::Widgets)

install(TARGETS kde_cdemu DESTINATION ${BIN_INSTALL_DIR})
install(FILES kde_cdemuui.rc DESTINATION  ${KXMLGUI_INSTALL_DIR}/kde_cdemu)
install(FILES kde_cdemu_mount.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ServiceMenus)
install(FILES kde_cdemu.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
