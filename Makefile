# expects the name to be a directory inside a valid git repo, on which some
# tooling is done!  for example UseCase is "$HOME/InPlaceReplace.git/UseCase"
# is inside one. this one.
WORKDIR=test2

all: \
	build/refactorial \
	${WORKDIR}/build/compile_commands.json \
	show_test_diff

restore_test_dir:
	cd ${WORKDIR} && git checkout -- .

build/CMakeCache.txt: CMakeLists.txt
	mkdir -p build && \
	cd build && \
	cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build/refactorial: build/CMakeCache.txt
	${MAKE} -C build

refactorial: build/refactorial

# we care to reset the test-dir, call the binary, comile the result and display
# the diff.
execute: restore_test_dir build/refactorial ${WORKDIR}/build/compile_commands.json
	./build/refactorial \
		-refactor-specification-file=${WORKDIR}/ClassRename.yml \
		-p=${WORKDIR}/build/compile_commands.json

try_test_build: execute ${WORKDIR}/build/compile_commands.json
	${MAKE} -C ${WORKDIR}/build

show_test_diff: try_test_build
	git diff ${WORKDIR}

${WORKDIR}/build/compile_commands.json:
	mkdir -p ${WORKDIR}/build && \
	cd ${WORKDIR}/build && \
	cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

.PHONY: restore_test_dir try_test_build show_test_diff build/refactorial
