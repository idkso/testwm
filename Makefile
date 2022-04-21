SRC := $(shell fd -e c)
# INCLUDE := include/*.h

main: $(SRC)
	gcc -o main -Og -g $(SRC)
