/**
 * Bytris Project - 11/16/2019
 * Giga-Jank Tetris
 */
public class Testing {
	public static void main(String[] args) {
		int[][] i_block = {{1, 3, 1, 1}};
		Tetromino i_tetromino = new Tetromino(i_block, false);
		
		int[][] j_block = {	{1, 0, 0},
					{1, 3, 1}};
		Tetromino j_tetromino = new Tetromino(j_block, true);
		
		int[][] l_block = {	{0, 0, 1},
					{1, 3, 1}};
		Tetromino l_tetromino = new Tetromino(l_block, true);
		
		int[][] o_block = {	{3, 1},
					{1, 1}};
		Tetromino o_tetromino = new Tetromino(o_block, false);
		
		int[][] s_block = {	{0, 1, 1},
					{1, 3, 0}};
		Tetromino s_tetromino = new Tetromino(s_block, true);
		
		int[][] t_block = {	{0, 1, 0},
					{1, 3, 1}};
		Tetromino t_tetromino = new Tetromino(t_block, true);
		
		int[][] z_block = {	{1, 1, 0},
					{0, 3, 1}};
		Tetromino z_tetromino = new Tetromino(z_block, true);
		
		Tetromino alpha_tetromino = o_tetromino;
		
		for(int k = 0; k < 4; k++) {
			System.out.println("state : " + alpha_tetromino.get_state() + "\npivot : " + alpha_tetromino.get_pivot());
			boolean[][] alpha_arr = alpha_tetromino.get_block();
			for(int i = 0; i < alpha_arr.length; i++) {
				for(int j = 0; j < alpha_arr[0].length; j++)
					System.out.print(((alpha_arr[i][j]) ? "T" : "-") + " ");
				System.out.println();
			}
			System.out.println();
			alpha_tetromino.rotate_right();
		}
	}
}
