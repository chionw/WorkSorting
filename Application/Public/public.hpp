#ifndef IBOX_PUBLIC_2014_11
#define IBOX_PUBLIC_2014_11

/***************模块标识*************
*************************************/
typedef enum _moduleFlag
{
	databaseEnum 	= 1,//数据库
	networkEnum 	= 2,//网络通讯
	viewEnum 		= 3,//界面
}ENUM_MODULE_FLAG;



/**************消息定义*************
************************************/
/*自定义的控件的消息*/
#define WM_CUSTOM_LIST_DBCLICK				WM_USER + 1
#define WM_CUSTOM_LIST_MENU_DEL				WM_USER + 2
#define WM_CUSTOM_LIST_MENU_CLEAR			WM_USER + 3
#define WM_CUSTOM_LIST_MENU_COPY			WM_USER + 4
#define WM_CUSTOM_LIST_RCLICK				WM_USER + 5
#define WM_CUSTOM_LIST_MENU_ADDTO_CLIENT	WM_USER + 6

//用户右键单击了列表
/*界面消息定义*/
#define WM_CUSTOM_MENU_MAIN				WM_USER + 100
#define WM_CUSTOM_MENU_LOCAL			WM_USER + 101
#define WM_CUSTOM_MENU_CLIENT			WM_USER + 102
#define WM_CUSTOM_MENU_PACKAGE			WM_USER + 103
#define WM_CUSTOM_MENU_TRANSACTION		WM_USER + 104

#define WM_CUSTOM_CLOSE 		WM_USER + 120
#define WM_CUSTOM_VIEW_BUILD 	WM_USER + 121

#define WM_MESSAGE_MAIN			WM_USER + 200
#define WM_MESSAGE_PACKAGE		WM_USER + 300
#define WM_MESSAGE_CLIENT		WM_USER + 400
#define WM_MESSAGE_TRANSACTION	WM_USER + 500
#define WM_MESSAGE_LOCAL		WM_USER + 600

#define WM_CUSTOM_MAIN_ADDTO_CLIENT_LIST WM_MESSAGE_MAIN + 1

#define WM_CUSTOM_LOCAL_SAVE	WM_MESSAGE_LOCAL + 1

#define WM_CUSTOM_CLIENT_ADD	WM_MESSAGE_CLIENT + 1
#define WM_CUSTOM_CLIENT_DEL	WM_MESSAGE_CLIENT + 2

#define WM_CUSTOM_PACKAGE_ADD	WM_MESSAGE_PACKAGE + 1
#define WM_CUSTOM_PACKAGE_DEL	WM_MESSAGE_PACKAGE + 2

#define WM_CUSTOM_TRANSACTION_ADD	WM_MESSAGE_TRANSACTION + 1
#define WM_CUSTOM_TRANSACTION_DEL	WM_MESSAGE_TRANSACTION + 2

/*本系统消息定义*/
#define WM_CUSTOM_REBOOT WM_USER + 700


/*客户端行为消息定义*/
#define WM_CUSTOM_TERMINAL_VISIST				WM_USER + 800			//客户端查询 wParam = 客户端ID（string*）, lParam = NULL
#define WM_CUSTOM_TERMINAL_START_DOWNDLOAD		WM_USER + 801			//客户端开始下载wParam = 客户端ID（string*）, lParam = NULL
#define WM_CUSTOM_TERMINAL_LOADING				WM_USER + 802			//客户端正在下载wParam = 客户端ID（string*）, lParam = 下载进度（int,0~100）
#define WM_CUSTOM_TERMINAL_FINISH_DOWNLOAD		WM_USER + 803			//客户端下载完成wParam = 客户端ID（string*）, lParam = NULL
#define WM_CUSTOM_TERMINAL_CONNECT				WM_USER + 804			//客户端连接进系统wParam = 客户端ID（string*）, lParam = NULL
#define WM_CUSTOM_TERMINAL_DISCONNECT			WM_USER + 805			//客户端断开wParam = 客户端ID（string*）, lParam = NULL
/*日志消息定义
* PostMessage(
*	取下值之一；
*	要发送的字符串，需要是堆内存中的字符串；
*	模块标志，见ENUM_MODULE_FLAG定义)
*/

#define WM_LOGGER_INFO 	WM_USER + 900
#define WM_LOGGER_WARM 	WM_USER + 901
#define WM_LOGGER_ERROR WM_USER + 902












#endif