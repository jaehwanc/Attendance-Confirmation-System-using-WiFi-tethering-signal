/**
* File Name: "AlarmActivity.java"
* Description:
*     - 메인엑티비티로 시간표 데이터를 저장하고 백그라운드 프로세스 작업(알람 메세지) 설정 
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
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.text.ParseException;
import java.util.Date;

import android.annotation.SuppressLint;
import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

@SuppressLint("SdCardPath")
//메인 액티비티 클래스
public class AlarmActivity extends ListActivity
{
	private static final String fileName = "db.txt";
	private AppData app;
	private EditText et_ee; // 요일
	private EditText et_hh; // 시간
	private EditText et_mm; // 분
	private TextView tv_add;
	private TextView tv_start;
	private ListAdapter mAdapter;  // 리스트
	

	/**
	*  Function Name: onCreate(app 실행 시 수행되는 함수, C의 main문과 동일)
	*  Input arguments (condition):
	*    1) Bundle - Activity간의 데이터를 주고 받을때 사용
	*  Processing in function (in pseudo code style):
	*    1) initialization: 요일, 시간, 분
	*    2) loadDB() : DB에 저장된 데이터를 불러와 display 
	*    3) ListAdapter() : 데이터를 리스트에 뿌려주고 이벤트괄리를 위한 클래스 
	*    4) getListView() : listView 입출력
	*/

	
	@Override
	public void onCreate( Bundle savedInstanceState )
	{
		super.onCreate( savedInstanceState );
		setContentView( R.layout.activity_alarm );
		//어댑터객체생성(생성시에 데이터, row에 대한 화면지정
		
		app = ( AppData )this.getApplicationContext(); //어플리케이션 종료 후에도 활동하는 글로벌 context
		et_ee = ( EditText )findViewById( R.id.et_ee );
		et_hh = ( EditText )findViewById( R.id.et_hh );
		et_mm = ( EditText )findViewById( R.id.et_mm );
		tv_add = ( TextView )findViewById( R.id.text_add );
		tv_start = ( TextView )findViewById( R.id.text_start );

		loadDB();
		
		mAdapter = new ListAdapter( this );
		setListAdapter( mAdapter );
		getListView().setItemsCanFocus( true );
		tv_add.setText( "시간표를 입력하세요." );
		tv_start.setText( "서비스 시작 버튼을 눌러서 시작하세요." );
	}
	
	@Override
	public void onResume() //서비스재시작
	{
		super.onResume();
		Intent intent = new Intent( AlarmActivity.this, AlarmService.class );
		stopService( intent );
	}
	
	@Override
	public void onStop() //서비스종료
	{
		Log.d( "1234", "활성화 오류" );
		super.onStop();
	}
	
	@Override
	public void onDestroy() //서비스해제
	{
		Log.d( "1234", "활성화 실패" );
		super.onDestroy();
	}
	
	/**
	*  Function Name: loadDB(DB 불러옴)
	*  Processing in function (in pseudo code style):
	*    1) initialization: 버퍼
	*    2) BufferedReader() : 문자 입력 스트링으로 문자를 읽어들임 
	*    3) 예외처리
	*/
	private void loadDB() //DB불러옴
	{
		BufferedReader br = null;
		String line = null;
		try {
			br = new BufferedReader( new FileReader( "/sdcard/" + fileName ) ); //SDcard에 저장된 데이터 불러옴
			
			try {
				while( ( line = br.readLine() ) != null ) {
					try {
						app.addAlarmTime( app.getDateFormat().parse( line ) );
					} catch ( ParseException pe ) {
						Log.d( "1234", "날짜 포멧 오류" + pe );
					}
				}
			} catch ( IOException ioe ) {
				Log.d( "1234", "에러 발생" + ioe );
			}
			
		} catch( FileNotFoundException fnfe ) {
			Log.d( "1234", "파일 찾기 오류" + fnfe );
			File file = new File( "/sdcard/", fileName );
			try {
				file.createNewFile();
			} catch ( IOException ioe ) {
				Log.d( "1234", "에러 발생" + ioe );
			}
			try {
				br = new BufferedReader( new FileReader( "/sdcard/" + fileName )  );
				try {
					while( ( line = br.readLine() ) != null ) {
						try {
							app.addAlarmTime( app.getDateFormat().parse( line ) );
						} catch ( ParseException pe ) {
							Log.d( "1234", "날짜 포멧 오류" + pe );
						}
					}
				} catch ( IOException ioe ) {
					Log.d( "1234", "에러 발생" + ioe );
				}
				
			} catch (FileNotFoundException  fnfe2) {
				Log.d( "1234", "파일 찾기 오류" + fnfe2 );
			}	
		}
	}
	

	/**
	*  Function Name: saveDB(DB 저장)
	*  Processing in function (in pseudo code style):
	*    1) initialization: 버퍼
	*    2) File() : 파일생성
	*    3) BufferedWriter() : 문자 입력 스트링으로 문자를 씀 
	*/
	private void saveDB() //DB 저장
	{
		
		BufferedWriter bw = null;
		try {
			File file = new File( "/sdcard/", fileName );
			try {
				file.createNewFile();
			} catch ( IOException ioe ) {
				Log.d( "1234", "에러 발생" + ioe );
			}
			bw = new BufferedWriter( new FileWriter( file ) );
			for( Date date : app.getAlarmList() ) {
				bw.write( app.getDateFormat().format( date ) + "\r\n" );
			} // end for
			bw.close();
		} catch ( IOException ioe ) {
			Log.d( "1234", "에러 발생" + ioe );
		}
	}
	
		
	public void onClickAdd( View view ) { 
		String ee = et_ee.getText().toString();
		String hh = et_hh.getText().toString();
		String mm = et_mm.getText().toString();
		
		String alarmTime = ee + ' ' + hh + ':' + mm; //display
		try {
			if( mAdapter.addItem( app.getDateFormat().parse( alarmTime ) ) ) {
				Toast.makeText( this, alarmTime + "추가됨", Toast.LENGTH_SHORT ).show(); //추가내용에 대한toast 문구
			} else {
				Toast.makeText( this, "시간 중복 확인하세요.", Toast.LENGTH_SHORT ).show(); 
			} // end if then else
			
		} catch (ParseException pe) {
			Toast.makeText( this, "날짜 포멧 에러", Toast.LENGTH_SHORT ).show();
		}
		setListAdapter( mAdapter );
		et_ee.setText( "" );
		et_hh.setText( "" );
		et_mm.setText( "" );
		saveDB();
	}
	
	public void onClickStart( View view ) {//서비스 시작
		
		Toast.makeText( this, "서비스 시작", Toast.LENGTH_SHORT ).show();
		Intent intent = new Intent( AlarmActivity.this, AlarmService.class );
		startService( intent );
		saveDB();
		finish();
	}
}