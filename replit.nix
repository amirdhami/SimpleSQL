{ pkgs }: {
	deps = [
		pkgs.gdb
    pkgs.valgrind
    pkgs.cppcheck
    pkgs.ccls
	];
}