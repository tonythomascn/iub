package dataModel;

import java.util.Calendar;
import java.awt.Graphics;
import java.awt.Graphics2D;

import observerModel.Observer;

public class DigitalClock implements Observer{
	
	private Calendar currentTime;
	private int currentHour;
	private int currentMinute;
	private int currentSecond;
    String storedTime;
    public boolean isStart = true;
    
    public void displayClock(Graphics2D gameImage)
    {
    	String displayTime;
       
    	currentTime = Calendar.getInstance();
        currentHour = currentTime.get(Calendar.HOUR_OF_DAY);
        currentMinute = currentTime.get(Calendar.MINUTE);
        currentSecond = currentTime.get(Calendar.SECOND);
        
    	if (currentHour < 10)
    		displayTime = "0" + currentHour + ":";
    	else 
    		displayTime = currentHour + ":";
    	
    	if (currentMinute < 10)
    		displayTime = displayTime + "0" + currentMinute + ":";
    	else 
    		displayTime = displayTime +currentMinute + ":";
    	
    	if (currentSecond < 10)
    		displayTime = displayTime + "0" + currentSecond;
    	else 
    		displayTime = displayTime +currentSecond;

    	if (isStart == true)
    		storedTime = displayTime;
    	
    	gameImage.drawString(storedTime, 530, 20);
    }

    
}
