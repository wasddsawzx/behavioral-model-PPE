ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I m4

if COND_TARGETS
    MAYBE_TARGETS = targets
endif

if COND_PDFIXED
    MAYBE_PDFIXED = pdfixed
endif

if COND_THRIFT
    MAYBE_THRIFT = thrift_src
    MAYBE_TESTS = tests
endif

if COND_PI
    MAYBE_PI = PI services
endif

# simple_switch depends on libbmpi so PI needs to appear first
SUBDIRS = $(MAYBE_THRIFT) third_party src include \
$(MAYBE_TESTS) $(MAYBE_PI) $(MAYBE_TARGETS) tools $(MAYBE_PDFIXED)

# I am leaving all style-related files (cpplint) out of dist on purpose, maybe
# will add them later if needed

EXTRA_DIST = \
Doxyfile \
Doxymain.md \
LICENSE \
README.md \
install_deps.sh \
autogen.sh

# 'ci' files, used by install_deps.sh to install some dependencies
EXTRA_DIST += \
ci/check-nnpy.py \
ci/common.sh \
ci/install-nanomsg.sh \
ci/install-nnpy.sh \
ci/install-thrift.sh

# mininet directory
EXTRA_DIST += \
mininet/1sw_demo.py \
mininet/p4_mininet.py \
mininet/stress_test_ipv4.py.in \
mininet/simple_router.p4 \
mininet/simple_router.json \
mininet/stress_test_commands.txt

# docs directory
EXTRA_DIST += \
docs/JSON_format.md

EXTRA_DIST += \
VERSION

dist-hook:
	@if test -f $(srcdir)/VERSION-build; then \
	  cp $(srcdir)/VERSION-build $(distdir); \
	else :; \
	fi

AM_DISTCHECK_CONFIGURE_FLAGS = --with-pdfixed --with-stress-tests
