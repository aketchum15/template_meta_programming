if [ ! -d "out/" ]; then 
    mkdir out
fi

gcc -o out/hello_world -I./src/ -Os -fno-exceptions -fno-rtti -s -ftemplate-depth=10000 brainfuck/HelloWorld.cxx 
