@all: ./builddir/lartc

run: ./builddir/lartc
	./builddir/lartc ../lart-examples/tree.lart

test: ./builddir/lartc
	./builddir/lartc ../lart-examples/bad-type-checking.lart

./builddir/lartc: ./builddir
	ninja -C ./builddir

./builddir: meson.build
	meson setup builddir --reconfigure

clean:
	rm -rf builddir

depclean: clean
	rm -rf subprojects/tree-sitter-lart.git
