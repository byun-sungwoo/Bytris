# Bytris

My implementation of modern tetris using ncurses in C.

Game Features:
- modified SRS rotation
	- two rotation types (offset, normal)
	- offset applies to the i and o block
	- tetromino.c contains rotation algorithm
- 21 row x 10 col board size
	- game displays bottom 20 rows
- 7 bag block generation
- fixed lock delay
- holding blocks

GUI Features:
- well display
- hold display
- next block display
- stats display
- game message display
