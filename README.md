# Bytris

My take on modern tetris using ncurses in C.

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
- sonic drop, left, and right

GUI Features:
- well display
- hold display
- next block display
- stats display
- game message display

Available Controls
- move left
- move right
- rotate clockwise
- rotate counterclockwise
- rotate 180
- soft drop
- hard drop
- sonic movement
	- sonic drop  : instant drop (no lock)
	- sonic left  : goes as far left as possible
	- sonic right : goes as far right as possible
- hold current piece
