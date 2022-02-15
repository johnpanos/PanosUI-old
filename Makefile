cairo:
	g++ -I. -o cairo -x none cairo.cpp -x c xdg-shell-protocol.c -lwayland-client -lrt

clean:
	rm ./cairo