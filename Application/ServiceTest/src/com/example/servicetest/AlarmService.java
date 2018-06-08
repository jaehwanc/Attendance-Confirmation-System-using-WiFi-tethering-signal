/**
* File Name: "AlarmService.java"
* Description:
*     - 서비스 제공하는 부분으로 AlramThread를 이용하여 시간 데이터 확인 및 비교하여 output 
*
* Programmed by Jae-Hwan Choi, So-Yeon Lee (October 4, 2013),
* Last updated: Version 2.0, December 4, 2013 (by Jae-Hwan Choi, So-Yeon Lee).
*
* ==================================================================================
*  Version Control (Explain updates in detail)
* ==================================================================================
*  Name                     YYYY/MM/DD    Version  Remarks
*  So-Yeon Lee              2013/10/15    v1.0      Display (GUI)
*  Jae-Hwan Choi            2013/10/20    v1.1      AlarmThread
*  So-Yeon Lee              2013/11/14    v1.2      Store in SD card   
*  Jae-Hwan Choi            2013/11/30    v1.1      Background process
*  Jae-Hwan Choi(with Lee)  2013/12/04    v2.0      Major change in the program structure
* ==================================================================================
*/
package com.example.servicetest;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import android.annotation.SuppressLint;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import android.widget.Toast;

@SuppressLint({ "SdCardPath", "SimpleDateFormat", "HandlerLeak" })

public class AlarmService extends Service
{
	private static final String fileName = "db.txt";
	private static SimpleDateFormat sdf;
	private AlarmThread alarmThread;
	private ArrayList< Date >list;
	private Handler mHandler;
	
	@Override
	public void onCreate()
	{
		super.onCreate();
		Log.d( "1234", "서비스 생성" );
		sdf = new SimpleDateFormat( "EE HH:mm" );	
	}
	
	@Override
	public IBinder onBind( Intent arg0 ) 
	{
		return null;
	}
	
	// 서비스가 시작될때 db.txt에서 현재 알람 리스트를 불러오고 알람 스레드를 시작하게됨

	/**
	*  Function Name: onStartCommand(app 실행 시 수행되는 함수, C의 main문과 동일)
	*  Input arguments (condition):
	*    1) Intent
	*    2) flags
	*    3) startId
	*  Processing in function (in pseudo code style):
	*    1) initialization: 버퍼
	*    2) Handler() : 시간비교를 위해 메인스레드와 서브스레드 간 통신하는 메세지 전달 
	*/

	@Override
	public int onStartCommand( Intent intent, int falgs, int startId )
	{
		list = new ArrayList< Date >();
		BufferedReader br = null;
		String line = null;
		try {
			br = new BufferedReader( new FileReader( "/sdcard/" + fileName ) );
			
			try {
				while( ( line = br.readLine() ) != null ) {
					try {
						list.add( sdf.parse( line ) );
					} catch ( ParseException pe ) {
						Log.d( "1234", "날짜포멧 오류" + pe );
					}
				}
			} catch ( IOException ioe ) {
				Log.d( "1234", "오류발생" + ioe );
			}
		} catch( FileNotFoundException fnfe ) {
			Log.d( "1234", "파일 찾기 오류" + fnfe );
		}
		
		mHandler = new Handler() {
			@Override
			public void handleMessage( Message msg )
			{
				Toast.makeText( AlarmService.this, ( String )msg.obj, Toast.LENGTH_SHORT ).show();
				showNotify();
			}
		};
		
		alarmThread = new AlarmThread( mHandler );//AlarmThread 호출
		alarmThread.start();
		return START_STICKY;
	}
	
	// 엑티비티가 시작될때 서비스를 죽음 그러면 현재 알람 쓰레드는 졸료가 됨
	@Override
	public void onDestroy()
	{
		Toast.makeText( getApplicationContext(), "서비스가 종료되었습니다.", Toast.LENGTH_SHORT ).show();
		Log.d( "1234", "service 소멸" );
		alarmThread.cancle();
		super.onDestroy();
	}
	
