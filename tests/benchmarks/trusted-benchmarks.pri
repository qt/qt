# Edit the list of trusted benchmarks in each of the sub-targets

check-trusted.depends = qmake
for(benchmark, TRUSTED_BENCHMARKS) {
    check-trusted.commands += (cd $$benchmark && $(MAKE) -f $(MAKEFILE) check);
}

QMAKE_EXTRA_TARGETS += check-trusted
