TEMPLATE = lib
CONFIG += staticlib

CONFIG += building-libs

QT =

win32|mac:!macx-xcode:CONFIG += debug_and_release
macx:CONFIG(debug, debug|release) {
    TARGET = v8_debug
}

include($$PWD/v8.pri)
V8SOURCES = \
    $$V8DIR/src/accessors.cc \
    $$V8DIR/src/allocation.cc \
    $$V8DIR/src/api.cc \
    $$V8DIR/src/assembler.cc \
    $$V8DIR/src/ast.cc \
    $$V8DIR/src/atomicops_internals_x86_gcc.cc \
    $$V8DIR/src/bignum.cc \
    $$V8DIR/src/bignum-dtoa.cc \
    $$V8DIR/src/bootstrapper.cc \
    $$V8DIR/src/builtins.cc \
    $$V8DIR/src/cached-powers.cc \
    $$V8DIR/src/checks.cc \
    $$V8DIR/src/circular-queue.cc \
    $$V8DIR/src/code-stubs.cc \
    $$V8DIR/src/codegen.cc \
    $$V8DIR/src/compilation-cache.cc \
    $$V8DIR/src/compiler.cc \
    $$V8DIR/src/contexts.cc \
    $$V8DIR/src/conversions.cc \
    $$V8DIR/src/counters.cc \
    $$V8DIR/src/cpu-profiler.cc \
    $$V8DIR/src/data-flow.cc \
    $$V8DIR/src/dateparser.cc \
    $$V8DIR/src/debug-agent.cc \
    $$V8DIR/src/debug.cc \
    $$V8DIR/src/deoptimizer.cc \
    $$V8DIR/src/disassembler.cc \
    $$V8DIR/src/diy-fp.cc \
    $$V8DIR/src/dtoa.cc \
    $$V8DIR/src/execution.cc \
    $$V8DIR/src/factory.cc \
    $$V8DIR/src/flags.cc \
    $$V8DIR/src/frame-element.cc \
    $$V8DIR/src/frames.cc \
    $$V8DIR/src/full-codegen.cc \
    $$V8DIR/src/func-name-inferrer.cc \
    $$V8DIR/src/gdb-jit.cc \
    $$V8DIR/src/global-handles.cc \
    $$V8DIR/src/fast-dtoa.cc \
    $$V8DIR/src/fixed-dtoa.cc \
    $$V8DIR/src/handles.cc \
    $$V8DIR/src/hashmap.cc \
    $$V8DIR/src/heap-profiler.cc \
    $$V8DIR/src/heap.cc \
    $$V8DIR/src/hydrogen.cc \
    $$V8DIR/src/hydrogen-instructions.cc \
    $$V8DIR/src/ic.cc \
    $$V8DIR/src/inspector.cc \
    $$V8DIR/src/interpreter-irregexp.cc \
    $$V8DIR/src/isolate.cc \
    $$V8DIR/src/jsregexp.cc \
    $$V8DIR/src/jump-target.cc \
    $$V8DIR/src/lithium-allocator.cc \
    $$V8DIR/src/lithium.cc \
    $$V8DIR/src/liveedit.cc \
    $$V8DIR/src/liveobjectlist.cc \
    $$V8DIR/src/log-utils.cc \
    $$V8DIR/src/log.cc \
    $$V8DIR/src/mark-compact.cc \
    $$V8DIR/src/messages.cc \
    $$V8DIR/src/objects.cc \
    $$V8DIR/src/objects-printer.cc \
    $$V8DIR/src/objects-visiting.cc \
    $$V8DIR/src/parser.cc \
    $$V8DIR/src/preparser.cc \
    $$V8DIR/src/preparse-data.cc \
    $$V8DIR/src/profile-generator.cc \
    $$V8DIR/src/property.cc \
    $$V8DIR/src/regexp-macro-assembler-irregexp.cc \
    $$V8DIR/src/regexp-macro-assembler.cc \
    $$V8DIR/src/regexp-stack.cc \
    $$V8DIR/src/register-allocator.cc \
    $$V8DIR/src/rewriter.cc \
    $$V8DIR/src/runtime.cc \
    $$V8DIR/src/runtime-profiler.cc \
    $$V8DIR/src/safepoint-table.cc \
    $$V8DIR/src/scanner-base.cc \
    $$V8DIR/src/scanner.cc \
    $$V8DIR/src/scopeinfo.cc \
    $$V8DIR/src/scopes.cc \
    $$V8DIR/src/serialize.cc \
    $$V8DIR/src/snapshot-common.cc \
    $$V8DIR/src/spaces.cc \
    $$V8DIR/src/string-search.cc \
    $$V8DIR/src/string-stream.cc \
    $$V8DIR/src/strtod.cc \
    $$V8DIR/src/stub-cache.cc \
    $$V8DIR/src/token.cc \
    $$V8DIR/src/top.cc \
    $$V8DIR/src/type-info.cc \
    $$V8DIR/src/unicode.cc \
    $$V8DIR/src/utils.cc \
    $$V8DIR/src/v8-counters.cc \
    $$V8DIR/src/v8.cc \
    $$V8DIR/src/v8threads.cc \
    $$V8DIR/src/variables.cc \
    $$V8DIR/src/version.cc \
    $$V8DIR/src/virtual-frame.cc \
    $$V8DIR/src/zone.cc \
    $$V8DIR/src/extensions/gc-extension.cc \
    $$V8DIR/src/extensions/externalize-string-extension.cc \


