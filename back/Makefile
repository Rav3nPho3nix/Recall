extension =
build_dir = build
# all: server

# server.o: server.c libs/jansson.h
# 	gcc $< -c

# server$(extension): server.o
# 	gcc $^ -o $@ -lmicrohttpd -ljansson

# clean:
# 	rm -f *.o

# delete: clean
# 	rm -f server

all: server$(extension) clean

json.o: src/json.c headers/json.h
	gcc $< -c -o $(build_dir)/$@

server.o: src/server.c libs/jansson.h
	gcc $< -c -o $(build_dir)/$@

server$(extension): server.o json.o
	gcc $(build_dir)/server.o $(build_dir)/json.o -o $(build_dir)/$@ -lmicrohttpd -ljansson

clean:
	rm -f $(build_dir)/*.o

delete:
	rm -f $(build_dir)/server