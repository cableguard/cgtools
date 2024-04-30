# SPDX-License-Identifier: GPL-2.0
#
# Copyright (C) 2015-2020 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.

PKG_CONFIG ?= pkg-config
PREFIX ?= /usr
DESTDIR ?=
SYSCONFDIR ?= /etc
BINDIR ?= $(PREFIX)/bin
LIBDIR ?= $(PREFIX)/lib
MANDIR ?= $(PREFIX)/share/man
BASHCOMPDIR ?= $(PREFIX)/share/bash-completion/completions
SYSTEMDUNITDIR ?= $(shell $(PKG_CONFIG) --variable=systemdsystemunitdir systemd 2>/dev/null || echo "$(PREFIX)/lib/systemd/system")
RUNSTATEDIR ?= /var/run
WITH_BASHCOMPLETION ?=
WITH_WGQUICK ?=
WITH_SYSTEMDUNITS ?=


ifeq ($(WITH_BASHCOMPLETION),)
ifneq ($(strip $(wildcard $(BASHCOMPDIR))),)
WITH_BASHCOMPLETION := yes
endif
endif
ifeq ($(WITH_WGQUICK),)
ifneq ($(strip $(wildcard $(BINDIR)/bash)),)
WITH_WGQUICK := yes
endif
ifneq ($(strip $(wildcard $(DESTDIR)/bin/bash)),)
WITH_WGQUICK := yes
endif
endif
ifeq ($(WITH_SYSTEMDUNITS),)
ifneq ($(strip $(wildcard $(SYSTEMDUNITDIR))),)
WITH_SYSTEMDUNITS := yes
endif
endif

PLATFORM ?= $(shell uname -s | tr '[:upper:]' '[:lower:]')

CFLAGS ?= -O3
ifneq ($(wildcard uapi/$(PLATFORM)/.),)
CFLAGS += -idirafter uapi/$(PLATFORM)
endif
CFLAGS += -std=gnu99 -D_GNU_SOURCE
CFLAGS += -Wall -Wextra
CFLAGS += -MMD -MP
CFLAGS += -DRUNSTATEDIR="\"$(RUNSTATEDIR)\""
ifeq ($(DEBUG),yes)
CFLAGS += -g
endif
WIREGUARD_TOOLS_VERSION = $(patsubst v%,%,$(shell GIT_DIR="$(PWD)/../.git" git describe --dirty 2>/dev/null))
ifneq ($(WIREGUARD_TOOLS_VERSION),)
CFLAGS += -D'WIREGUARD_TOOLS_VERSION="$(WIREGUARD_TOOLS_VERSION)"'
endif
ifeq ($(PLATFORM),freebsd)
LDLIBS += -lnv
endif
ifeq ($(PLATFORM),haiku)
LDLIBS += -lnetwork -lbsd
endif
ifeq ($(PLATFORM),windows)
CC := x86_64-w64-mingw32-clang
WINDRES := $(shell $(CC) $(CFLAGS) -print-prog-name=windres 2>/dev/null)
CFLAGS += -Iwincompat/include -include wincompat/compat.h -DWINVER=0x0601 -D_WIN32_WINNT=0x0601 -flto
LDLIBS += -lws2_32 -lsetupapi -lole32 -ladvapi32 -lntdll -Lwincompat
LDFLAGS += -flto -Wl,--dynamicbase -Wl,--nxcompat -Wl,--tsaware -mconsole
LDFLAGS += -Wl,--major-os-version=6 -Wl,--minor-os-version=1 -Wl,--major-subsystem-version=6 -Wl,--minor-subsystem-version=1
# The use of -Wl,/delayload: here implies we're using llvm-mingw
LDFLAGS += -Wl,/delayload:ws2_32.dll -Wl,/delayload:setupapi.dll -Wl,/delayload:ole32.dll -Wl,/delayload:advapi32.dll
VERSION := $(patsubst "%",%,$(filter "%",$(file < version.h)))
wg: wincompat/libc.o wincompat/init.o wincompat/loader.o wincompat/resources.o
wincompat/resources.o: wincompat/resources.rc wincompat/manifest.xml
	$(WINDRES) -DVERSION_STR=$(VERSION) -O coff -c 65001 -i $< -o $@
endif

ifneq ($(V),1)
BUILT_IN_LINK.o := $(LINK.o)
LINK.o = @echo "  LD      $@";
LINK.o += $(BUILT_IN_LINK.o)
BUILT_IN_COMPILE.c := $(COMPILE.c)
COMPILE.c = @echo "  CC      $@";
COMPILE.c += $(BUILT_IN_COMPILE.c)
BUILT_IN_RM := $(RM)
RM := @a() { echo "  CLEAN   $$@"; $(BUILT_IN_RM) "$$@"; }; a
WINDRES := @a() { echo "  WINDRES $${@: -1}"; $(WINDRES) "$$@"; }; a
endif

wg: $(sort $(patsubst %.c,%.o,$(wildcard *.c))) $(LDLIBS)

clean:
	$(RM) wg *.o *.d $(wildcard wincompat/*.o wincompat/*.lib wincompat/*.dll)

install: wg
	@install -v -d "$(DESTDIR)$(BINDIR)" && install -v -m 0755 wg "$(DESTDIR)$(BINDIR)/wg"
	@install -v -d "$(DESTDIR)$(MANDIR)/man8" && install -v -m 0644 man/wg.8 "$(DESTDIR)$(MANDIR)/man8/wg.8"
	@[ "$(WITH_BASHCOMPLETION)" = "yes" ] || exit 0; \
	install -v -d "$(DESTDIR)$(BASHCOMPDIR)" && install -v -m 0644 completion/wg.bash-completion "$(DESTDIR)$(BASHCOMPDIR)/wg"
	@[ "$(WITH_WGQUICK)" = "yes" ] || exit 0; \
	install -v -m 0755 wg-quick/$(PLATFORM).bash "$(DESTDIR)$(BINDIR)/wg-quick" && install -v -m 0700 -d "$(DESTDIR)$(SYSCONFDIR)/wireguard"
	@[ "$(WITH_WGQUICK)" = "yes" ] || exit 0; \
	install -v -m 0644 man/wg-quick.8 "$(DESTDIR)$(MANDIR)/man8/wg-quick.8"
	@[ "$(WITH_WGQUICK)" = "yes" -a "$(WITH_BASHCOMPLETION)" = "yes" ] || exit 0; \
	install -v -m 0644 completion/wg-quick.bash-completion "$(DESTDIR)$(BASHCOMPDIR)/wg-quick"
	@[ "$(WITH_WGQUICK)" = "yes" -a "$(WITH_SYSTEMDUNITS)" = "yes" ] || exit 0; \
	install -v -d "$(DESTDIR)$(SYSTEMDUNITDIR)" && install -v -m 0644 systemd/* "$(DESTDIR)$(SYSTEMDUNITDIR)/"

check: clean
	scan-build --html-title=wireguard-tools -maxloop 100 --view --keep-going $(MAKE) wg

all: wg
.DEFAULT_GOAL: all
.PHONY: clean install check

-include *.d
