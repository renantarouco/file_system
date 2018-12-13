# Baixando
    
    git clone https://github.com/renantarouco/file_system
    cd file_system
    mkdir lib
    cd lib
    git clone https://github.com/nlohmann/json
    mv json nlohmann_json

# Compilando
    
    mkdir build
    cd build
    cmake ..
    make

    ./terminal ../config-fat.json
    ./terminal ../config-inode.json