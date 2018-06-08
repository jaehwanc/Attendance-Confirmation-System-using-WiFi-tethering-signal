/**
* File Name: "wlanscan.cpp"
* Description:
*     - 현재 방안에 존재하는 AP신호를 감지하여 출석 판단하는 프로그램
*
* Programmed by Jae-Hwan Choi, So-Yeon Lee (September 15, 2013),
* Last updated: Version 2.0, Novemver 1, 2013 (by Jae-Hwan Choi, So-Yeon Lee).
*
* ==================================================================================
*  Version Control (Explain updates in detail)
* ==================================================================================
*  Name                        YYYY/MM/DD   Version   Remarks
*  Jae-Hwan Choi               2013/09/15    v1.0     AP signal scan
*  So-Yeon Lee                 2013/10/01    v1.1     signal data extraction through Threshold
*  Jae-Hwan Choi               2013/10/20    v1.2     Student information store
*  So-Yoen Lee				   2013/11/10    v1.3     Attendence Algorithm
*  Jae-Hwan Choi(with Lee)     2013/11/20    v2.0     Major change in the program structure
* ==================================================================================
*/



#pragma comment(lib, "wlanapi.lib")

#include <stdio.h>
#include <windows.h>
#include <wlanapi.h>
#include <IOSTREAM>
#include <string.h>
using namespace std;

VOID WlanNotification(WLAN_NOTIFICATION_DATA *wlanNotifData,VOID *p)
{
	if(wlanNotifData->NotificationCode == wlan_notification_acm_scan_complete)
	{
		//bWait = false;
	}
	else if(wlanNotifData->NotificationCode == wlan_notification_acm_scan_fail)
	{
		printf("Scanning failed with error: %x\n", wlanNotifData->pData);
		//bWait = false;
	}
}

bool IsVistaOrHigher()
{
	OSVERSIONINFO osVersion; ZeroMemory(&osVersion, sizeof(OSVERSIONINFO));
	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if(!GetVersionEx(&osVersion))
		return false;

	if(osVersion.dwMajorVersion >= 6)
		return true;
	return false;
}


int q[4][10]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int n,m=0;
char student[7]="0";
char* student1="20913110";
char* student2="20921656";
char* student3="20912345";
char* student4="20954321";
int a=0;
int b=0;
int sum[4]={0,0,0,0};

/**
*  Function Name:main
*  Input arguments (condition):
*    1) int argc
*    2) char *argv[]
*  Processing in function (in pseudo code style):
*    1) initialization:dwError, dwSupperedVersion
*    2) struct WLAN_INTERFACE_INFO_LIST : AP 존재 유무 확인 구조체
*    3) struct WLAN_AVAILABLE_NETWORK_LIST : 존재하는 AP에 대한 정보(SSID, Signal Quality)
*    4) 정보에 따른 display 및 threshold에 맞추어 출석체크 알고리즘 구현
*/

