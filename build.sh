if [ ! -d "out/" ]; then 
    mkdir out
fi

gcc -o out/hello_world -Os -fno-exceptions -fno-rtti -s -ftemplate-depth=10000 src/bf.cxx 
