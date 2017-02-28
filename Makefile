
all: grtd-stamp
	
INCLUDES=-I$(PWD) -I$(PWD)/ta

bootstrap-stamp: tdlib/configure.ac
	cd tdlib; ./bootstrap
	touch $@

config-stamp: bootstrap-stamp
	-mkdir build
	cd build; ../tdlib/configure CPPFLAGS="$(INCLUDES) -Dincomplete\(\)"
	touch $@

grtd-stamp: config-stamp
	$(MAKE) -C build/grtd tdlib
	$(MAKE) -C build/grtd grtd
	touch $@
