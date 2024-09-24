FILE=../lart-examples/hello.lart

@all: ./builddir/lartc

./builddir/lartc: ./builddir
	ninja -C ./builddir

./builddir: meson.build
	meson setup builddir --reconfigure

clean:
	rm -rf builddir

test: ./builddir/lartc
	./test.sh

edit:
	nvim ${FILE}

run: ./builddir/lartc
	./builddir/lartc ${FILE}

inspect:
	nvim tmp/bucket.ll
