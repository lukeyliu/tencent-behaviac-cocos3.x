package main.fm.cs2.push;

import java.util.List;

import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class TimeChangeReceiver extends BroadcastReceiver {
	private Context context;
	@Override
	public void onReceive(Context context1, Intent intent) {
		// TODO Auto-generated method stub
		String action = intent.getAction();
		context = context1;
		if (action.equals(Intent.ACTION_TIME_TICK)) {
			// do what you want to do ...13
			Log.d("PushService", "time tick");
//			Toast.makeText(context1, "time tick", Toast.LENGTH_SHORT).show();
			if (!isServiceRunning("PushMessageService")){
				NotificationInterface.setContext(context);
		        NotificationInterface.cancelAlarm();
		        NotificationInterface.startAlarm();
			}
			else{
				Log.d("PushService", "service is  running");
			}
		}
	}
	
    public boolean isServiceRunning(String className) {
		boolean isRunning = false;
		ActivityManager activityManager = (ActivityManager) context
				.getSystemService(Context.ACTIVITY_SERVICE);
		List<ActivityManager.RunningServiceInfo> serviceList = activityManager
				.getRunningServices(100);
		if (!(serviceList.size() > 0)) {
			return false;
		}
//		Log.d("PushService", "main.fm.cs2.push."+className);
		for (int i = 0; i < serviceList.size(); i++) {
//			Log.d("PushService", serviceList.get(i).service.getClassName());
			if (serviceList.get(i).service.getClassName().equals("main.fm.cs2.push."+className) == true) {
				isRunning = true;
				break;
			}
		}
		return isRunning;
    }

}
