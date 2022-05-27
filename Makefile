.PHONY: run release debug clean

run: ./build/release/mtop
	./build/release/mtop

release: ./build/release/mtop

debug: ./build/debug/mtop

./build/release/mtop:
	cmake --build build/release

./build/debug/mtop:
	cmake --build build/debug

clean:
	sh ./init-cmake.sh
