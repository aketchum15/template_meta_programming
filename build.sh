if [ ! -d "out/" ]; then 
    mkdir out
fi

gcc -o out/hello_world -I./src/ -std=c++11 -Os -fno-exceptions -fno-rtti -s -ftemplate-depth=10000 brainfuck/HelloWorld.cxx 
