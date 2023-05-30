g++ -std=c++17 src/assembler.cpp src/instructions.cpp
./a.out $1 $2
if  [ -z "$2"]
  then
    cat output
    exit 1
fi

cat $2
