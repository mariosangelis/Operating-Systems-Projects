all:
	gcc -g -Wall bbfs.c log.c \
		`pkg-config fuse --cflags --libs` \
		`pkg-config libssl --cflags --libs` -o bbfs
clean:
	rm -rf ./bbfs