to build (in root of project)

- cmake --preset default .
- cmake --build build

catch2 build instructions
- git clone https://github.com/catchorg/Catch2.git
- cd Catch2
- git checkout "v3.x.x"
- mkdir build && cd build
- cmake .. -DBUILD_TESTING=off
- make -j $(nproc)
- sudo make install

fmt:
debian - apt install libfmt-dev

debian - apt install libreadline-dev

zydis:
debian - apt-get install libzydis-dev zydis-tools
