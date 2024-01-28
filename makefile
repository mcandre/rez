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
	cmake-init \
	install \
	lint \
	port \
	publish \
	test \
	test-athena \
	test-solarsystem \
	uninstall

BANNER=rez-0.0.16

all: build

audit: cmake-init
	cmake --build build --target audit

build: cmake-init
	cmake --build build --config Release
	mkdir -p bin/$(BANNER)/$(TARGET)
	cp build/bin/* bin/$(BANNER)/$(TARGET)

cmake-init:
	BANNER="$(BANNER)" cmake -B build -G "Unix Makefiles"

doc: init
	BANNER=$(BANNER) cmake --build build --target doc

lint: cmake-init
	cmake --build build --target lint

install: build
	cmake --build build --target install

port:
	snek
	sh -c "cd bin && tar czf $(BANNER).tgz $(BANNER)"

test: test-athena test-solarsystem

test-athena:
	sh -c "cd examples/athena && source .envrc && rez && rez clean && rez -c"

test-solarsystem:
	sh -c "cd examples/solarsystem && source .envrc && rez && rez clean && rez -c"

uninstall: cmake-init
	cmake --build build --target uninstall

clean: clean-bin clean-cmake clean-doc

clean-bin:
	rm -rf bin

clean-cmake:
	rm -rf build

clean-doc:
	rm -rf html
	rm -rf latex
