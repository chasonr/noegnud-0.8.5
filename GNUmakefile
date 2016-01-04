# This makefile is for unix and it's clones only.
# This makefile is meant for developers and people wanting
# to experiment with noegnud only. It is NOT meant for system
# installation, nor is it meant for building the game for
# final gameplay. If you want to play noegnud either check if
# your distribution of unix has a port for it, or if you are
# comfortable with system installation yourself then read and
# follow the instructions of INSTALL_unix in the doc directory

GAME = noegnud
DATE := $(shell date +%Y%m%d%H%M%S)
VERSION = snapshot-$(DATE)
FULLNAME = $(GAME)-$(VERSION)
DISTDIR = dist/$(FULLNAME)
OS = ${shell case `uname -s` in *CYGWIN*|*MINGW*|*MSYS*|*Windows*) echo "win32" ;; *) echo "unix" ;; esac}
CWD = $(shell pwd)
TESTDIR = $(CWD)/testdir
INSTALLDIR = $$HOME/$(GAME)
MD5 = ${shell which md5sum}
SHA256 = ${shell which sha256sum}

NETHACKVARIANT = "nethack"
SLASHEMVARIANT = "slashem"
ALLVARIANTS    = "full"

help:
	@echo "to build NetHack in your home directory: $(MAKE) nethack-home"
	@echo "to build Slashem in your home directory: $(MAKE) slashem-home"
	@echo "to build * Both  in your home directory: $(MAKE) home"

install:
	@echo "Nothing to do for 'install'"

home: nethack-home slashem-home

test: test-$(OS)

test-win32:
	mingw-make-nethack.bat
	mingw-make-slashem.bat

test-unix: nethack-test slashem-test

nethack-home: nethack/Makefile nethack/win/$(GAME)
	@echo "Building and installing NetHack in "$(INSTALLDIR)
	@mkdir -p $(INSTALLDIR)
	@$(MAKE) PREFIX=$(INSTALLDIR) GAMEPERM=0755 CHOWN=true CHGRP=true -C nethack install >/dev/null

slashem-home: slashem/Makefile slashem/win/$(GAME)
	@echo "Building and installing Slash'EM in "$(INSTALLDIR)
	@mkdir -p $(INSTALLDIR)
	@$(MAKE) PREFIX=$(INSTALLDIR) GAMEPERM=0755 CHOWN=true CHGRP=true -C slashem install >/dev/null

$(TESTDIR):
	@mkdir $(TESTDIR)

slashem-test: slashem/Makefile slashem/win/$(GAME) $(TESTDIR)
	@echo "Test building Slash'EM ..."
	@mkdir -p $(TESTDIR)/slashem
	@$(MAKE) PREFIX=$(TESTDIR)/slashem/ GAMEPERM=0755 CHOWN=true CHGRP=true -C slashem install >/dev/null
nethack-test: nethack/Makefile nethack/win/$(GAME) $(TESTDIR)
	@echo "Test building NetHack ..."
	@mkdir -p $(TESTDIR)/nethack
	@$(MAKE) PREFIX=$(TESTDIR)/nethack/ GAMEPERM=0755 CHOWN=true CHGRP=true -C nethack install >/dev/null

nethack/Makefile:
	@echo "Setup NetHack build environment ..."
	@cd nethack && sh sys/unix/setup.sh - >/dev/null

nethack/win/$(GAME):
	@cd nethack/win && ln -s ../../$(GAME)

slashem/Makefile:
	@echo "Setup Slash'EM build environment ..."
	@cd slashem && sh sys/unix/setup.sh - >/dev/null

slashem/win/$(GAME):
	@cd slashem/win && ln -s ../../$(GAME)

clean:
	-$(MAKE) -C nethack clean
	-$(MAKE) -C slashem clean

spotless:
	-$(MAKE) -C nethack spotless
	-$(MAKE) -C slashem spotless
	-rm -rf dist/$(GAME)*

snapshot: $(DISTDIR)/$(FULLNAME)-$(ALLVARIANTS).tar.gz
	-mv $(DISTDIR)/$(FULLNAME)-$(ALLVARIANTS).tar.gz* snapshot/.
	-rm -rf $(DISTDIR)

distfiles_targz: \
	$(DISTDIR)/$(FULLNAME)-$(ALLVARIANTS).tar.gz	\
	$(DISTDIR)/$(FULLNAME)-$(NETHACKVARIANT).tar.gz	\
	$(DISTDIR)/$(FULLNAME)-$(SLASHEMVARIANT).tar.gz

distfiles_tarbz2: \
	$(DISTDIR)/$(FULLNAME)-$(ALLVARIANTS).tar.bz2	\
	$(DISTDIR)/$(FULLNAME)-$(NETHACKVARIANT).tar.bz2	\
	$(DISTDIR)/$(FULLNAME)-$(SLASHEMVARIANT).tar.bz2

distfiles_zip: \
	$(DISTDIR)/$(FULLNAME)-$(ALLVARIANTS).zip		\
	$(DISTDIR)/$(FULLNAME)-$(NETHACKVARIANT).zip		\
	$(DISTDIR)/$(FULLNAME)-$(SLASHEMVARIANT).zip

