g++ main.cpp source/timer.cpp source/arrays.cpp source/cmd_args_handler.cpp \
    source/intrinsics.cpp source/native.cpp \
    -I include -g -O2 -DNDEBUG -march=native \
    -lcsfml-graphics -lcsfml-window -lcsfml-system
