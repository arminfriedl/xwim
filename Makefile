all: compile_commands.json
	cd build && ninja

compile_commands.json:
	cd build && ninja -t compdb > compile_commands.json

clean:
	cd build && ninja -t clean

.PHONY:
	compile_commands.json
	clean