distfiles_unixbin:	\
	$(DISTDIR)/Unix\ Installer/$(FULLNAME)-$(ALLVARIANTS)_unix-1.bin.sh \
	$(DISTDIR)/Unix\ Installer/$(FULLNAME)-$(NETHACKVARIANT)_unix-1.bin.sh \
	$(DISTDIR)/Unix\ Installer/$(FULLNAME)-$(SLASHEMVARIANT)_unix-1.bin.sh

distfiles: distfiles_targz distfiles_tarbz2 distfiles_zip distfiles_unixbin

$(DISTDIR)/$(FULLNAME)-$(ALLVARIANTS).tar.gz: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar zcvf $(FULLNAME)-$(ALLVARIANTS).tar.gz $(FULLNAME)
	cd $(DISTDIR); $(MD5) $(FULLNAME)-$(ALLVARIANTS).tar.gz > $(FULLNAME)-$(ALLVARIANTS).tar.gz.md5
	cd $(DISTDIR); $(SHA256) $(FULLNAME)-$(ALLVARIANTS).tar.gz > $(FULLNAME)-$(ALLVARIANTS).tar.gz.sha256

$(DISTDIR)/$(FULLNAME)-$(NETHACKVARIANT).tar.gz: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar zcvf $(FULLNAME)-$(NETHACKVARIANT).tar.gz $(FULLNAME)/nethack $(FULLNAME)/doc $(FULLNAME)/build
	cd $(DISTDIR); $(MD5) $(FULLNAME)-$(NETHACKVARIANT).tar.gz > $(FULLNAME)-$(NETHACKVARIANT).tar.gz.md5
	cd $(DISTDIR); $(SHA256) $(FULLNAME)-$(NETHACKVARIANT).tar.gz > $(FULLNAME)-$(NETHACKVARIANT).tar.gz.sha256

$(DISTDIR)/$(FULLNAME)-$(SLASHEMVARIANT).tar.gz: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar zcvf $(FULLNAME)-$(SLASHEMVARIANT).tar.gz $(FULLNAME)/slashem $(FULLNAME)/doc $(FULLNAME)/build
	cd $(DISTDIR); $(MD5) $(FULLNAME)-$(SLASHEMVARIANT).tar.gz > $(FULLNAME)-$(SLASHEMVARIANT).tar.gz.md5
	cd $(DISTDIR); $(SHA256) $(FULLNAME)-$(SLASHEMVARIANT).tar.gz > $(FULLNAME)-$(SLASHEMVARIANT).tar.gz.sha256

$(DISTDIR)/$(FULLNAME)-$(ALLVARIANTS).tar.bz2: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar jcvf $(FULLNAME)-$(ALLVARIANTS).tar.bz2 $(FULLNAME)
	cd $(DISTDIR); $(MD5) $(FULLNAME)-$(ALLVARIANTS).tar.bz2 > $(FULLNAME)-$(ALLVARIANTS).tar.bz2.md5
	cd $(DISTDIR); $(SHA256) $(FULLNAME)-$(ALLVARIANTS).tar.bz2 > $(FULLNAME)-$(ALLVARIANTS).tar.bz2.sha256

$(DISTDIR)/$(FULLNAME)-$(NETHACKVARIANT).tar.bz2: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar jcvf $(FULLNAME)-$(NETHACKVARIANT).tar.bz2 $(FULLNAME)/nethack $(FULLNAME)/doc
	cd $(DISTDIR); $(MD5) $(FULLNAME)-$(NETHACKVARIANT).tar.bz2 > $(FULLNAME)-$(NETHACKVARIANT).tar.bz2.md5
	cd $(DISTDIR); $(SHA256) $(FULLNAME)-$(NETHACKVARIANT).tar.bz2 > $(FULLNAME)-$(NETHACKVARIANT).tar.bz2.sha256

$(DISTDIR)/$(FULLNAME)-$(SLASHEMVARIANT).tar.bz2: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); tar jcvf $(FULLNAME)-$(SLASHEMVARIANT).tar.bz2 $(FULLNAME)/slashem $(FULLNAME)/doc
	cd $(DISTDIR); $(MD5) $(FULLNAME)-$(SLASHEMVARIANT).tar.bz2 > $(FULLNAME)-$(SLASHEMVARIANT).tar.bz2.md5
	cd $(DISTDIR); $(SHA256) $(FULLNAME)-$(SLASHEMVARIANT).tar.bz2 > $(FULLNAME)-$(SLASHEMVARIANT).tar.bz2.sha256

$(DISTDIR)/$(FULLNAME)-$(ALLVARIANTS).zip: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); zip -y -r -9 $(FULLNAME)-$(ALLVARIANTS).zip $(FULLNAME)
	cd $(DISTDIR); $(MD5) $(FULLNAME)-$(ALLVARIANTS).zip > $(FULLNAME)-$(ALLVARIANTS).zip.md5
	cd $(DISTDIR); $(SHA256) $(FULLNAME)-$(ALLVARIANTS).zip > $(FULLNAME)-$(ALLVARIANTS).zip.sha256