int main(int argc, char *argv[])
{
	HANDLE hWlan = NULL;

	DWORD dwError = 0;
	DWORD dwSupportedVersion = 0;
	DWORD dwClientVersion = (IsVistaOrHigher() ? 2 : 1);

	GUID guidInterface; ZeroMemory(&guidInterface, sizeof(GUID));

	WLAN_AVAILABLE_NETWORK_LIST *wlanNetworkList = (WLAN_AVAILABLE_NETWORK_LIST*)WlanAllocateMemory(sizeof(WLAN_AVAILABLE_NETWORK_LIST));
	WLAN_INTERFACE_INFO_LIST *wlanInterfaceList = (WLAN_INTERFACE_INFO_LIST*)WlanAllocateMemory(sizeof(WLAN_INTERFACE_INFO_LIST));



	try
	{	
		while(1)
		{
			n=0;
			int list[4]={0,0,0,0};
			printf("%d번째 check\n",m+1);
			Sleep(1000);
			while(1)
			{
				ZeroMemory(wlanInterfaceList, sizeof(WLAN_INTERFACE_INFO_LIST));
				ZeroMemory(wlanNetworkList, sizeof(WLAN_AVAILABLE_NETWORK_LIST));
				n+=1;
				dwError = WlanOpenHandle(dwClientVersion, NULL, &dwSupportedVersion, &hWlan);

				//	if(dwError = WlanOpenHandle(dwClientVersion, NULL, &dwSupportedVersion, &hWlan) != ERROR_SUCCESS)
				//		throw("[x] Unable access wireless interface");

				if(dwError = WlanEnumInterfaces(hWlan, NULL, &wlanInterfaceList) != ERROR_SUCCESS)
					throw("[x] Unable to enum wireless interfaces");

				//	wprintf(L"[!] Found adapter %s\n", wlanInterfaceList->InterfaceInfo[0].strInterfaceDescription);

				if(dwError = wlanInterfaceList->InterfaceInfo[0].isState != wlan_interface_state_not_ready)
				{
					if(wlanInterfaceList->dwNumberOfItems > 1)
					{
						// TODO: Add processing for multiple wireless cards here
						printf("[!] Detected multiple wireless adapters, using default\n");
						guidInterface = wlanInterfaceList->InterfaceInfo[0].InterfaceGuid;
					}
					else
					{
						guidInterface = wlanInterfaceList->InterfaceInfo[0].InterfaceGuid;
					}
				}
				else
					throw("[x] Default wireless adapter disabled");

				DWORD dwPrevNotif = 0;

				// Scan takes awhile so we need to register a callback
				if(dwError = WlanRegisterNotification(hWlan, WLAN_NOTIFICATION_SOURCE_ACM, TRUE,
					(WLAN_NOTIFICATION_CALLBACK)WlanNotification, NULL, NULL, &dwPrevNotif) != ERROR_SUCCESS)
					throw("[x] Unable to register for notifications");

				//printf("===========================================\n");
				if(dwError = WlanScan(hWlan, &guidInterface, NULL, NULL, NULL) != ERROR_SUCCESS)
					throw("[x] Scan failed, check adapter is enabled");

				// Yawn...
				//while(bWait)
				Sleep(1000);

				// Unregister callback, don't care if it succeeds or not
				WlanRegisterNotification(hWlan, WLAN_NOTIFICATION_SOURCE_NONE, TRUE, NULL, NULL, NULL, &dwPrevNotif);

				if(dwError = WlanGetAvailableNetworkList(hWlan, &guidInterface, NULL, NULL, &wlanNetworkList) != ERROR_SUCCESS)
					throw("[x] Unable to obtain network list");


				for( unsigned int i = 0; i < wlanNetworkList->dwNumberOfItems; i++)
				{
					if (wlanNetworkList->Network[i].wlanSignalQuality >75)
					{
						//					q[i]+=1;
						if(strcmp((char*)wlanNetworkList->Network[i].dot11Ssid.ucSSID,student1) == 0)
						{
							list[0]+=1;
							strcpy(student, "최재환");
							printf("\nSSID:\t\t\t%s\nSignal:\t\t\t%d%\nCount:\t\t\t%d번\n",student,wlanNetworkList->Network[i].wlanSignalQuality,list[0]);
						}
						if(strcmp((char*)wlanNetworkList->Network[i].dot11Ssid.ucSSID,student2) == 0)
						{
							list[1]+=1;//에러카운트
							strcpy(student, "이상훈");
							printf("\nSSID:\t\t\t%s\nSignal:\t\t\t%d%\nCount:\t\t\t%d번\n",student,wlanNetworkList->Network[i].wlanSignalQuality,list[1]);
						}
						if(strcmp((char*)wlanNetworkList->Network[i].dot11Ssid.ucSSID,student3) == 0)
						{
							list[2]+=1;//에러카운트
							strcpy(student, "강광현");
							printf("\nSSID:\t\t\t%s\nSignal:\t\t\t%d%\nCount:\t\t\t%d번\n",student,wlanNetworkList->Network[i].wlanSignalQuality,list[2]);
						}
						if(strcmp((char*)wlanNetworkList->Network[i].dot11Ssid.ucSSID,student4) == 0)
						{
							list[3]+=1;//에러카운트
							strcpy(student, "김유진");
							printf("\nSSID:\t\t\t%s\nSignal:\t\t\t%d%\nCount:\t\t\t%d번\n",student,wlanNetworkList->Network[i].wlanSignalQuality,list[3]);
						}
					}

				}
				printf("=====================================================\n");
				Sleep(100);//소단위check 간격(6sec)
				if(n==10) break;
			}
			for(a=0;a<4;a++)//학생2명 판단
			{
				if(a==0)
					printf("\n\n최재환 :");
				else if(a==1)
					printf("\n\n이상훈 :");
				else if(a==2)
					printf("\n\n강광현 :");
				else if(a==3)
					printf("\n\n김유진 :");

				if(list[a]>=8) 
				{
					q[a][m]=1;//출석일 때 1을 배열에 넣음
					printf("  OKAY\n");
					Sleep(3000);//중간check 간격(7m30s-1m=6m30s)
				}
				else 
				{
					q[a][m]=0;//결석일 때 0을 배열에 넣음
					printf("  ERROR\n");
					Sleep(3000);
				}
			}
			//			q[m]=a;
			m+=1;
			if(m==10) break;
		}//모든 체크 끝

		//////////출석표//////////
		printf("=====================================================\n");
		printf(" 학생이름              학번              출결사항\n");
		printf("=====================================================\n");
		
		for(a=0;a<4;a++)
		{
			//printf("출석현황 : ");
			if(a==0) printf("  최재환             20913110              ");
			else if(a==1) printf("  이상훈             20921656              ");
			else if(a==2) printf("  강광현             20912345              ");
			else if(a==3) printf("  김유진             20954321              ");
			for(m=0;m<10;m++)
			{
				//printf("%d ",q[a][m]);
				sum[a]+=q[a][m];
			}//배열 확인
			//출결확인
			//printf("\n출석결과 : ");
			if(sum[a]>=9) printf("출석\n");
			else if(sum[a]>=7&&sum[a]<=8)
			{
				if(q[a][0]+q[a][1]==0) printf("지각\n");
				else printf("이탈\n");
			}
			else if(sum[a]<=6) printf("결석\n");
			else printf("오류\n");
			//printf("\n");
		}
		printf("=====================================================\n");

	}
	catch(char *szError)
	{
		printf("%s (0x%X)\nQuitting...\n", szError);
	}
	if(wlanNetworkList)
		WlanFreeMemory(wlanNetworkList);
	if(wlanInterfaceList)
		WlanFreeMemory(wlanInterfaceList);
	if(hWlan)
		WlanCloseHandle(hWlan, NULL);


	return dwError;
}