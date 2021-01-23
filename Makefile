$(shell mkdir -p bin)

build:
	gcc *.c -static -o bin/so-injector

clean:
	rm -rf bin
