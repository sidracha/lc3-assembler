g++ -std=c++17 src/assembler.cpp src/instructions.cpp
./a.out $1 $2
cat output
