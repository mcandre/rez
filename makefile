.POSIX:
.SILENT:
.IGNORE: \
	clean \
	clean-bin \
	clean-cmake \
	clean-doc \
	uninstall
.PHONY: \
	all \
	audit \
	build \
	clean \
	doc \
	init \
	install \
	lint \
	port \
	publish \
	safety \
	test \
	uninstall \
	unmake

BANNER=tinyrick-0.0.14

all: build

audit: init
	cmake --build build --target audit

build: init
	cmake --build build --config Release

doc: init
	BANNER=$(BANNER) cmake --build build --target doc

lint: init unmake
	cmake --build build --target lint

init:
	BANNER="$(BANNER)" cmake -B build

install: build
	cmake --build build --target install

port:
	snek
	sh -c "cd bin && tar czf $(BANNER).tgz $(BANNER)"

safety:
	safety check

uninstall: init
	cmake --build build --target uninstall

unmake:
	unmake .
	unmake -n .

clean: clean-bin clean-cmake clean-doc

clean-bin:
	rm -rf bin

clean-cmake:
	rm -rf build

clean-doc:
	rm -rf html
	rm -rf latex