arch_arm {
DEFINES += V8_TARGET_ARCH_ARM
V8SOURCES += \
    $$V8DIR/src/jump-target-light.cc \
    $$V8DIR/src/virtual-frame-light.cc \
    $$V8DIR/src/arm/builtins-arm.cc \
    $$V8DIR/src/arm/code-stubs-arm.cc \
    $$V8DIR/src/arm/codegen-arm.cc \
    $$V8DIR/src/arm/constants-arm.cc \
    $$V8DIR/src/arm/cpu-arm.cc \
    $$V8DIR/src/arm/debug-arm.cc \
    $$V8DIR/src/arm/deoptimizer-arm.cc \
    $$V8DIR/src/arm/disasm-arm.cc \
    $$V8DIR/src/arm/frames-arm.cc \
    $$V8DIR/src/arm/full-codegen-arm.cc \
    $$V8DIR/src/arm/ic-arm.cc \
    $$V8DIR/src/arm/jump-target-arm.cc \
    $$V8DIR/src/arm/lithium-arm.cc \
    $$V8DIR/src/arm/lithium-codegen-arm.cc \
    $$V8DIR/src/arm/lithium-gap-resolver-arm.cc \
    $$V8DIR/src/arm/macro-assembler-arm.cc \
    $$V8DIR/src/arm/regexp-macro-assembler-arm.cc \
    $$V8DIR/src/arm/register-allocator-arm.cc \
    $$V8DIR/src/arm/stub-cache-arm.cc \
    $$V8DIR/src/arm/virtual-frame-arm.cc \
    $$V8DIR/src/arm/assembler-arm.cc \

}

arch_mips {
DEFINES += V8_TARGET_ARCH_MIPS
V8SOURCES += \
    $$V8DIR/src/mips/assembler-mips.cc \
    $$V8DIR/src/mips/builtins-mips.cc \
    $$V8DIR/src/mips/codegen-mips.cc \
    $$V8DIR/src/mips/constants-mips.cc \
    $$V8DIR/src/mips/cpu-mips.cc \
    $$V8DIR/src/mips/debug-mips.cc \
    $$V8DIR/src/mips/disasm-mips.cc \
    $$V8DIR/src/mips/full-codegen-mips.cc \
    $$V8DIR/src/mips/frames-mips.cc \
    $$V8DIR/src/mips/ic-mips.cc \
    $$V8DIR/src/mips/jump-target-mips.cc \
    $$V8DIR/src/mips/macro-assembler-mips.cc \
    $$V8DIR/src/mips/register-allocator-mips.cc \
    $$V8DIR/src/mips/stub-cache-mips.cc \
    $$V8DIR/src/mips/virtual-frame-mips.cc \

}

arch_i386 {
DEFINES += V8_TARGET_ARCH_IA32
V8SOURCES += \
    $$V8DIR/src/jump-target-heavy.cc \
    $$V8DIR/src/virtual-frame-heavy.cc \
    $$V8DIR/src/ia32/assembler-ia32.cc \
    $$V8DIR/src/ia32/builtins-ia32.cc \
    $$V8DIR/src/ia32/code-stubs-ia32.cc \
    $$V8DIR/src/ia32/codegen-ia32.cc \
    $$V8DIR/src/ia32/cpu-ia32.cc \
    $$V8DIR/src/ia32/debug-ia32.cc \
    $$V8DIR/src/ia32/deoptimizer-ia32.cc \
    $$V8DIR/src/ia32/disasm-ia32.cc \
    $$V8DIR/src/ia32/frames-ia32.cc \
    $$V8DIR/src/ia32/full-codegen-ia32.cc \
    $$V8DIR/src/ia32/ic-ia32.cc \
    $$V8DIR/src/ia32/jump-target-ia32.cc \
    $$V8DIR/src/ia32/lithium-codegen-ia32.cc \
    $$V8DIR/src/ia32/lithium-gap-resolver-ia32.cc \
    $$V8DIR/src/ia32/lithium-ia32.cc \
    $$V8DIR/src/ia32/macro-assembler-ia32.cc \
    $$V8DIR/src/ia32/regexp-macro-assembler-ia32.cc \
    $$V8DIR/src/ia32/register-allocator-ia32.cc \
    $$V8DIR/src/ia32/stub-cache-ia32.cc \
    $$V8DIR/src/ia32/virtual-frame-ia32.cc \

}

