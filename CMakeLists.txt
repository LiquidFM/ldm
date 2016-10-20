project (ldm)

# Project header
project_static_library_header_default ()

# 3rdparty
find_package (Qt4 4.7.1 COMPONENTS QtCore QtGui QtDbus REQUIRED)
include (${QT_USE_FILE})
add_definitions (${QT_DEFINITIONS} -DQT_NO_CAST_FROM_ASCII)

find_package (EFC REQUIRED)
include_directories (${EFC_INCLUDE})

find_package (Lvfs REQUIRED)
include_directories (${LVFS_INCLUDE})

# Sources
add_subdirectory (src)
QT4_WRAP_CPP (${PROJECT_NAME}_MOC_SOURCES ${${PROJECT_NAME}_HEADERS} OPTIONS "-nw")
list (APPEND ${PROJECT_NAME}_SOURCES ${${PROJECT_NAME}_MOC_SOURCES})

# Target - ldm
add_library (ldm SHARED ${${PROJECT_NAME}_SOURCES})
target_link_libraries (ldm ${QT_LIBRARIES} ${LVFS_LIB} ${EFC_LIB})

# Documentation
add_documentation (ldm 0.0.1 "Qt based devices manager library")

# Install rules
install_header_files (ldm "src/ldm_Device.h:Device"
                          "src/ldm_Devices.h:Devices"
                          "src/ldm_Drive.h:Drive"
                          "src/ldm_FlashDrive.h:FlashDrive"
                          "src/ldm_FloppyDrive.h:FloppyDrive"
                          "src/ldm_HardDrive.h:HardDrive"
                          "src/ldm_OpticalDrive.h:OpticalDrive"
                          "src/ldm_Partition.h:Partition"
                          "src/ldm_RemovableDrive.h:RemovableDrive"
                          "src/ldm_Visitor.h:Visitor")
install_cmake_files ("cmake/FindLDM.cmake")
install_target (ldm)
