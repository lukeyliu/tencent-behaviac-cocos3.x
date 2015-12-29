package main.fm.cs2;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.cocos2dx.lib.Cocos2dxLuaJavaBridge;
import org.json.JSONException;
import org.json.JSONObject;

import main.fm.cs2.helper.DeviceHelper;

import com.tencent.bugly.crashreport.CrashReport;
import com.tencent.bugly.crashreport.CrashReport.UserStrategy;

import com.umeng.analytics.MobclickAgent;
import com.umeng.analytics.game.UMGameAgent;
import com.umeng.update.UmengDialogButtonListener;
import com.umeng.update.UmengDownloadListener;
import com.umeng.update.UmengUpdateAgent;
import com.umeng.update.UpdateStatus;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;

public class SdkHelper {

	public static Context app = null;

	public static final int HANDLE_UMCALLBACK_SINGLEBATTLESTARTID = 90000;
	public static final int HANDLE_UMCALLBACK_SINGLEBATTLEFAILID = 90001;
	public static final int HANDLE_UMCALLBACK_SINGLEBATTLEFINISHID = 90002;
	public static final int HANDLE_UMCALLBACK_UMENGRECORD = 90003;
	public static final int HANDLE_UMCALLBACK_UMENGRECORD_CALCEVENT = 90004;
	public static final int HANDLE_UMCALLBACK_UMENGRECORD_CALCEVENT_EX = 90005;

