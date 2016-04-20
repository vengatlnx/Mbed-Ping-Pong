#include "mbed.h"
#include "ST7567.h"
#include "rtos.h"

#define SCREEN_WIDTH 127
#define SCREEN_HEIGHT 63
#define PADDLE_LWIDTH 3
#define PADDLE_RWIDTH 127
#define PADDLE_HEIGHT 25
#define BLACK 1
#define WHITE 0

ST7567 lcd(P1_24, P1_20, P1_25, P1_22, P1_21); // mosi, sclk, reset, A0, nCS
DigitalIn key1(P1_19);
DigitalIn key2(P1_27);
DigitalIn key3(P1_31);
DigitalIn key4(P2_13);

struct paddles {
    int x_coord;
    int old_y_coord;
    int new_y_coord;
} left_paddle, right_paddle;

struct balls {
    int radius;
    int old_x_coord;
    int new_x_coord;
    int old_y_coord;
    int new_y_coord;
    int speedx;
    int speedy;
} ball;


/* Function to initialize ball */
void _init_ball() {
    ball.radius = 2;
    ball.old_x_coord = 5;
    ball.new_x_coord = 62;
    ball.old_y_coord = 5;
    ball.new_y_coord = 30;

    lcd.circle(ball.new_x_coord, ball.new_y_coord, ball.radius, BLACK);

    ball.speedx = 1;
    ball.speedy = 1;
}

/* Function to initialize left paddle */
void _init_left_paddle() {
    left_paddle.x_coord = 0;
    left_paddle.old_y_coord = 0;
    left_paddle.new_y_coord = 0;
    
    lcd.rect(left_paddle.x_coord, left_paddle.new_y_coord, PADDLE_LWIDTH,
	     PADDLE_HEIGHT, BLACK);
}


/* Function to initialize right paddle */
void _init_right_paddle() {
    right_paddle.x_coord = 124;
    right_paddle.old_y_coord = 0;
    right_paddle.new_y_coord = 0;
    
    lcd.rect(right_paddle.x_coord, right_paddle.new_y_coord, PADDLE_RWIDTH,
	     PADDLE_HEIGHT, BLACK);
}


/* Initialize screen */
void init() {
    lcd.set_contrast(0x3F);
    lcd.cls();

    _init_left_paddle();
    _init_right_paddle();
    _init_ball();
}


/* Function to move left paddle */
void _move_left_paddle() {
    lcd.rect(left_paddle.x_coord, left_paddle.old_y_coord, PADDLE_LWIDTH,
	     left_paddle.old_y_coord + PADDLE_HEIGHT, BLACK);
    lcd.fillrect(left_paddle.x_coord, left_paddle.old_y_coord, PADDLE_LWIDTH,
	     left_paddle.old_y_coord + PADDLE_HEIGHT, WHITE);
    lcd.rect(left_paddle.x_coord, left_paddle.new_y_coord, PADDLE_LWIDTH,
	     left_paddle.new_y_coord + PADDLE_HEIGHT, BLACK);
}

/* Function to move right paddle */
void _move_right_paddle() {
    lcd.rect(right_paddle.x_coord, right_paddle.old_y_coord, PADDLE_RWIDTH,
	     right_paddle.old_y_coord + PADDLE_HEIGHT, BLACK);
    lcd.fillrect(right_paddle.x_coord, right_paddle.old_y_coord, PADDLE_RWIDTH,
	     right_paddle.old_y_coord + PADDLE_HEIGHT, WHITE);
    lcd.rect(right_paddle.x_coord, right_paddle.new_y_coord, PADDLE_RWIDTH,
	     right_paddle.new_y_coord + PADDLE_HEIGHT, BLACK);
}


/* Function to move paddle up and down */
void move_paddle() {
	if (key1 == 0) {
	    if (left_paddle.new_y_coord + PADDLE_HEIGHT < SCREEN_HEIGHT) {
		// Left paddle down
		left_paddle.old_y_coord = left_paddle.new_y_coord;
		left_paddle.new_y_coord++;

		_move_left_paddle();
	    }
	} else if (key2 == 0) {
	    //Left paddle up
	    if (left_paddle.new_y_coord > 0) {
		left_paddle.old_y_coord = left_paddle.new_y_coord;
		left_paddle.new_y_coord--;

		_move_left_paddle();
	    }
	} else if (key3 == 0) {
	    if (right_paddle.new_y_coord + PADDLE_HEIGHT < SCREEN_HEIGHT) {
		// Right paddle down
		right_paddle.old_y_coord = right_paddle.new_y_coord;
		right_paddle.new_y_coord++;

		_move_right_paddle();
	    }
	} else if (key4 == 0) {
	    if (right_paddle.new_y_coord > 0) {
		// Right paddle up
		right_paddle.old_y_coord = right_paddle.new_y_coord;
		right_paddle.new_y_coord--;

		_move_right_paddle();
	    }
	}
}


/* Function to move ball */
void move_ball(void const *argument) {
    while (1) {
	ball.old_x_coord = ball.new_x_coord;
	ball.old_y_coord = ball.new_y_coord;
	ball.new_x_coord = ball.new_x_coord + ball.speedx;
	ball.new_y_coord = ball.new_y_coord + ball.speedy;

	lcd.circle(ball.old_x_coord, ball.old_y_coord, ball.radius, BLACK);
	lcd.fillcircle(ball.old_x_coord, ball.old_y_coord, ball.radius, WHITE);
	lcd.circle(ball.new_x_coord, ball.new_y_coord, ball.radius, BLACK);

	if (ball.new_y_coord - ball.radius < 0)
	    ball.speedy = -ball.speedy;
	if (ball.new_y_coord + ball.radius > SCREEN_HEIGHT)
	    ball.speedy = -ball.speedy;

	wait(0.05);

	if (ball.new_x_coord - ball.radius < 5) {
	    if ((ball.new_y_coord > left_paddle.new_y_coord) &&
		(ball.new_y_coord < left_paddle.new_y_coord + PADDLE_HEIGHT)) {
		ball.speedx = -ball.speedx;
		ball.speedy = -ball.speedy;
	    } else {
		init();
	    }
	}
	if (ball.new_x_coord + ball.radius > 122) {
	    if ((ball.new_y_coord > right_paddle.new_y_coord) &&
		(ball.new_y_coord < right_paddle.new_y_coord + PADDLE_HEIGHT)) {
		ball.speedx = -ball.speedx;
		ball.speedy = -ball.speedy;
	    } else {
		init();
	    }
	} 
    }
}

/* Main Function */
int main(void) {
    init();

    Thread thread(move_ball);
    while(1) {
	move_paddle();
    }
}