	// 상단 바에 메세지 뜸
	private void showNotify()
	{
		int _ID = 0xffff0001;
		NotificationManager mn = ( NotificationManager )getSystemService( Context.NOTIFICATION_SERVICE );
			
		Intent intent = new Intent( android.provider.Settings.ACTION_WIRELESS_SETTINGS );
		PendingIntent pIntent = PendingIntent.getActivity( getApplicationContext(), 0, intent, 0 );
		
		Notification notification = new NotificationCompat.Builder( getApplicationContext() )
		.setVibrate( new long[] { 200, 200, 600, 600, 600, 200, 200, 600, 600, 200, 200, 200 } )
		.setContentTitle( "출석체크" )
		.setContentText( "테더링을 켜주세요." )
		.setSmallIcon( R.drawable.ic_cute )
		.setTicker( "출석체크알람" )
		.setAutoCancel( true )
		.setContentIntent( pIntent )
		.build();
		
		mn.notify( _ID, notification );
	} // end showNotify()
	
	/**
	*  Function Name: class AlarmThread(현재 시간과 비교하여 서비스 시작을 알려줌)
	*  Processing in function (in pseudo code style):
	*    1) initialization: 요일, 시간, 분
	*    2) run() : 1분마다 확인하고 현재 시간과 비교하여 5분 안이면 메세지 발송 
	*    3) equal() : 비교 함수로 요일, 시간, 분을 순차적으로 비교함
	*/

	// 알람 쓰레드
	public class AlarmThread extends Thread
	{
		private final SimpleDateFormat sdf_ee;
		private final SimpleDateFormat sdf_hh;
		private final SimpleDateFormat sdf_mm;
		private Handler mmHandler;
		private boolean isRun;
		
		public AlarmThread( Handler handler )
		{
			sdf_ee = new SimpleDateFormat( "EE" );
			sdf_hh = new SimpleDateFormat( "HH" );
			sdf_mm = new SimpleDateFormat( "mm" );
			isRun = true;
			mmHandler = handler;
		} // end constructor
		
		@Override
		public void run() 
		{
			
			while( isRun ) {
				Date currentTime = new Date();
				
				for( Date arlamTime : list ) {
					Log.d( "1234", sdf.format(arlamTime) + " vs "  + sdf.format( currentTime ) );
					if( equal( currentTime, arlamTime ) ) { // +-5분  오차 이내의 시간과 비교
						Message msg  = new Message();
						msg.what = 1;
						msg.obj = "테더링을 켜주세요.";
						mmHandler.sendMessage( msg );
						//Log.d( "1234", ( String )msg.obj + "------------------" );
					} // end if
				} // end for
				try {
					Thread.sleep( 60 * 1000 ); // 1분 마다 확인 
				} catch ( InterruptedException ie ) {
				}
			} // end while
		} // end run()
		
		synchronized public void cancle()
		{
			isRun = false; // 쓰레드가 종료되게 설정
		} // end cancel()
		
		private boolean equal( Date a, Date b )
		{
			if( !sdf_ee.format( a ).equals( sdf_ee.format( b ) ) ) {
				return false;                // 요일 다르면 다른 시간
			} else if( !sdf_hh.format( a ).equals( sdf_hh.format( b ) ) ) {
				return false;                // 시간 다르면 다른 시간
			} else {
				int aa = Integer.parseInt( sdf_mm.format( a ) );
				int bb = Integer.parseInt( sdf_mm.format( b ) );
				if( Math.abs( aa - bb ) > 5 ) // 5분이상 차이나면 다른 시간
					return false;
				else                          // 5분 이내의 오차에는 같은 시간
					return true;
			} // end if then else
		} // end equal()
	} // end class Alarm Thread
} // end Alarm Service