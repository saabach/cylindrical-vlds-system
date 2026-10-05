CC ?= cc
PYTHON ?= python3
CPPFLAGS ?=
CFLAGS ?= -O2 -std=c11 -Wall -Wextra -Wpedantic
# Do not use -ffast-math: historical operation order/rounding are part of the oracle.
FPFLAGS = -fno-fast-math -ffp-contract=off
LDLIBS = -lm
SOURCES = src/main.c src/lattice.c src/crsos.c src/observables.c src/sampling.c src/output.c src/rng.c
OBJECTS = $(SOURCES:src/%.c=build/%.o)

.PHONY: all clean test test-sanitize audit-public
all: cylindrical-crsos

cylindrical-crsos: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) $(FPFLAGS) -MMD -MP -c $< -o $@

-include $(OBJECTS:.o=.d)

test: cylindrical-crsos
	CC="$(CC)" $(PYTHON) tests/test_equivalence.py

test-sanitize:
	CC="$(CC)" $(PYTHON) tests/test_equivalence.py --sanitize

audit-public:
	$(PYTHON) tests/audit_public.py

clean:
	$(RM) $(OBJECTS) $(OBJECTS:.o=.d) cylindrical-crsos
