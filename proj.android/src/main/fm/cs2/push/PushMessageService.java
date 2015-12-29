package main.fm.cs2.push;

import java.util.List;

import main.fm.cs2.AppActivity;
import mian.fm.cs2.online.R;
import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.app.KeyguardManager;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.IBinder;
import android.util.Log;

@SuppressLint("NewApi")
public class PushMessageService extends Service{
	Context context = null;
	
	@Override
	public void onCreate() {
		// TODO Auto-generated method stub
		Log.d("PushService","onCreate");
		super.onCreate();
		context = this;
	}
	
	@Override
	public IBinder onBind(Intent arg0) {
		return null;
	}

	public void onStart(Intent intent, int startId) {
		Log.d("PushService", "onStart");
		if (intent != null)
	    {
//			if(this.isActivityRuning()){
//				Log.d("PushService", "onStart  Running");
//				if (!this.isBackgroundRunning())
//				{
//					Log.d("PushService", "onStart back");
//					return ;
//				}
//			}
			Log.d("PushService", "ppppp");
			int tag = intent.getExtras().getInt("ID");
			SharedPreferences sharedPreferences = context.getSharedPreferences("pushText", Context.MODE_PRIVATE); 
			int length = sharedPreferences.getInt("alarmTime" + tag + "length", 0);	
			
			Log.d("PushService", "---- length = " + length);
			int ran = (int) (Math.random()*(length-1));
			Log.d("PushService", "---- ran = " + ran);
			NotificationManager localNotificationManager = (NotificationManager)getSystemService("notification");
			Intent localIntent = new Intent(this, AppActivity.class);
			localIntent.addFlags(603979777);
			PendingIntent localPendingIntent = PendingIntent.getActivity(this, 0, localIntent, PendingIntent.FLAG_UPDATE_CURRENT);
			
			String title = intent.getExtras().getString("title");
			String text = sharedPreferences.getString("alarmTime" + tag + "text" + ran, "");			
			Log.d("PushService", "---- text = " + text);
			Notification.BigTextStyle textStyle = new Notification.BigTextStyle();
	         textStyle
	                 .setBigContentTitle(title)
	                 .setSummaryText("")
	                 .bigText(text);
	         
			Notification.Builder localBuilder = new Notification.Builder(this);
			localBuilder.setAutoCancel(true);
			localBuilder.setContentIntent(localPendingIntent);
			localBuilder.setDefaults(-1);
			localBuilder.setContentTitle(title);
			localBuilder.setContentText(text);
			localBuilder.setStyle(textStyle);
			localBuilder.setTicker(text);
			localBuilder.setSmallIcon(R.drawable.icon);
			localNotificationManager.notify(intent.getExtras().getInt("ID"), localBuilder.build());
	    }
	}
	
	@Override   
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.d("PushService","flags="+flags);
		return super.onStartCommand(intent, Service.START_REDELIVER_INTENT, startId);
	}
	
	@Override
	public void onDestroy() {
		// TODO Auto-generated method stub
		Log.d("PushService", "Service   onDestroy ");
		super.onDestroy();
		NotificationInterface.setContext(context);
		NotificationInterface.cancelAlarm();
		NotificationInterface.startAlarm();	
	}	
	
	@Override
	public boolean stopService(Intent name) {
		// TODO Auto-generated method stub		
		Log.d("PushService", "Service   stopService");		
		return super.stopService(name);
	}
	
	
	//̬
	public boolean isActivityRuning() {
		String packageName = this.getPackageName();
		boolean isAppRunning = false;

		ActivityManager manager = (ActivityManager) context
				.getSystemService(Context.ACTIVITY_SERVICE);

		List<RunningTaskInfo> list = manager.getRunningTasks(100);
		RunningTaskInfo info;
		for (int i = 0; i < list.size(); i++) {
			info = list.get(i);
			if (info.topActivity.getPackageName().equals(packageName)) {
				isAppRunning = true;
				break;
			}
		}
		
		return isAppRunning;

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
		for (int i = 0; i < serviceList.size(); i++) {
//			Log.d("PushService", "ff = "+serviceList.get(i).service.getClassName());
			if (serviceList.get(i).service.getClassName().equals(this.getPackageName()+className) == true) {
				isRunning = true;
				break;
			}
		}
		return isRunning;
    }
    
	private boolean isBackgroundRunning() {
		String processName = this.getPackageName();

		ActivityManager activityManager = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
		KeyguardManager keyguardManager = (KeyguardManager) getSystemService(KEYGUARD_SERVICE);

		if (activityManager == null)
		{
			Log.d("PushService", "isBackgroundRunning null");
			return false;
		}
		// get running application processes
		List<ActivityManager.RunningAppProcessInfo> processList = activityManager
				.getRunningAppProcesses();
		for (ActivityManager.RunningAppProcessInfo process : processList) {
			Log.d("PushService", "isBackgroundRunning  = " + process.processName);
			if (process.processName.startsWith(processName)) {
				Log.d("PushService", "process.importance = " + process.importance);
				boolean isBackground = process.importance != ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND
						&& process.importance != ActivityManager.RunningAppProcessInfo.IMPORTANCE_VISIBLE;
				boolean isLockedState = keyguardManager
						.inKeyguardRestrictedInputMode();
				if (isBackground || isLockedState)
				{
					Log.d("PushService", "isBackgroundRunning ttt");
					return true;
				}else{
					Log.d("PushService", "isBackgroundRunning fff");
					return false;
				}
					
			}
		}
		Log.d("PushService", "isBackgroundRunning 11");
		return false;
	}
}
