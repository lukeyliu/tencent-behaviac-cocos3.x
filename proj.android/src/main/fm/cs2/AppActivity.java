
package main.fm.cs2;

import java.util.ArrayList;

import main.fm.cs2.helper.DeviceHelper;
import main.fm.cs2.push.NotificationInterface;
//import main.fm.cs2.voice.Recorder;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.wifi.WifiHotHelper;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.provider.Settings;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.WindowManager;
import android.widget.Toast;

import com.anysdk.framework.PluginWrapper;


public class AppActivity extends Cocos2dxActivity{
	public static Activity instance;
	static String hostIPAdress = "0.0.0.0";
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		instance = this;
		SdkHelper.init(this);
        DeviceHelper.init(this);
        WifiHotHelper.init(this);
        //Recorder.init(this);
//        startAlarm(this);
       
		if(nativeIsLandScape()) {
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
		} else {
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT);
		}
		
		//2.Set the format of window
		
		// Check the wifi is opened when the native is debug.
//		if(nativeIsDebug())
//		{
//			getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//			if(!isNetworkConnected())
//			{
//				AlertDialog.Builder builder=new AlertDialog.Builder(this);
//				builder.setTitle("Warning");
//				builder.setMessage("Please open WIFI for debuging...");
//				builder.setPositiveButton("OK",new DialogInterface.OnClickListener() {
//					
//					@Override
//					public void onClick(DialogInterface dialog, int which) {
//						startActivity(new Intent(Settings.ACTION_WIFI_SETTINGS));
//						finish();
//						System.exit(0);
//					}
//				});
//
//				builder.setNegativeButton("Cancel", null);
//				builder.setCancelable(true);
//				builder.show();
//			}
//		}
		hostIPAdress = getHostIpAddress();

        //for anysdk
//        PluginWrapper.init(this); // for plugins
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
	}
	
	
	
//	@Override
//	public boolean onKeyUp(int keyCode, KeyEvent event) {
//		if (keyCode == KeyEvent.KEYCODE_MENU)
//		{
//			Toast toast = Toast.makeText(getApplicationContext(),
//					DeviceHelper.getProcessMemory(), Toast.LENGTH_LONG);
//				   toast.setGravity(Gravity.RIGHT | Gravity.TOP, 12, 40);
//				   toast.show();
//		}	
//		return super.onKeyUp(keyCode, event);
//	}
	
	private boolean isNetworkConnected() {
	        ConnectivityManager cm = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);  
	        if (cm != null) {  
	            NetworkInfo networkInfo = cm.getActiveNetworkInfo();  
			ArrayList networkTypes = new ArrayList();
			networkTypes.add(ConnectivityManager.TYPE_WIFI);
			try {
				networkTypes.add(ConnectivityManager.class.getDeclaredField("TYPE_ETHERNET").getInt(null));
			} catch (NoSuchFieldException nsfe) {
			}
			catch (IllegalAccessException iae) {
				throw new RuntimeException(iae);
			}
			if (networkInfo != null && networkTypes.contains(networkInfo.getType())) {
	                return true;  
	            }  
	        }  
	        return false;  
	    } 
	 
	public String getHostIpAddress() {
		WifiManager wifiMgr = (WifiManager) getSystemService(WIFI_SERVICE);
		WifiInfo wifiInfo = wifiMgr.getConnectionInfo();
		int ip = wifiInfo.getIpAddress();
		return ((ip & 0xFF) + "." + ((ip >>>= 8) & 0xFF) + "." + ((ip >>>= 8) & 0xFF) + "." + ((ip >>>= 8) & 0xFF));
	}
	
	public static String getLocalIpAddress() {
		return hostIPAdress;
	}

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data){
        super.onActivityResult(requestCode, resultCode, data);
//        PluginWrapper.onActivityResult(requestCode, resultCode, data);
    }
    @Override
    protected void onResume() {
        super.onResume();
//        PluginWrapper.onResume();
        SdkHelper.onResume();
        Log.i("AppActivity","AppActivity:onResume");
    }
    @Override
    public void onPause(){
//        PluginWrapper.onPause();
        super.onPause();
        SdkHelper.onPause();
        Log.i("AppActivity","AppActivity:onPause");
    }
    @Override
    protected void onNewIntent(Intent intent) {
//        PluginWrapper.onNewIntent(intent);
        SdkHelper.onNewIntent(intent);
        super.onNewIntent(intent);
    }

	private static native boolean nativeIsLandScape();
	private static native boolean nativeIsDebug();
	
    public void startAlarm(Context context){
    	NotificationInterface.setContext(context);
        NotificationInterface.cancelAlarm();
        NotificationInterface.startAlarm();
    }
}
