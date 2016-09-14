cmake_minimum_required(VERSION 3.3)
project(kde_cdemu)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(KF5 REQUIRED COMPONENTS
    I18n
    CoreAddons
    Notifications
    XmlGui
    ConfigWidgets   #KStandardAction
    WidgetsAddons   #KMessageBox
    DBusAddons      #KDBusService
)

set(KDE_CDEMU_VERSION "0.6.1")

add_subdirectory(src)
add_subdirectory(po)
