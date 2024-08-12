leave off on page 92

catch2 build instructions
- git clone https://github.com/catchorg/Catch2.git
- cd Catch2
- git checkout "v3.x.x"
- mkdir build && cd build
- cmake .. -DBUILD_TESTING=off
- make -j $(nproc)
- sudo make install