# FIXME Should we use QT_CONFIG instead? What about 32 bit Macs?
arch_x86_64|contains(CONFIG, x86_64) {
DEFINES += V8_TARGET_ARCH_X64
V8SOURCES += \
    $$V8DIR/src/jump-target-heavy.cc \
    $$V8DIR/src/virtual-frame-heavy.cc \
    $$V8DIR/src/x64/assembler-x64.cc \
    $$V8DIR/src/x64/builtins-x64.cc \
    $$V8DIR/src/x64/code-stubs-x64.cc \
    $$V8DIR/src/x64/codegen-x64.cc \
    $$V8DIR/src/x64/cpu-x64.cc \
    $$V8DIR/src/x64/debug-x64.cc \
    $$V8DIR/src/x64/deoptimizer-x64.cc \
    $$V8DIR/src/x64/disasm-x64.cc \
    $$V8DIR/src/x64/frames-x64.cc \
    $$V8DIR/src/x64/full-codegen-x64.cc \
    $$V8DIR/src/x64/ic-x64.cc \
    $$V8DIR/src/x64/jump-target-x64.cc \
    $$V8DIR/src/x64/lithium-codegen-x64.cc \
    $$V8DIR/src/x64/lithium-gap-resolver-x64.cc \
    $$V8DIR/src/x64/lithium-x64.cc \
    $$V8DIR/src/x64/macro-assembler-x64.cc \
    $$V8DIR/src/x64/regexp-macro-assembler-x64.cc \
    $$V8DIR/src/x64/register-allocator-x64.cc \
    $$V8DIR/src/x64/stub-cache-x64.cc \
    $$V8DIR/src/x64/virtual-frame-x64.cc \

}

unix:!symbian:!macx {
V8SOURCES += \
    $$V8DIR/src/platform-linux.cc \
    $$V8DIR/src/platform-posix.cc
}

#os:macos
macx {
V8SOURCES += \
    $$V8DIR/src/platform-macos.cc \
    $$V8DIR/src/platform-posix.cc
}

win32 {
V8SOURCES += \
    $$V8DIR/src/platform-win32.cc
}

#mode:debug
CONFIG(debug) {
    V8SOURCES += \
        $$V8DIR/src/objects-debug.cc \
        $$V8DIR/src/prettyprinter.cc \
        $$V8DIR/src/regexp-macro-assembler-tracer.cc
}

symbian {
    # RVCT 2.2 doesn't understand .cc extension, and -cpp option doesn't
    # seem to do the right thing either. So we create .cpp files that
    # simply include the corresponding .cc file.
    wrapcc.commands = perl wrapcc.pl ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    wrapcc.input = V8SOURCES
    wrapcc.output = $$V8_GENERATED_SOURCES_DIR/${QMAKE_FILE_BASE}.cpp
    wrapcc.variable_out = SOURCES
    QMAKE_EXTRA_COMPILERS += wrapcc
} else {
    SOURCES += $$V8SOURCES
}

V8_LIBRARY_FILES = \
    $$V8DIR/src/runtime.js \
    $$V8DIR/src/v8natives.js \
    $$V8DIR/src/array.js \
    $$V8DIR/src/string.js \
    $$V8DIR/src/uri.js \
    $$V8DIR/src/math.js \
    $$V8DIR/src/messages.js \
    $$V8DIR/src/apinatives.js \
    $$V8DIR/src/date.js \
    $$V8DIR/src/regexp.js \
    $$V8DIR/src/json.js \
    $$V8DIR/src/liveedit-debugger.js \
    $$V8DIR/src/mirror-debugger.js \
    $$V8DIR/src/debug-debugger.js

v8_js2c.commands = python $$V8DIR/tools/js2c.py $$V8_GENERATED_SOURCES_DIR/libraries.cpp $$V8_GENERATED_SOURCES_DIR/libraries-empty.cpp CORE
v8_js2c.commands += $$V8DIR/src/macros.py ${QMAKE_FILE_IN}
v8_js2c.output = $$V8_GENERATED_SOURCES_DIR/libraries.cpp
v8_js2c.input = V8_LIBRARY_FILES
v8_js2c.variable_out = SOURCES
v8_js2c.dependency_type = TYPE_C
v8_js2c.depends = $$V8DIR/tools/js2c.py $$V8DIR/src/macros.py
v8_js2c.CONFIG += combine
v8_js2c.name = generating[v8] ${QMAKE_FILE_IN}
silent:v8_js2c.commands = @echo generating[v8] ${QMAKE_FILE_IN} && $$v8_js2c.commands
QMAKE_EXTRA_COMPILERS += v8_js2c
