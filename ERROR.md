<html><body><code style="white-space:pre;font-family:Menlo">clang++: error: linker command failed with exit code 1 (use -v to see invocation)
make[2]: *** [elective_course_system.app/Contents/MacOS/elective_course_system] Error 1
make[1]: *** [CMakeFiles/elective_course_system.dir/all] Error 2
make: *** [all] Error 2</code></body></html><html><body><code style="white-space:pre;font-family:Menlo">Undefined symbols for architecture arm64:
  &quot;UserDao::findbyusername(QString)&quot;, referenced from:
      UserService::login(QString, QString) in UserService.cpp.o</code></body></html>加入cmake了，名字也一致