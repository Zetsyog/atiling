AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = src pluto

ACLOCAL_AMFLAGS = -I m4

bin_SCRIPTS = atiling

CLEANFILES: $(bin_SCRIPTS)

EXTRA_DIST = atiling.sh.in

################################################################################
# Trahrhe
trahrhe:
	$(MAKE) -C trahrhe

test: 
	$(MAKE) -C test

atiling: atiling.sh
	rm -f atiling
	echo "#!"$(BASH) > atiling
	cat atiling.sh >> atiling
	chmod ugo+x atiling

.PHONY: trahrhe test
