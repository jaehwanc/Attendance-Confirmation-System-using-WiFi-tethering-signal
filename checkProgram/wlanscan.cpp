/**
* File Name: "wlanscan.cpp"
* Description:
*     - ���� ��ȿ� �����ϴ� AP��ȣ�� �����Ͽ� �⼮ �Ǵ��ϴ� ���α׷�
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
*    2) struct WLAN_INTERFACE_INFO_LIST : AP ���� ���� Ȯ�� ����ü
*    3) struct WLAN_AVAILABLE_NETWORK_LIST : �����ϴ� AP�� ���� ����(SSID, Signal Quality)
*    4) ������ ���� display �� threshold�� ���߾� �⼮üũ �˰��� ����
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
			printf("%d��° check\n",m+1);
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
							strcpy(student, "����ȯ");
							printf("\nSSID:\t\t\t%s\nSignal:\t\t\t%d%\nCount:\t\t\t%d��\n",student,wlanNetworkList->Network[i].wlanSignalQuality,list[0]);
						}
						if(strcmp((char*)wlanNetworkList->Network[i].dot11Ssid.ucSSID,student2) == 0)
						{
							list[1]+=1;//����ī��Ʈ
							strcpy(student, "�̻���");
							printf("\nSSID:\t\t\t%s\nSignal:\t\t\t%d%\nCount:\t\t\t%d��\n",student,wlanNetworkList->Network[i].wlanSignalQuality,list[1]);
						}
						if(strcmp((char*)wlanNetworkList->Network[i].dot11Ssid.ucSSID,student3) == 0)
						{
							list[2]+=1;//����ī��Ʈ
							strcpy(student, "������");
							printf("\nSSID:\t\t\t%s\nSignal:\t\t\t%d%\nCount:\t\t\t%d��\n",student,wlanNetworkList->Network[i].wlanSignalQuality,list[2]);
						}
						if(strcmp((char*)wlanNetworkList->Network[i].dot11Ssid.ucSSID,student4) == 0)
						{
							list[3]+=1;//����ī��Ʈ
							strcpy(student, "������");
							printf("\nSSID:\t\t\t%s\nSignal:\t\t\t%d%\nCount:\t\t\t%d��\n",student,wlanNetworkList->Network[i].wlanSignalQuality,list[3]);
						}
					}

				}
				printf("=====================================================\n");
				Sleep(100);//�Ҵ���check ����(6sec)
				if(n==10) break;
			}
			for(a=0;a<4;a++)//�л�2�� �Ǵ�
			{
				if(a==0)
					printf("\n\n����ȯ :");
				else if(a==1)
					printf("\n\n�̻��� :");
				else if(a==2)
					printf("\n\n������ :");
				else if(a==3)
					printf("\n\n������ :");

				if(list[a]>=8) 
				{
					q[a][m]=1;//�⼮�� �� 1�� �迭�� ����
					printf("  OKAY\n");
					Sleep(3000);//�߰�check ����(7m30s-1m=6m30s)
				}
				else 
				{
					q[a][m]=0;//�Ἦ�� �� 0�� �迭�� ����
					printf("  ERROR\n");
					Sleep(3000);
				}
			}
			//			q[m]=a;
			m+=1;
			if(m==10) break;
		}//��� üũ ��

		//////////�⼮ǥ//////////
		printf("=====================================================\n");
		printf(" �л��̸�              �й�              ������\n");
		printf("=====================================================\n");
		
		for(a=0;a<4;a++)
		{
			//printf("�⼮��Ȳ : ");
			if(a==0) printf("  ����ȯ             20913110              ");
			else if(a==1) printf("  �̻���             20921656              ");
			else if(a==2) printf("  ������             20912345              ");
			else if(a==3) printf("  ������             20954321              ");
			for(m=0;m<10;m++)
			{
				//printf("%d ",q[a][m]);
				sum[a]+=q[a][m];
			}//�迭 Ȯ��
			//���Ȯ��
			//printf("\n�⼮��� : ");
			if(sum[a]>=9) printf("�⼮\n");
			else if(sum[a]>=7&&sum[a]<=8)
			{
				if(q[a][0]+q[a][1]==0) printf("����\n");
				else printf("��Ż\n");
			}
			else if(sum[a]<=6) printf("�Ἦ\n");
			else printf("����\n");
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