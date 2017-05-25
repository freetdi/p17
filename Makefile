
all: grtd-stamp
	
INCLUDES=-I$(PWD) -I$(PWD)/ta

bootstrap-stamp: tdlib/configure.ac
	cd tdlib; ./bootstrap
	touch $@

config-stamp: bootstrap-stamp
	-mkdir build
	cd build; ../tdlib/configure --without-python CPPFLAGS="$(INCLUDES) -Dunreachable\(\) -Dincomplete\(\)" CXXFLAGS=-O3
	touch $@

grtd-stamp: config-stamp
	$(MAKE) -C build/grtd tdlib
	-(cd build/grtd; ln -s ../../gala)
	$(MAKE) -C build/grtd grtd
	touch $@
