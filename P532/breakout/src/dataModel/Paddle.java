package dataModel;

import gameHelper.Commons;

import java.awt.Graphics2D;
import java.awt.event.KeyEvent;

import java.awt.Rectangle;

import dataModel.Coordinate;


public class Paddle extends Coordinate implements Commons {

	int xOffset;

//set initial X and Y coordinate for paddle
	public Paddle() {
		this.setX(Commons.PADDLE_X);
		this.setY(Commons.PADDLE_Y);
		xOffset = 0;
	}

	//return xcordinate
	public int returnXcoordinate() {
		return this.getX();
	}

//return Y coordinate
	public int returnYcoordinate() {
		return this.getY();
	}

//adjust paddle coordinates as per key movement
	public void movePaddle() {

//Change y coordinate		
		if (this.getX() + xOffset > 0
				&& this.getX() + xOffset < (Commons.WIDTH - Commons.PADDLE_HEIGHT)) {

			this.setX(this.getX() + xOffset);
		}
	}

//Draw paddle with new coordinates after movement
	public void paint(Graphics2D peddleImage) {
		peddleImage.fillRect(this.getX(), this.getY(), Commons.PADDLE_HEIGHT,
				Commons.PADDLE_WIDTH);
	}

//Do nothing when key released
	public void keyReleased(KeyEvent keyEntered) {
		xOffset = 0;
	}

//Get the values of key pressed for paddle movement
	public void keyPressed(KeyEvent keyEntered) {
//Up key		
		if (keyEntered.getKeyCode() == KeyEvent.VK_RIGHT)
			xOffset = 2;
//Down key		
		else if (keyEntered.getKeyCode() == KeyEvent.VK_LEFT)
			xOffset = -2;
	}
	public Rectangle getBounds() {
		return new Rectangle(this.getX(), this.getY(), Commons.PADDLE_HEIGHT,
				Commons.PADDLE_WIDTH);
	}
}
