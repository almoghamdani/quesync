# Create the build folder
if [[ -d $build ]]; then
    mkdir $build
fi

# Build the makefile
cd build
cmake -DUSE32bt=false ..

make

cd ..