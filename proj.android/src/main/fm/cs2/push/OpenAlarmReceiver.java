package main.fm.cs2.push;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class OpenAlarmReceiver extends BroadcastReceiver {

	@Override
	public void onReceive(Context context, Intent intent) {
		Log.d("PushService", "tick   ttttttttttttttt");
//		Toast.makeText(context, "", Toast.LENGTH_SHORT).show();
        NotificationInterface.setContext(context);
        NotificationInterface.cancelAlarm();
        NotificationInterface.startAlarm();
	}

}
