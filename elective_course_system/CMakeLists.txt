cmake_minimum_required(VERSION 3.19)
project(elective_course_system LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(elective_course_system
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    adminwindow.h adminwindow.cpp adminwindow.ui
    db/dbmanager.cpp
    db/dbmanager.h
    model/user.h
    model/user.cpp
    model/course.h
    model/selection.h
    utils/validators.h
    utils/validators.cpp
    utils/passwordutils.h
    utils/passwordutils.cpp
    Service/UserService.h
    Service/UserService.cpp
    Service/CourseService.h
    Service/CourseService.cpp
    Service/AdminService.h
    Service/AdminService.cpp
    Dao/UserDao.cpp
    Dao/UserDao.h
    Dao/CourseDao.cpp
    Dao/CourseDao.h
    Dao/SelectionDao.cpp
    Dao/SelectionDao.h
    Dao/ApplicationDao.cpp
    Dao/ApplicationDao.h
    Dao/StudentDao.cpp
    Dao/StudentDao.h
)

target_include_directories(elective_course_system PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(elective_course_system
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
)

install(TARGETS elective_course_system
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET elective_course_system
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
