
/**
* File Name: "ListAdapter.java"
* Description:
*     - 연속된 데이터를 처리하기 위해 Adapter가 그 데이터를 받아 리스트에 뿌려주고 이벤트 관리를 함
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
/**
*  Function Name: class ListAdapter(BaseAdapter)
*  Processing in function (in pseudo code style):
*    1) ListAdapter(), addItem(), getCount(), getItem(), getItemId(), remove()
*    2) getView() : 리스트를 꾸밈, 뷰를 생성하고 리턴하여 화면에 나타냄
*/


package com.example.servicetest;

import java.util.ArrayList;
import java.util.Date;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.TextView;

public class ListAdapter extends BaseAdapter
{
	private ArrayList< Date >list_alarm;
	private Context mContext;
	private LayoutInflater mInflater;
	private AppData app;
	
	public ListAdapter( Context context )
	{
		mContext = context;
		app = ( AppData )mContext.getApplicationContext();
		list_alarm = app.getAlarmList();
		mInflater = ( LayoutInflater )mContext.getSystemService( Context.LAYOUT_INFLATER_SERVICE );
	}
	
	public boolean addItem( Date time )
	{
		return app.addAlarmTime( time );
	}
	
	@Override
	public int getCount() {//목록의 카운트 리턴
		return list_alarm.size();
	}

	@Override
	public Object getItem(int arg0) {//아이템 데이터 리턴
		return list_alarm.get( arg0 );
	}

	@Override
	public long getItemId(int arg0) {//아이템 뷰의 ID리턴
		return arg0;
	}
	
	private void remove( int arg0 )//화면 갱신
	{
		list_alarm.remove( arg0 );
		this.notifyDataSetChanged();
	}

	@Override
	public View getView( final int arg0, View arg1, ViewGroup arg2 ) {//목록의 화면처리
		View view = null;
		if( view == null ) {
			view = mInflater.inflate( R.layout.row_alarm_list, null );
		} //  end if
		final Date info = ( Date )getItem( arg0 );
		if( info != null ) {
			TextView tv = ( TextView )view.findViewById( R.id.row_time ); //텍스트
			Button btn = ( Button )view.findViewById( R.id.row_delete ); //버튼
			
			tv.setText( app.getDateFormat().format( info ) );
		    btn.setOnClickListener( new OnClickListener() {
				@Override
				public void onClick( View v )
				{
					remove( arg0 );
				}
		    } );
		} // end if
		return view; 
	}
	
}