all: PrintNames.so a.out

check: all 
	./a.out
clean: 
	rm -rf *.so a.out *.o

# plugin so , must have this to provide the registry instantiation!!	
libplugin.so: plugin.C
	g++ -std=c++11 -I. -L. -fpic -O0 -g $^ -shared -o $@
PrintNames.so: PrintNames.C
	g++ -std=c++11 -I. -L. -fpic -O0 -g $^ -shared -o $@
main.o: main.C 
	g++ -std=c++11 -I. -c -O0 -g $^ -o $@ 
a.out: main.o libplugin.so
	g++ -std=c++11 -I. -L. -O0 -g $^ -o $@ -ldl -lplugin
