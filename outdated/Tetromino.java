import java.util.ArrayList;

/**
 * Bytris Project - 11/16/2019
 * Giga-Jank Tetris
 * 
 * Tetromino object using SRS rotation system.
 */
public class Tetromino {
	// 0 1 2 3
	private int state = 0; // 0-spawn, 1-right, 2-upside down, 3-left
	private boolean rotation_mode; // true is blockcenter {T, Z, S, L, J}, false is crosshair {O, I}
	private int[][] block; // 0-empty, 1-block, 2-rotation, 3-overlap between block&rotation
	private ArrayList<boolean[][]> block_state = new ArrayList<>();
	private ArrayList<Point> rotation_state = new ArrayList<>();
	
	/**
	 * If rotationMode is true, the point will be in the center of the block.
	 * If rotationMode is false, the point will be lower right corner of the block.
	 */
	public Tetromino(int[][] block, boolean rotationMode) {
		this.block = block;
		this.rotation_mode = rotationMode;
		this.generate_states();
	}
	
	/**
	 * Populate rotation_state and rotation_point
	 */
	private void generate_states() {
		int balance = rotation_mode ? 1 : 0;
		boolean[][] bool_arr;
		int max_j, max_k, block_j, block_k;
		for(int i = 0; i < 4; i++) {
			max_j = i % 2 == 0 ? block.length : block[0].length;
			max_k = i % 2 == 0 ? block[0].length : block.length;
			bool_arr = new boolean[max_j][max_k];
			for(int j = 0; j < max_j; j++) {
				for(int k = 0; k < max_k; k++) {
					if(i == 0) {
						block_j = j;
						block_k = k;
					} else if(i == 1) {
						block_j = block.length-1-k;
						block_k = j;
					} else if(i == 2) {
						block_j = block.length-1-j;
						block_k = block[0].length-1-k;
					} else {
						block_j = k;
						block_k = block[0].length-1-j;
					}
					if(block[block_j][block_k] == 3 || block[block_j][block_k] == 3) { // found pivot
						switch(i) {
						case 0: rotation_state.add(new Point(j, k)); break;
						case 1: rotation_state.add(new Point(j, k-1+balance)); break;
						case 2: rotation_state.add(new Point(j-1+balance, k-1+balance)); break;
						default: rotation_state.add(new Point(j-1+balance, k)); break;
						}
					}
					bool_arr[j][k] = block[block_j][block_k] != 0 && block[block_j][block_k] != 2;
				}
			}
			block_state.add(bool_arr);
		}
	}
	
	public void rotate_right() {
		state++;
		if(state > 3)
			state = 0;
	}
	
	public void rotate_left() {
		state--;
		if(state < 0)
			state = 3;
	}
	
	public int get_state() {
		return state;
	}
	
	public boolean[][] get_block() {
		return block_state.get(state);
	}
	
	public Point get_pivot() {
		return rotation_state.get(state);
	}
	
	public static void main(String[] args) {
		int[][] i_block = {{3, 1}, {1, 1}};
		Tetromino t = new Tetromino(i_block, false);
		for(boolean[][] i : t.block_state) {
			for(int j = 0; j < i.length; j++) {
				for(int k = 0; k < i[j].length; k++)
					System.out.print(i[j][k] + " ");
				System.out.println();
			}
			System.out.println();
		}
	}
}
