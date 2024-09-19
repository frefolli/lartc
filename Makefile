FILE=../lart-examples/include-files.lart

@all: ./builddir/lartc

./builddir/lartc: ./builddir
	ninja -C ./builddir

./builddir: meson.build
	meson setup builddir --reconfigure

clean:
	rm -rf builddir

depclean: clean
	rm -rf subprojects/tree-sitter-lart.git

test: ./builddir/lartc
	./test.sh

edit:
	nvim ${FILE}

run: ./builddir/lartc
	./builddir/lartc ${FILE}
