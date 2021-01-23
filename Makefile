$(shell mkdir -p bin)

build:
	gcc *.c -ldl -o bin/so-injector

clean:
	rm -rf bin
