package main.fm.cs2.push;

import java.util.Calendar;
import java.util.Date;

import mian.fm.cs2.online.R;

import org.cocos2dx.lib.Cocos2dxLuaJavaBridge;

import android.app.AlarmManager;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;


public class NotificationInterface {
	public static Context context;
	public static AlarmManager alarmManager = null;
//	private static SharedPreferences sharedPreferences = context.getSharedPreferences("pushText", Context.MODE_PRIVATE); 
	private static int pushStateCallback;
	public static void setContext(Context context){
		NotificationInterface.context = context;
		alarmManager =  (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
	}


	public static void setAlarm(int tag, double delaytime){
		Log.d("PushService", "setAlarm ----" + context.getPackageName());		
		Log.d("PushService", "delaytime  = " + delaytime);
		if (delaytime < 0) 
		{
			delaytime = delaytime + 24 * 3600;
		}
		long value = (long) delaytime;
		Log.d("PushService", "delaytime  === " + delaytime);
		long triggerTime = System.currentTimeMillis() + value * 1000L;
		Intent intent = new Intent(context, PushMessageService.class);//(context.getPackageName() + ".alarm");
		intent.putExtra("title", context.getString(R.string.app_name));
		intent.putExtra("ID", tag);
		intent.putExtra("time", delaytime);
		PendingIntent pendingIntent = PendingIntent.getService(context, tag, intent, PendingIntent.FLAG_UPDATE_CURRENT);
		//AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
		alarmManager.setRepeating(AlarmManager.RTC_WAKEUP, triggerTime,  AlarmManager.INTERVAL_DAY, pendingIntent);
//		
	}

	public static void cancelAlarm() {
		Log.d("PushService", "cancelAlarm ----");
		//AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
		SharedPreferences sharedPreferences = context.getSharedPreferences("pushText", Context.MODE_PRIVATE); 
		int length = sharedPreferences.getInt("alarmNums", 0);	
		for (int i =0; i< length; i ++) {
			Intent intent = new Intent(context, PushMessageService.class);//(context.getPackageName() + ".alarm");
			alarmManager.cancel(PendingIntent.getService(context, i, intent, PendingIntent.FLAG_UPDATE_CURRENT));
		}
		String ns = Context.NOTIFICATION_SERVICE;
		NotificationManager mNotificationManager = (NotificationManager) context.getSystemService(ns);
		mNotificationManager.cancelAll();
	}
	
	public static void setPushText(String text){
		Log.d("PushService", text);
		String[] content=text.split("-&&&-");
		SharedPreferences sharedPreferences = context.getSharedPreferences("pushText", Context.MODE_PRIVATE); 
		Editor editor = sharedPreferences.edit();
		Log.d("PushService", content.length + "");
		editor.putInt("alarmNums", content.length);
		Log.d("PushService", "alarmNums = "+ content.length);
	    for (int i = 0; i < content.length; i++) {
	    	Log.d("PushService", "content = " + content[i]);
	    	String[] content1 = content[i].split("-&&-");
	    	String alramTime = content1[0];
	    	editor.putString("alarmTime"+i, alramTime);
	    	Log.d("PushService", "alarmTime" + i + " = " + alramTime);
	    	String alramText = content1[1];
	    	String[] showTexts = alramText.split("-&-");
	    	editor.putInt("alarmTime"+i+"length", showTexts.length);
	    	for (int j=0; j < showTexts.length; j ++){
	    		editor.putString("alarmTime" + i + "text" + j, showTexts[j]);
	    		Log.d("PushService", "alarmTime" + i + "text" + j + " = " + showTexts[j]);
	    	}
	    }	    	

		editor.commit();
		cancelAlarm();
		startAlarm();
	}
	
	public static void setPushState(boolean state){
		SharedPreferences sharedPreferences = context.getSharedPreferences("pushText", Context.MODE_PRIVATE); 
		Editor editor = sharedPreferences.edit();
		editor.putBoolean("switch_state", state);
		editor.commit();
		if (state){
			startAlarm();
		}
		else{
			cancelAlarm();
		}		
	}

	public static boolean getPushState(int functionID){
		pushStateCallback = functionID;
		SharedPreferences sharedPreferences = context.getSharedPreferences("pushText", Context.MODE_PRIVATE); 
		boolean switch_state = sharedPreferences.getBoolean("switch_state", true);
		if (switch_state){
			Cocos2dxLuaJavaBridge.callLuaFunctionWithString(pushStateCallback, "true");
		}
		else{
			Cocos2dxLuaJavaBridge.callLuaFunctionWithString(pushStateCallback, "false");
		}
		return switch_state;
	}
	
	public static void startAlarm(){		
		SharedPreferences sharedPreferences = context.getSharedPreferences("pushText", Context.MODE_PRIVATE); 
		int length = sharedPreferences.getInt("alarmNums", 0);
		Log.d("PushService", "startAlarm alarmNums " + length);
		if (length <= 0){
			return;
		}	
		boolean switch_state = sharedPreferences.getBoolean("switch_state", true);
		
		if (!switch_state){
			return;
		}
        
        Log.d("PushService", "ssssssssstartttttttt");
        for (int i=0; i<length; i++){
        	Date curDate =  new Date(System.currentTimeMillis());        
            Calendar c = Calendar.getInstance();  
        	String alarmTime = sharedPreferences.getString("alarmTime"+i, "12 00");
        	Log.d("PushService", "alarmTime"+i + " = " + alarmTime);
        	String[] time = alarmTime.split(" ");
        	int start_index = alarmTime.indexOf(" ");
        	Log.d("PushService", "start_index=====" + start_index);
        	if (start_index == -1){
        		time = new String[2];
        		time[0] = "12";
        		time[1] = "0";
        	}
        	else{
        		time = alarmTime.split(" ");
        	}
        	c.setTimeInMillis(System.currentTimeMillis());   
        	Log.d("PushService", "alarmTime=====" + time[0] + "," + time[1]);
            c.set(Calendar.HOUR_OF_DAY,Integer.parseInt(time[0]));  
            c.set(Calendar.MINUTE,Integer.parseInt(time[1]));  
            c.set(Calendar.SECOND,0);  
            c.set(Calendar.MILLISECOND,0); 
            long time1 = (c.getTime().getTime() - curDate.getTime()) / 1000;
            Log.d("PushService", "time" + i + " = " + time1);
            setAlarm(i, time1);

        	Log.d("PushService", i + " - " + alarmTime + " - " + Integer.parseInt(time[0]) + " - " + Integer.parseInt(time[1]));
        }
	}
	
	public static int getStringNums(){
		SharedPreferences sharedPreferences = context.getSharedPreferences("pushText", Context.MODE_PRIVATE); 
		int length = sharedPreferences.getInt("length", 0);	
		return length;
	}
	
	public static String getStringByID(int id){
		SharedPreferences sharedPreferences = context.getSharedPreferences("pushText", Context.MODE_PRIVATE);
		String text = sharedPreferences.getString("name"+id, "");
		return text;
	}
}