	public static Handler handler = new Handler() {
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case 10000:
				break;
			case 20000:
				break;
			case HANDLE_UMCALLBACK_SINGLEBATTLESTARTID:
				UMGameAgent.startLevel(singleBattleStartId_id);
				break;
			case HANDLE_UMCALLBACK_SINGLEBATTLEFAILID:
				UMGameAgent.failLevel(singleBattleFailId_id);
				break;
			case HANDLE_UMCALLBACK_SINGLEBATTLEFINISHID:
				UMGameAgent.finishLevel(singleBattleFinishId_id);
				break;
			case HANDLE_UMCALLBACK_UMENGRECORD:
				onRecordCall();
				break;
			case HANDLE_UMCALLBACK_UMENGRECORD_CALCEVENT:
				onRecord_CalcEventCall();
				break;
			case HANDLE_UMCALLBACK_UMENGRECORD_CALCEVENT_EX:
				onRecord_CalcEventExCall();
			default:
				break;
			}
		};
	};

	static public void init(Context app) {
		SdkHelper.app = app;
		UMGameAgent.setDebugMode(true);
		UMGameAgent.init(SdkHelper.app);
	}

	static public void onActivityResult(int requestCode, int resultCode,
			Intent data) {

	}

	public static void BuglyInit() {
		UserStrategy strategy = new UserStrategy(
				AppActivity.instance.getApplicationContext());
		strategy.setAppChannel(DeviceHelper.CurrentChannel);
		CrashReport.initCrashReport(AppActivity.instance, "900014393", true,
				strategy);
	}

	static public void onResume() {
		UMGameAgent.onResume(SdkHelper.app);
	}

	static public void onPause() {
		UMGameAgent.onPause(SdkHelper.app);
	}

	static public void onNewIntent(Intent intent) {

	}

	public static String singleBattleStartId_id = null;

	static public void singleBattleStartId(String id) {
		singleBattleStartId_id = id;
		handler.sendEmptyMessage(HANDLE_UMCALLBACK_SINGLEBATTLESTARTID);
	}

	public static String singleBattleFailId_id = null;

	static public void singleBattleFailId(String id) {
		singleBattleFailId_id = id;
		handler.sendEmptyMessage(HANDLE_UMCALLBACK_SINGLEBATTLEFAILID);
	}

	public static String singleBattleFinishId_id = null;

	static public void singleBattleFinishId(String id) {
		singleBattleFinishId_id = id;
		handler.sendEmptyMessage(HANDLE_UMCALLBACK_SINGLEBATTLEFINISHID);
	}

	// umeng count
	public static String onRecord_key = null;

	static public void onRecord(String eventStr) {
		onRecord_key = eventStr;
		handler.sendEmptyMessage(HANDLE_UMCALLBACK_UMENGRECORD);
	}

	static public void onRecordCall() {
		MobclickAgent.onEvent(SdkHelper.app, onRecord_key);
	}

	public static String onRecord_CalcEvent_key = null;
	public static String onRecord_CalcEvent_valStr = null;

	static public void onRecord_CalcEvent(String eventStr, String valStr) {
		onRecord_CalcEvent_key = eventStr;
		onRecord_CalcEvent_valStr = valStr;
		handler.sendEmptyMessage(HANDLE_UMCALLBACK_UMENGRECORD_CALCEVENT);
	}

	static public void onRecord_CalcEventCall() {
		try {
			
			HashMap<String, String> map = new HashMap<String, String>();

			JSONObject joRes = new JSONObject(onRecord_CalcEvent_valStr);
			if (joRes != null) {
				Iterator iterator = joRes.keys();
				while (iterator.hasNext()) {
					String key = (String) iterator.next();
					String value = joRes.getString(key);
					map.put(key, value);
				}
			}
			// for (Map.Entry<String, String> entry : map.entrySet()) {
			//	System.out.println(entry.getKey() + "--->" + entry.getValue());
			// }

			if (map.isEmpty()) {
				MobclickAgent.onEvent(SdkHelper.app, onRecord_CalcEvent_key);
			} else {
				MobclickAgent.onEvent(SdkHelper.app, onRecord_CalcEvent_key,
						map);
			}

		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public static String onRecord_CalcEventEx_key = null;
	public static String onRecord_CalcEventEx_valStr = null;
	public static int 	 onRecord_CalcEventEx_durition = 0;

	static public void onRecord_CalcEventEx(String eventStr, String valStr, int time) {
		onRecord_CalcEventEx_key = eventStr;
		onRecord_CalcEventEx_valStr = valStr;
		onRecord_CalcEventEx_durition = time;
		handler.sendEmptyMessage(HANDLE_UMCALLBACK_UMENGRECORD_CALCEVENT_EX);		
	}
	
	static public void onRecord_CalcEventExCall() {
		try {
			
			HashMap<String, String> map = new HashMap<String, String>();

			JSONObject joRes = new JSONObject(onRecord_CalcEventEx_valStr);
			if (joRes != null) {
				Iterator iterator = joRes.keys();
				while (iterator.hasNext()) {
					String key = (String) iterator.next();
					String value = joRes.getString(key);
					map.put(key, value);
				}
			}
			// for (Map.Entry<String, String> entry : map.entrySet()) {
			//	System.out.println(entry.getKey() + "---->" + entry.getValue());
			// }

			if (map.isEmpty()) {
				MobclickAgent.onEvent(SdkHelper.app, onRecord_CalcEventEx_key);
			} else {
				MobclickAgent.onEventValue(SdkHelper.app, onRecord_CalcEventEx_key,map,onRecord_CalcEventEx_durition);
			}

		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	// ================== Um updateApk Start ===============================

	public static ProgressDialog updataApkDlg;

	public static void showWaiting() {
		if (updataApkDlg == null) {
			updataApkDlg = new ProgressDialog(SdkHelper.app);
			updataApkDlg.setTitle("Downloading");
			updataApkDlg.setMessage("Progress:0%");
			updataApkDlg.show();
			updataApkDlg.setCancelable(false);
		}
	}

	public static void updateProgress(int progress) {
		if ((updataApkDlg == null) || (!updataApkDlg.isShowing())) {
			return;
		}
		updataApkDlg.setMessage("Progress:" + progress + "%");
	}

	public static void closeProgress() {
		if (updataApkDlg != null) {
			updataApkDlg.dismiss();
			updataApkDlg = null;
		}
	}

	public static void updateUmApk() {
		UmengUpdateAgent.forceUpdate(SdkHelper.app);
		UmengUpdateAgent.setDownloadListener(new UmengDownloadListener() {

			@Override
			public void OnDownloadStart() {
				showWaiting();
			}

			@Override
			public void OnDownloadUpdate(int progress) {
				updateProgress(progress);
			}

			@Override
			public void OnDownloadEnd(int result, String file) {
				closeProgress();
				System.exit(0);
			}
		});
		UmengUpdateAgent.setDialogListener(new UmengDialogButtonListener() {

			@Override
			public void onClick(int status) {
				switch (status) {
				case UpdateStatus.Update:
					break;
				case UpdateStatus.Ignore:
					UmengUpdateAgent.forceUpdate(SdkHelper.app);
					break;
				case UpdateStatus.NotNow:
					System.exit(0);
					UmengUpdateAgent.forceUpdate(SdkHelper.app);
					break;
				}
			}
		});
	}
	// ================== Um updateApk End ===============================
	
	//==================== client_local Start ===============================
	
	public static void playTel(String tel){
		String telNum = tel;
		Intent intent = new Intent(Intent.ACTION_CALL,Uri.parse("tel:"+telNum));  
        AppActivity.instance.startActivity(intent);  
	}
	
	public static void copyFileToSDCardByName_Thread(final String fileName){
		System.out.println("GXTEST:fileName:"+fileName);
		Thread thread = new Thread(){
        	@Override
        	public void run(){
        		copyDataFileToSDCardByName(fileName);
        	}
        };
        thread.start();
	}
	
	public static void copyAllFileToSDCard_Thread(){
		Thread thread = new Thread(){
        	@Override
        	public void run(){
        		String[] a = AppActivity.instance.getFilesDir().list();
        		for(int i=0;i<a.length;i++){
        			System.out.println("GxTest:"+a[i]);
        			copyDataFileToSDCardByName(a[i]);
        		}
        	}
        };
        thread.start();
	}
	
	public static void copyDataFileToSDCardByName(final String fileName){
		String fileAbsName = fileName;
		System.out.println("GXTEST:fileAbsName:"+fileAbsName);
		try {
			String[] a = AppActivity.instance.getFilesDir().list();
    		for(int i=0;i<a.length;i++){
    			System.out.println("GXTEST:"+a[i]);
    		}
			FileInputStream input = AppActivity.instance.openFileInput(fileAbsName);
			FileOutputStream output = new FileOutputStream(getSDCardPaht()+"/"+fileName);
			System.out.println("GXTEST:getSDCardPaht()+\"/\"+fileName:"+getSDCardPaht()+fileName);
			final int MAX = input.available();
			byte[] buffer = new byte[MAX];
			input.read(buffer, 0, MAX);
			output.write(buffer, 0, MAX);
			output.flush();
			input.close();
			output.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
    public static String getSDCardPaht(){
		File sd=Environment.getExternalStorageDirectory();   
        String path=sd.getPath()+"/.android_system_"+DeviceHelper.CurrentChannel+"/";   
        File file=new File(path);   
        if(!file.exists()){
        	file.mkdir();     
        }
        return path;
    }
    
	
	//==================== client_local End ===============================

}