$(DISTDIR)/$(FULLNAME)-$(NETHACKVARIANT).zip: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); zip -r -9 $(FULLNAME)-$(NETHACKVARIANT).zip $(FULLNAME)/nethack $(FULLNAME)/doc
	cd $(DISTDIR); $(MD5) $(FULLNAME)-$(NETHACKVARIANT).zip > $(FULLNAME)-$(NETHACKVARIANT).zip.md5
	cd $(DISTDIR); $(SHA256) $(FULLNAME)-$(NETHACKVARIANT).zip > $(FULLNAME)-$(NETHACKVARIANT).zip.sha256

$(DISTDIR)/$(FULLNAME)-$(SLASHEMVARIANT).zip: $(DISTDIR)/$(FULLNAME)
	cd $(DISTDIR); zip -r -9 $(FULLNAME)-$(SLASHEMVARIANT).zip $(FULLNAME)/slashem $(FULLNAME)/doc
	cd $(DISTDIR); $(MD5) $(FULLNAME)-$(SLASHEMVARIANT).zip > $(FULLNAME)-$(SLASHEMVARIANT).zip.md5
	cd $(DISTDIR); $(SHA256) $(FULLNAME)-$(SLASHEMVARIANT).zip > $(FULLNAME)-$(SLASHEMVARIANT).zip.sha256

$(DISTDIR)/Unix\ Installer: $(DISTDIR)
	mkdir $(DISTDIR)/Unix\ Installer

$(DISTDIR)/Unix\ Installer/$(FULLNAME)-$(ALLVARIANTS)_unix-1.bin.sh: $(DISTDIR)/$(FULLNAME) $(DISTDIR)/Unix\ Installer
	cd $(DISTDIR); makeself $(FULLNAME) Unix\ Installer/$(FULLNAME)-$(ALLVARIANTS)_unix-1.bin.sh $(FULLNAME) "make home"
	cd $(DISTDIR)/Unix\ Installer; $(MD5) $(FULLNAME)-$(ALLVARIANTS)_unix-1.bin.sh > $(FULLNAME)-$(ALLVARIANTS)_unix-1.bin.sh.md5
	cd $(DISTDIR)/Unix\ Installer; $(SHA256) $(FULLNAME)-$(ALLVARIANTS)_unix-1.bin.sh > $(FULLNAME)-$(ALLVARIANTS)_unix-1.bin.sh.sha256

$(DISTDIR)/Unix\ Installer/$(FULLNAME)-$(NETHACKVARIANT)_unix-1.bin.sh: $(DISTDIR)/$(FULLNAME) $(DISTDIR)/Unix\ Installer
	cd $(DISTDIR); mv $(FULLNAME)/slashem .slashem; makeself $(FULLNAME) Unix\ Installer/$(FULLNAME)-$(NETHACKVARIANT)_unix-1.bin.sh $(FULLNAME) "make nethack-home"; mv .slashem $(FULLNAME)/slashem
	cd $(DISTDIR)/Unix\ Installer; $(MD5) $(FULLNAME)-$(NETHACKVARIANT)_unix-1.bin.sh > $(FULLNAME)-$(NETHACKVARIANT)_unix-1.bin.sh.md5
	cd $(DISTDIR)/Unix\ Installer; $(SHA256) $(FULLNAME)-$(NETHACKVARIANT)_unix-1.bin.sh > $(FULLNAME)-$(NETHACKVARIANT)_unix-1.bin.sh.sha256

$(DISTDIR)/Unix\ Installer/$(FULLNAME)-$(SLASHEMVARIANT)_unix-1.bin.sh: $(DISTDIR)/$(FULLNAME) $(DISTDIR)/Unix\ Installer
	cd $(DISTDIR); mv $(FULLNAME)/nethack .nethack; makeself $(FULLNAME) Unix\ Installer/$(FULLNAME)-$(SLASHEMVARIANT)_unix-1.bin.sh $(FULLNAME) "make slashem-home"; mv .nethack $(FULLNAME)/nethack
	cd $(DISTDIR)/Unix\ Installer; $(MD5) $(FULLNAME)-$(SLASHEMVARIANT)_unix-1.bin.sh > $(FULLNAME)-$(SLASHEMVARIANT)_unix-1.bin.sh.md5
	cd $(DISTDIR)/Unix\ Installer; $(SHA256) $(FULLNAME)-$(SLASHEMVARIANT)_unix-1.bin.sh > $(FULLNAME)-$(SLASHEMVARIANT)_unix-1.bin.sh.sha256

$(DISTDIR)/$(FULLNAME): $(DISTDIR)
	git clone ./ $@
	rm -rf $@/.git
	rm -rf $@/future
	@$(MAKE) -C doc changelog.txt
	cp doc/changelog.txt $@/doc/
	echo "#define NOEGNUD_PORT_VERSION \"$(VERSION)\"">$@/$(GAME)/noegnudversion.h
	ln -s ../../$(GAME) $@/nethack/win/$(GAME)
	ln -s ../../$(GAME) $@/slashem/win/$(GAME)

$(DISTDIR):
	mkdir -p $(DISTDIR)

