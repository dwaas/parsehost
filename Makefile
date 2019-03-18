.PHONY: run build

run: build
	./main

build:
	g++ --std=c++17 -INamedType main.cpp -o main
