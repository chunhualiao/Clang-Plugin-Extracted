all: PrintNames.so a.out PreOrderPlugin.so

check: all 
	./a.out
clean: 
	rm -rf *.so a.out *.o

# plugin so , must have this to provide the registry instantiation!!	
libplugin.so: plugin.C
	g++ -I. -L. -fpic -O0 -g $^ -shared -o $@
PrintNames.so: PrintNames.C
	g++ -I. -L. -fpic -O0 -g $^ -shared -o $@
PreOrderPlugin.so: PreOrderPlugin.C	
	g++ -I. -L. -fpic -O0 -g $^ -shared -o $@
main.o: main.C 
	g++ -I. -c -O0 -g $^ -o $@ 
a.out: main.o libplugin.so
	g++ -I. -L. -O0 -g $^ -o $@ -ldl -lplugin
