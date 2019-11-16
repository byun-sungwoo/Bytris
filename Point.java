/**
 * Bcoltris Project - 11/16/2019
 * Giga-Jank Tetris
 */
public class Point {
	private int row;
	private int col;
	
	public Point(int row, int col) {
		this.row = row;
		this.col = col;
	}
	
	public void set(int row, int col) {
		this.row = row;
		this.col = col;
	}
	
	public void set_row(int row) { this.row = row; }
	public void set_col(int col) { this.col = col; }
	public int get_row() { return this.row; }
	public int get_col() { return this.col; }
	
	public String toString() {
		return "[" + this.row + ", " + this.col + "]";
	}
}
