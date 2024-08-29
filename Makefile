@all: ./builddir/lartc

run : ./builddir/lartc
	./builddir/lartc

./builddir/lartc: ./builddir
	ninja -C ./builddir

./builddir: meson.build
	meson setup builddir --reconfigure

clean:
	rm -r builddir

depclean: clean
	rm -r subprojects/tree-sitter-lart.git
