/**
 * Bytris Project - 11/16/2019
 * Giga-Jank Tetris
 * 
 * Bytris
 */
public class Bytris {
	public Tetromino[] tetromino_list;
	public int[][] board;
	private Point focus;
	
	public Bytris() {
		board = new int[20][10];
		tetromino_list = new Tetromino[7];
		int[][] i_block = {{1, 3, 1, 1}};
		tetromino_list[0] = new Tetromino(i_block, false);
		int[][] j_block = {{1, 0, 0}, {1, 3, 1}};
		tetromino_list[1] = new Tetromino(j_block, true);
		int[][] l_block = {{0, 0, 1}, {1, 3, 1}};
		tetromino_list[2] = new Tetromino(l_block, true);
		int[][] o_block = {{3, 1}, {1, 1}};
		tetromino_list[3] = new Tetromino(o_block, false);
		int[][] s_block = {{0, 1, 1}, {1, 3, 0}};
		tetromino_list[4] = new Tetromino(s_block, true);
		int[][] t_block = {{0, 1, 0}, {1, 3, 1}};
		tetromino_list[5] = new Tetromino(t_block, true);
		int[][] z_block = {	{1, 1, 0}, {0, 3, 1}};
		tetromino_list[6] = new Tetromino(z_block, true);
	}
	
	public void write_board(Tetromino t, Point p, int icon) {
		int row, col;
		boolean[][] blst = t.get_block();
		Point pivot = t.get_pivot();
		for(int i = 0; i < blst.length; i++) {
			for(int j = 0; j < blst[0].length; j++) {
				row = p.get_row()-pivot.get_row()+i;
				col = p.get_col()-pivot.get_col()+j;
				if(blst[i][j] && row >= 0 && row < board[0].length && col >= 0 && col < board.length)
					board[row][col] = icon;
			}
		}
	}
	
	public void erase_board(Tetromino t, Point p) {
		int row, col;
		boolean[][] blst = t.get_block();
		Point pivot = t.get_pivot();
		for(int i = 0; i < blst.length; i++) {
			for(int j = 0; j < blst[0].length; j++) {
				row = p.get_row()-pivot.get_row()+i;
				col = p.get_col()-pivot.get_col()+j;
				if(blst[i][j] && row >= 0 && row < board[0].length && col >= 0 && col < board.length)
					board[row][col] = '-';
			}
		}
	}
	
	public void display_board() {
		char c;
		for(int i = 0; i < board.length; i++) {
			for(int j = 0; j < board[0].length; j++) {
				switch(board[i][j]) {
				case 1 : c = 'i'; break;
				case 2 : c = 'j'; break;
				case 3 : c = 'l'; break;
				case 4 : c = 'o'; break;
				case 5 : c = 's'; break;
				case 6 : c = 't'; break;
				case 7 : c = 'z'; break;
				default: c = '-'; break;
				}
				System.out.print(c + " ");
			}
			System.out.println();
		}
	}
	
	public void run() {
		
	}
	
	public static void main(String[] args) {
		Bytris game = new Bytris();
		game.run();
		
		int b = 6;
		Tetromino load = game.tetromino_list[b];
		Point p =  new Point(2, 4);
		
		game.write_board(load, p, b+1);
		game.display_board();
		game.erase_board(load, p);
		System.out.println();
		
		load.rotate_right();
		game.write_board(load, p, b+1);
		game.display_board();
		game.erase_board(load, p);
		System.out.println();
		
		load.rotate_right();
		game.write_board(load, p, b+1);
		game.display_board();
		game.erase_board(load, p);
		System.out.println();
		
		load.rotate_right();
		game.write_board(load, p, b+1);
		game.display_board();
		game.erase_board(load, p);
		System.out.println();
	}
}
