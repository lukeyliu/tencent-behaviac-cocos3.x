package main.fm.cs2.helper;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.List;

import main.fm.cs2.AppActivity;
import main.fm.cs2.SdkHelper;
import main.fm.cs2.channel.ChannelID;

import org.cocos2dx.lib.Cocos2dxActivity;

import android.app.ActivityManager;
import android.app.ActivityManager.MemoryInfo;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.os.StatFs;
import android.os.Vibrator;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.text.format.Formatter;
import android.util.Log;
import android.os.Debug;
public class DeviceHelper {

	public static String CurrentChannel = ChannelID.Channel_ZJXL;
	public static String CurrentVersion = "UNKNOWN";
	static Cocos2dxActivity mContext = null;
	static TelephonyManager mTm = null;
	static ActivityManager mam = null;
    public static final String NETWORK_UNKNOWN = "UNKNOWN";  
    public static final String NETWORK_2_G = "NETWORK_2_G";  
    public static final String NETWORK_3_G = "NETWORK_3_G";  
    public static final String NETWORK_4_G = "NETWORK_4_G";  

	public static void init(Cocos2dxActivity context) 
	{
		mContext = context;
		mam = (ActivityManager) mContext.getSystemService(Context.ACTIVITY_SERVICE);
		mTm = (TelephonyManager) (mContext.getSystemService(Context.TELEPHONY_SERVICE)); 
	}
	
	public static String getNetworkOperatorName()
	{
		String result = mTm.getNetworkOperatorName();
		if(TextUtils.isEmpty(result)){
			return "UNKNOWN";
		}
		return result;
	}
	
	public static String getSimOperatorName()
	{
		return mTm.getSimOperatorName();
	}
	
	public static String getDeviceName()
	{
		return Build.MODEL;
	}
	
	public static String getChannelID()
	{
		return DeviceHelper.CurrentChannel;
	}

	public static String getAvailMemory() {
        MemoryInfo memoryInfo = new MemoryInfo() ;  
        mam.getMemoryInfo(memoryInfo) ;  
        long memSize = memoryInfo.availMem ;            
        String availMemStr = Formatter.formatFileSize(mContext, memSize);          
        return availMemStr ;  
	}
	
	public static String getProcessMemory()
	{
        MemoryInfo memoryInfo = new MemoryInfo() ;   
        mam.getMemoryInfo(memoryInfo) ;  
        long memSize = memoryInfo.availMem ;  
          
    	int pid[] = new int[1];
    	pid[0] = android.os.Process.myPid();
    	android.os.Debug.MemoryInfo [] infos = mam.getProcessMemoryInfo(pid) ;  
    	
    	android.os.Debug.MemoryInfo info = infos[0];
    		
        String availMemStr = "used Mem:" + (int)(info.getTotalPrivateDirty())/1024.0f + "MB  TotalPss:" + (int)(info.getTotalPss())/1024.0f + "MB  TotalSharedDirty" +
        		(int)(info.getTotalSharedDirty()) / 1024.0f +"MB Sysfree:" +  Formatter.formatFileSize(mContext,memSize); 
          
        return availMemStr ;  
	}
	
	public static void setAndroidVersion(final String version){
		DeviceHelper.CurrentVersion = version;
		SdkHelper.BuglyInit();
	}

	public static String getNetWorkTpye()
	{
		ConnectivityManager manager = (ConnectivityManager) (mContext.getSystemService(Context.CONNECTIVITY_SERVICE));
		NetworkInfo info = manager.getActiveNetworkInfo();
		if (info == null)
		{
			return "NET NOT OPEN";
		}
		else
		{
			int nType = info.getType(); 
			if (nType != ConnectivityManager.TYPE_WIFI){
				return getNetworkName();
			}		
		}
		return info.getTypeName();
	}
	
