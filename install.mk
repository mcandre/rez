.PHONY: all

all:
	@cargo install --force unmake@0.0.6

	@pip3 install --upgrade pip setuptools
	@pip3 install -r requirements-dev.txt
