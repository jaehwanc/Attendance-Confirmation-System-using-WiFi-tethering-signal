/**
* File Name: "AppData.java"
* Description:
*     - 데이터 관리 및 포멧 
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

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;

import android.annotation.SuppressLint;
import android.app.Application;
import android.util.Log;

@SuppressLint("SimpleDateFormat")
public class AppData extends Application
{
	private final SimpleDateFormat sdf = new SimpleDateFormat( "EE HH:mm" );
	                                                    
	private ArrayList< Date >alarmTimeList = new ArrayList< Date >();
	
	public SimpleDateFormat getDateFormat()
	{
		return sdf;
	}
	
	public boolean addAlarmTime( Date time ) 
	{
		for( Date _time : alarmTimeList ) {
			if( sdf.format( _time ).equals( sdf.format( time ) ) ) {
				Log.d("1234", "시간 중복 확인하세요." );
				return false;
			} // end if
		} // end for
		alarmTimeList.add( time );
		return true;
	}
	
	public final ArrayList< Date >getAlarmList()
	{
		return alarmTimeList;
	}
}