	public static boolean isFastMobileNetwork()
	{
		ConnectivityManager manager = (ConnectivityManager) (mContext.getSystemService(Context.CONNECTIVITY_SERVICE));
		NetworkInfo info = manager.getActiveNetworkInfo();
		if (info == null)
		{
			return false;
		}
		else
		{			
			int nType = info.getType(); 
			if (nType == ConnectivityManager.TYPE_WIFI){
				return true;
			}
			else{
				@SuppressWarnings("deprecation")
				String proxyHost = android.net.Proxy.getDefaultHost();  
				  
				if (TextUtils.isEmpty(proxyHost)) {
					if (fastMobileNetwork()){
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
		}
	}
	
	public static void vibrate(int time)
	{
		Log.i("vibrate", "time:"+time);
		Vibrator vib = (Vibrator) mContext.getSystemService(Service.VIBRATOR_SERVICE); 
		if (vib != null){
			vib.vibrate(time);  	
		} 
		else
		{
			Log.i("vibrate", "Vibrator is null");
		}
	}
	
	private static boolean fastMobileNetwork() {
		String netStr = getNetworkName();
		if (netStr.equals(NETWORK_UNKNOWN) || netStr.equals(NETWORK_2_G)) {
			return false;
		}
		return true;
	}
	
	public static boolean isHaveInternet() {
		try {
			ConnectivityManager manger = (ConnectivityManager) AppActivity.instance
					.getSystemService(Context.CONNECTIVITY_SERVICE);
			NetworkInfo info = manger.getActiveNetworkInfo();
			return (info != null && info.isConnected());
		} catch (Exception e) {
			return false;
		}
	}
	
	private static String getNetworkName() {
		switch (mTm.getNetworkType()) {  
		case TelephonyManager.NETWORK_TYPE_GPRS:  
        case TelephonyManager.NETWORK_TYPE_EDGE:  
        case TelephonyManager.NETWORK_TYPE_CDMA:  
        case TelephonyManager.NETWORK_TYPE_1xRTT:  
        case TelephonyManager.NETWORK_TYPE_IDEN:  
            return NETWORK_2_G;  
        case TelephonyManager.NETWORK_TYPE_UMTS:  
        case TelephonyManager.NETWORK_TYPE_EVDO_0:  
        case TelephonyManager.NETWORK_TYPE_EVDO_A:  
        case TelephonyManager.NETWORK_TYPE_HSDPA:  
        case TelephonyManager.NETWORK_TYPE_HSUPA:  
        case TelephonyManager.NETWORK_TYPE_HSPA:  
        case TelephonyManager.NETWORK_TYPE_EVDO_B:  
        case TelephonyManager.NETWORK_TYPE_EHRPD:  
        case TelephonyManager.NETWORK_TYPE_HSPAP:  
            return NETWORK_3_G;  
        case TelephonyManager.NETWORK_TYPE_LTE:  
            return NETWORK_4_G;  
        default:  
            return NETWORK_UNKNOWN;  
		    }  
		}
	
	/**GiveMeSomeScore*/
	public static void giveMeEvaluation(){
		Context context=AppActivity.instance;
	    String packageName = "";
	    try {   
	        // ---get the package info---   
	        PackageManager pm = context.getPackageManager();   
	        PackageInfo pi = pm.getPackageInfo(context.getPackageName(), 0);   
	        if(pi != null){  
	            ApplicationInfo appInfo = pi.applicationInfo;  
	            packageName = appInfo.packageName;  //
	        }
	    } catch (Exception e) {   
	        Log.e("VersionInfo", "Exception", e);   
	    }
	    Uri uri = Uri.parse("market://details?id="+packageName);
		Intent intent = new Intent(Intent.ACTION_VIEW, uri);
		AppActivity.instance.startActivity(intent);
	}
	
	public static String getRomTotalSize() {  
        File path = Environment.getDataDirectory();  
        StatFs stat = new StatFs(path.getPath());  
        @SuppressWarnings("deprecation")
		long blockSize = stat.getBlockSize();  
        @SuppressWarnings("deprecation")
		long totalBlocks = stat.getBlockCount();  
        return Formatter.formatFileSize(mContext, blockSize * totalBlocks);  
    }  
   
	public static String getRomAvailableSize() {  
        File path = Environment.getDataDirectory();  
        StatFs stat = new StatFs(path.getPath());  
        @SuppressWarnings("deprecation")
		long blockSize = stat.getBlockSize();  
        @SuppressWarnings("deprecation")
		long availableBlocks = stat.getAvailableBlocks();  
        return Formatter.formatFileSize(mContext, blockSize * availableBlocks);  
    }  
	
    public static String getTotalMemorySize() {
        String dir = "/proc/meminfo";
        try {
            FileReader fr = new FileReader(dir);
            BufferedReader br = new BufferedReader(fr, 2048);
            String memoryLine = br.readLine();
            String subMemoryLine = memoryLine.substring(memoryLine.indexOf("MemTotal:"));
            br.close();
            long temp = (Integer.parseInt(subMemoryLine.replaceAll("\\D+", "")) * 1024l);
            return Formatter.formatFileSize(mContext, temp);  
        } catch (IOException e) {
            e.printStackTrace();
        }
        return "0";
    }
    
    public static String getAvailableMemory() {
        ActivityManager am = (ActivityManager) mContext.getSystemService(Context.ACTIVITY_SERVICE);
        ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
        am.getMemoryInfo(memoryInfo);
        return Formatter.formatFileSize(mContext, memoryInfo.availMem);
    }
    
    public static final String ShareName = "simepleGx.png";
    public static String SharePath = null;
    public static String getFileName(){
    	String filePath = null;
		filePath = Environment.getExternalStorageDirectory().toString()+"/";
		filePath += ShareName;
		SharePath = filePath;
		File file = new File(SharePath);
		if(file.exists()){
			file.delete();
		}
		return filePath;
    }

	public static void share(){
		File file = new File(SharePath);
		if(file.exists()&&file.isFile()){
			Intent intent=new Intent(Intent.ACTION_SEND);   
			intent.setType("image/*");
			intent.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(file)); 
			intent.putExtra(Intent.EXTRA_SUBJECT, "Share");
			intent.putExtra(Intent.EXTRA_TEXT, AppActivity.instance.getTitle());    
			intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			AppActivity.instance.startActivity(Intent.createChooser(intent, "Share To")); 
		}
	}
	
	public static String getDeviceBrand(){
		return Build.BRAND;
	}
	
	public static String getDeviceParameter(){
		return Build.DEVICE;
	}
	
	public static String getDeviceIMEI(){
		String result = mTm.getDeviceId();
		if(TextUtils.isEmpty(result)){
			return "UNKNOWN";
		}
		return result;
	}
	
	public static String getAndroidVersion(){
		String result = null;
		try {
			PackageManager manager = AppActivity.instance.getPackageManager();
			PackageInfo info;
			info = manager.getPackageInfo(AppActivity.instance.getPackageName(), 0);
			result = info.versionName;
			if(TextUtils.isEmpty(result)){
				return "UNKNOWN";
			}
		} catch (NameNotFoundException e) {
			return "UNKNOWN";
		}
		return result;
	}
}
