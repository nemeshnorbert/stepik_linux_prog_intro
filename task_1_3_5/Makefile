# Do not forget to uncomment the next line before submitting
# build: solution

all: solution libShared

solution: solution.c
	gcc solution.c -fPIC -ldl -Wl,-rpath,. -o solution

libShared: secret.h secret.c
	gcc -o libShared.so -fPIC -shared secret.c

clean:
	rm -rf libShared.so

.PHONY: all clean
