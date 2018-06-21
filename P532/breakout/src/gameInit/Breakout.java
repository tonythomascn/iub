/*
 * Copyright (c) 1995, 2008, Oracle and/or its affiliates. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   - Neither the name of Oracle or the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * This example, like all Swing examples, exists in a package:
 * in this case, the "start" package.
 * If you are using an IDE, such as NetBeans, this should work 
 * seamlessly.  If you are compiling and running the examples
 * from the command-line, this may be confusing if you aren't
 * used to using named packages.  In most cases,
 * the quick and dirty solution is to delete or comment out
 * the "package" line from all the source files and the code
 * should work as expected.  For an explanation of how to
 * use the Swing examples as-is from the command line, see
 * http://docs.oracle.com/javase/javatutorials/tutorial/uiswing/start/compile.html#package
 */
package gameInit;

/*
 * HelloWorldSwing.java requires no other files. 
 */
import javax.swing.*;

import dataModel.Ball;
import dataModel.Brick;
import dataModel.DigitalClock;
import dataModel.Paddle;
import gameHelper.Commons;


import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.geom.Ellipse2D;
import java.awt.RenderingHints;
import java.awt.event.*;

public class Breakout extends JPanel {
	private Ball ball;
	private Brick bricks[];
	private Paddle gamePaddle;
	private DigitalClock clock;
	//private boolean isStart = true;

	private boolean gameon = true;

	public Breakout() {
		//create paddle ball clock and bricks
		gamePaddle = new Paddle();
		ball = new Ball();
		clock = new DigitalClock();
		bricks = new Brick[Commons.BRICK_ROW * Commons.BRICK_COLUMN];
		
		ball.register(clock);
		
		int k = 0;
		for (int i = 0; i < Commons.BRICK_ROW; i++) {
			for (int j = 0; j < Commons.BRICK_COLUMN; j++) {
				bricks[k] = new Brick(j * Commons.BRICK_WIDTH + 90, i
						* Commons.BRICK_HEIGHT + 30);
				k++;
			}
		}
//monitor keyboard
		addKeyListener(new KeyListener() {

			public void keyTyped(KeyEvent keyEntered) {
			}

			public void keyReleased(KeyEvent keyEntered) {
				gamePaddle.keyReleased(keyEntered);
			}

			public void keyPressed(KeyEvent keyEntered) {
				gamePaddle.keyPressed(keyEntered);
			}

		});

		setFocusable(true);
	}

	private void checkCollison() {
		//check collision and ball position
		// if ball get out of the border, stop
		if (ball.getY() > Commons.HEIGHT) {
			this.ball.updateObservers();
			//this.clock.isStart = false;
			this.gameon = false;
		}
		//if ball hits all bricks, stop
		for (int i = 0, j = 0; i < Commons.BRICK_ROW * Commons.BRICK_COLUMN; i++) {
			if (bricks[i].isDestroyed()) {
				j++;
			}
			if (j == Commons.BRICK_ROW * Commons.BRICK_COLUMN) {
				//this.clock.isStart = false;
				this.ball.updateObservers();
				this.gameon = false;
			}
		}
		//check the collision with paddle
		ball.collisionPaddle(gamePaddle);
		//check the collision with bricks
		for (int i = 0; i < Commons.BRICK_ROW * Commons.BRICK_COLUMN; i++) {
			ball.collisionBrick(bricks[i]);
		}
	}

	private void move() {
		//move ball and paddle
		ball.move();
		gamePaddle.movePaddle();
	}

	public void paint(Graphics g) {
		super.paint(g);
		Graphics2D g2d = (Graphics2D) g;
		if (gameon) {
			//when game is on, paint everything in the canvas

			g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
			g2d.setColor(Color.DARK_GRAY);
			

			gamePaddle.paint(g2d);
			ball.paint(g2d);
			for (int i = 0; i < Commons.BRICK_COLUMN * Commons.BRICK_ROW; i++) {
				if (!bricks[i].isDestroyed())
					bricks[i].paint(g2d);
			}
		}
		clock.displayClock(g2d);
		g.dispose();
	}

	public static void main(String[] args) {
		// Schedule a job for the event-dispatching thread:
		// creating and showing this application's GUI.
		JFrame frame = new JFrame("Breakout");//Q: is this just the name?
		Breakout breakout = new Breakout();
		frame.add(breakout);
		frame.setSize(Commons.HEIGHT, Commons.WIDTH);
		frame.setVisible(true);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		//frame.setIgnoreRepaint(true);
		frame.setResizable(false);

		while (true) {
			breakout.move();
			breakout.checkCollison();
			breakout.repaint();
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
}