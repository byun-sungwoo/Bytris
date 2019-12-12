// 2019-12-07
// Bytris - byun-sungwoo

#include <stm32f30x.h> // Pull in include files for F30x standard drivers
#include <f3d_uart.h>
#include <f3d_user_btn.h>
#include <f3d_lcd_sd.h>
#include <f3d_i2c.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <f3d_nunchuk.h>
#include <f3d_rtc.h>
#include <f3d_systick.h>
#include <stdio.h>
#include <math.h>
#include "board.c"
#define MAXBUF 10
#define ORANGE 0x0ABF
#define GHOST 0xFFFF

// Delay handler
// Down tick every tick cycle.
// Scan for nc input first, if left/right input && on floor, prevent tick.
// Prevent tick up until 20s have passed from inital block spawn.
// 	Once 20s have passed, automatically harddrop

// tetris sprint
int main(void) {
	// settings
	int ncdelay = 5;		// nunchuck refresh rate
	double tickrate = 1;		// block down tick
	double tick = 0.015;
	int autodrop = 20;
	// initializations
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	f3d_uart_init();
	f3d_user_btn_init();
	f3d_i2c1_init();
	f3d_gyro_init();
	f3d_lcd_init();
	delay(10);
	f3d_accel_init();
	delay(10);
	f3d_mag_init();
	delay(10);
	f3d_nunchuk_init();
	// tetris variables
	int nccounter = ncdelay;
	int sprintgoal = 40;
	int cmpboard[HEIGHT][WIDTH];
	char buf[MAXBUF];
	nunchuk_t nc;
	int draw_sidebar = 1;
	int linecounter = 0;
	int uj = 1;	// up down joystick. prevents spam using up down.
	int zb = 1;	// z button nunchuk. prevents spam using z.
	int cb = 1;	// c button nunchuk. prevents spam using c.
	int blsc = 8;	// block scale
	int y_max = 20;
	int x_max = 10;
	int y,x;
	int ghostr = 0;
	int ghostc = 0;
	int ghostshift = 0;
	tetromino ghost;
	int holdinit = 1;
	int canhold = 0;
	int canholdreset = 0;
	// text
	char *line_str = "cleared";
	int y_line = 2*4;
	int x_line = (17)*4;
	char *time_str = "time";
	int y_time = 4*4;
	int x_time = (17)*4;
	double time = 0;
	char *hold_str = "hold";
	int y_hold = 7*4;
	int x_hold = (20+1)*4;
	char *next_str = "next";
	int y_next = 13*4;
	int x_next = (20+1)*4;
	// other
	int seconds = 1;
	int playerinput = 1;
	double timesinceinput = 0;
	int oldposr = 0;
	int oldposc = 0;
	int oldblocktype = 0;
	int holdblocktype = 0;
	int i,j,k;
	int refresh = 0;
	double autodropcounter = 0;
	int notbottom = 1;
	// draw empty board & text
	f3d_lcd_fillScreen2(BLACK);	// clear screen
	for(y=0;y<y_max;y++) {		// draw grid pattern
		for(x=0;x<x_max;x++) {
			//[text]
			// gcvt(board[y+1][x], 6, buf);
			// f3d_lcd_drawString(x*blsc,y*blsc,"_",WHITE,BLACK);
			//[dot]
			f3d_lcd_fillcertain(x*blsc,y*blsc,x*blsc+1,y*blsc+1,WHITE);
			if(x+1==x_max)
				f3d_lcd_fillcertain((x+1)*blsc-1,(y+1)*blsc,(x+1)*blsc,(y+1)*blsc+1,WHITE);
			//[squares]
			// f3d_lcd_fillcertain(x*blsc,y*blsc,(x+1)*blsc,y*blsc+1,WHITE);
			// f3d_lcd_fillcertain(x*blsc,y*blsc,x*blsc+1,(y+2)*blsc,WHITE);
			// if(x+1==x_max)
			// 	f3d_lcd_fillcertain((x+1)*blsc,y*blsc,(x+1)*blsc+1,(y+2)*blsc,WHITE);
		}
		// if(y+1==y_max)
		// 	f3d_lcd_fillcertain(0,y_max*blsc-1,x_max*blsc,y_max*blsc,WHITE);
	}
	int npad = 4*4;
	// f3d_lcd_drawString(x_line, y_line, line_str, WHITE, BLACK);
	gcvt(linecounter, 6, buf);
	// f3d_lcd_drawString(x_line+npad, y_line, buf, WHITE, BLACK);
	// f3d_lcd_drawString(x_time, y_time, time_str, WHITE, BLACK);
	// gcvt(time, 6, buf);
	// f3d_lcd_drawString(x_time+npad, y_time, buf, WHITE, BLACK);
	// f3d_lcd_drawString(x_hold, y_hold, hold_str, WHITE, BLACK);
	// f3d_lcd_drawString(x_next, y_next, next_str, WHITE, BLACK);
	
	// start game
	f3d_nunchuk_read(&nc);
	int kindarandom = nc.ay;
	printf("rand:%d\n",kindarandom);
	initgame(kindarandom);
	
	while(1) {
		// if over 20s, hd if so
		// else read for nc input
		// if no nc input
		// 	downtick, lockin if floor
		// if playerinput input
		// 	if room, downtick
		// 	else do nothing
		if(autodropcounter > autodrop) {
			linecounter += hdropcurrent();
			autodropcounter = 0;
			refresh = 1;
			draw_sidebar = 1;
			playerinput = 1;
		} else {
			notbottom = 1;
			// Nunchuk Input
			f3d_nunchuk_read(&nc);
			if(0 <= nc.jx && nc.jx <= 64) {			// Left Joystick
				if(nccounter == ncdelay)
					shiftcurrent(0,-1);
				nccounter--;
				playerinput = 1;
			}
			if(190 <= nc.jx && nc.jx <= 255) {		// Right Joystick
				if(nccounter == ncdelay)
					shiftcurrent(0,1);
				nccounter--;
				playerinput = 1;
			}
			if(0 <= nc.jy && nc.jy <= 64) {			// Down Joystick
				if(nccounter == ncdelay)
					shiftcurrent(1,0);
				nccounter--;
				playerinput = 1;
			}
			if(cb == 1 && nc.c == 1) {			// C Button
				// debugging start
				// oldblocktype = blocktype;
				// nextcurrent();
				// refresh = 1;
				// debugging end

				rleftcurrent();
				cb = 0;
				playerinput = 1;
			}
			if(nc.z == 1 && (190 <= nc.jy && nc.jy <= 255)) {	// Hold
				if(holdinit) {
					while(current.state != 0)
						rleft(&current);
					memcpy(&hold,&current,sizeof(tetromino));
					removeblock(&current,posr,posc);
					holdblocktype = blocktype;
					nextcurrent();
					placeblock(&current,posr,posc);
					autodropcounter = 0;
					refresh = 1;
					draw_sidebar = 1;
					playerinput = 1;
					holdinit = 0;
					canhold = 0;
					pdata(&hold);
					printf("\n\n\n");
				} else if(canhold) {
					while(current.state != 0)
						rleft(&current);
					int tempblocktype = blocktype;
					blocktype = holdblocktype;
					holdblocktype = tempblocktype;
					tetromino temptet;
					removeblock(&current,posr,posc);
					memcpy(&temptet,&current,sizeof(tetromino));
					memcpy(&current,&hold,sizeof(tetromino));
					memcpy(&hold,&temptet,sizeof(tetromino));
					placeblock(&current,posr,posc);
					autodropcounter = 0;
					refresh = 1;
					draw_sidebar = 1;
					playerinput = 1;
					posc = 5;
					posr = 1;
					canhold = 0;
					pdata(&hold);
					printf("\n\n\n");
				}
			}
			else if(uj == 1 && (190 <= nc.jy && nc.jy <= 255)) {	// Up Joystick
				rrightcurrent();
				uj = 0;
				playerinput = 1;
			}
			else if(zb == 1 && nc.z == 1) {			// Z Button
				oldblocktype = blocktype;
				linecounter += hdropcurrent();
				autodropcounter = 0;
				zb = 0;
				refresh = 1;
				draw_sidebar = 1;
				playerinput = 1;
			}
			if(uj == 0 && nc.jy >= 64 && nc.jy <= 190)	// Up Reset
				uj = 1;
			if(zb == 0 && nc.z == 0)			// Z Button Reset
				zb = 1;
			if(cb == 0 && nc.c == 0)			// C Button Reset
				cb = 1;
			// tickrate
			if(time>tickrate*seconds) {
				notbottom = shiftcurrent(1,0);
				if(!notbottom && playerinput == 0) {
					linecounter += hdropcurrent();
					autodropcounter = 0;
					refresh = 1;
					draw_sidebar = 1;
					playerinput = 1;
				}
				seconds++;
			}
		}
		// int shiftstatus = shiftcurrent(1,0);
		if(!playerinput && !notbottom) {
			linecounter += hdropcurrent();
			autodropcounter = 0;
			refresh = 1;
			draw_sidebar = 1;
			playerinput = 1;
		}
		if(gameover)
			break;
		// draw ghost
		// 	Draw only when posc has changed or rotation or when autodrop is 0
		/*if(time != 0) ghostshift = hdropshiftcurrent();
		if(oldposc != posc || cb == 0 || uj == 0 || autodropcounter == 0) {
			for(i=0;i<ghost.r;i++) {
				for(j=0;j<ghost.c;j++) {
					if(ghost.data[i][j] != 0) {
						int tobot = oldposr+ghostshift-1;
						int fr = (i+tobot-ghost.n)*blsc;
						int fc = (j+oldposc-ghost.m)*blsc;
						if(	board[i+tobot-ghost.n][j+oldposc-ghost.m] == 0 &&
							fc >= 0 && fc+blsc-1 <= 128 && fr >= 0 && fr+blsc-1 <= 160) {
							f3d_lcd_fillcertain(fc,fr,fc+blsc-1,fr+blsc-1,BLACK);
							f3d_lcd_fillcertain(fc,fr,fc+1,fr+1,WHITE);
						}
					}
				}
			}
			ghostshift = hdropshiftcurrent();
			for(i=0;i<current.r;i++) {
				for(j=0;j<current.c;j++) {
					if(current.data[i][j] != 0) {
						int tobot = posr+ghostshift-1;
						int fr = (i+tobot-current.n)*blsc;
						int fc = (j+posc-current.m)*blsc;
						printf("%d,%d\n\n\n",fr,fc);
						if(	board[i+tobot-ghost.n][j+oldposc-ghost.m] == 0 &&
							fc >= 0 && fc+blsc-1 <= 128 && fr >= 0 && fr+blsc-1 <= 160)
							f3d_lcd_fillcertain(fc,fr,fc+blsc-1,fr+blsc-1,GHOST);
					}
				}
			}
		}*/
		// draw blocks
		int hdrop = hdropshiftcurrent();
		for(i=0;i<HEIGHT-1;i++) {
			if(i==0)	// not sure why the upper right is being overwritten
				f3d_lcd_fillcertain(WIDTH*blsc-1,i*blsc,WIDTH*blsc,i*blsc+1,WHITE);
			for(j=0;j<WIDTH;j++) {
				int fr = i*blsc;
				int fc = j*blsc;
				if(j+1==WIDTH)
					f3d_lcd_fillcertain(fc+blsc-1,fr+blsc,fc+blsc,fr+blsc+1,WHITE);
				if(	((board[i+1][j] > 0 && cmpboard[i+1][j] == 0) || (board[i+1][j] == 0 && cmpboard[i+1][j] > 0)) ||
					(board[i+1][j] == 1 && cmpboard[i+1][j] == 1 && blocktype != oldblocktype & refresh)) {
					if(board[i+1][j] == 0) {
						//<normal>
						f3d_lcd_fillcertain(fc,fr,fc+blsc-1,fr+blsc-1,BLACK);
						//[string]
						// gcvt(board[i+1][j], 6, buf);
						// f3d_lcd_drawString(fc,fr,"_",WHITE,BLACK);
						//[dot]
						f3d_lcd_fillcertain(fc,fr,fc+1,fr+1,WHITE);
						//[box]
						// f3d_lcd_fillcertain(fc,fr,fc+blsc,fr+1,WHITE);
						// f3d_lcd_fillcertain(fc,fr,fc+1,fr+blsc*2,WHITE);
						//if(j+1==WIDTH)
						//	f3d_lcd_fillcertain((j+2)*blsc-1,i*blsc,(j+1)*blsc*blsc,(i+2)*blsc,WHITE);
					} else {
						// [why the fuck]
						int clr = board[i+1][j] == 1 ? blocktype : oldblocktype;
						//<normal>
						// printf("%d ",clr);
						//[string]
						// gcvt(board[i+1][j], 6, buf);
						// gcvt(clr, 6, buf);
						// f3d_lcd_drawString(fc,fr,buf,WHITE,BLACK);
						//[color box]
						switch(clr) {
						case 0: f3d_lcd_fillcertain(fc,fr,fc+blsc-1,fr+blsc-1,GREEN);break;
						case 1: f3d_lcd_fillcertain(fc,fr,fc+blsc-1,fr+blsc-1,RED);break;
						case 2: f3d_lcd_fillcertain(fc,fr,fc+blsc-1,fr+blsc-1,ORANGE);break;
						case 3: f3d_lcd_fillcertain(fc,fr,fc+blsc-1,fr+blsc-1,BLUE);break;
						case 4: f3d_lcd_fillcertain(fc,fr,fc+blsc-1,fr+blsc-1,MAGENTA);break;
						case 5: f3d_lcd_fillcertain(fc,fr,fc+blsc-1,fr+blsc-1,CYAN);break;
						default: f3d_lcd_fillcertain(fc,fr,fc+blsc-1,fr+blsc-1,YELLOW);break;
						}
					}
				}
			}
		}
		// redraw 1 blocks if a block placed
		if(refresh)
			refresh = 0;
		// draw sidebar if block placed
		if(draw_sidebar == 1) {
			// draw queue
			gcvt(linecounter, 6, buf);
			f3d_lcd_drawString(x_line+npad, y_line, buf, WHITE, BLACK);
			int queuegap = 3;
			// for(i=0;i<7;i++) {
			// 	tetromino ttmp = queue[i];
			// 	for(j=0;j<ttmp.r)
			// }
			draw_sidebar = 0;
		}
		// update time
		if(autodropcounter == 0)
			canholdreset++;
		if(canholdreset >= 2) {
			canholdreset = 0;
			canhold = 1;
		}
		time += tick;
		autodropcounter += tick;
		if((playerinput == 0 && posr == oldposr) || posr == oldposr)
			timesinceinput += tick;
		else
			timesinceinput = 0;
		gcvt(time, 6, buf);
		f3d_lcd_drawString(x_time+npad, y_time, buf, WHITE, BLACK);
		// check if game over
		if(linecounter >= sprintgoal)
			break;
		// update
		if(nccounter <= 0)
			nccounter = ncdelay;
		if(!playerinput)
			nccounter = ncdelay;
		oldposr = posr;
		oldposc = posc;
		memcpy(&ghost,&current,sizeof(tetromino));
		playerinput = 0;
		memcpy(cmpboard,board,sizeof(int)*HEIGHT*WIDTH);
	}
	while(1);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */
  while(1);
}
#endif
