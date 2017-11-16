
/*
在VC下访问 MySql5.5数据库的方法：


方法一：
       通过ODBC数据源+ADO的方法

建立数据源: 
开始菜单－>设置－>控制面板－>管理工具－>数据源(ODBC)－>系统DSN－>添加－>选择MySQL ODBC 3.51 Driver(我下载的MyODBC版本是3.51)

然后在出现的Connector/ODBC Configure Data Source Name对话框中输入

Data Source Name---------ADOLinkMySQL    //数据源名称

Server-------------------localhost      //数据库服务器名称

User---------------------root           //用户名(mysql默认的用户名是root)

Password-----------------123456         //密码(由MySQL root用户管理)

DataName-----------------adolinkmysql   //你刚才建立的数据库名称

　　输入以后，点击Test。如果出现Success;connection was made!表示建立数据源成功，即可　继续下一步

4，最后应用程序就可以通过ADO来访问MySQL了。注意在连接数据库中要用以下代码来连接

_ConnectionPtr m_pConnection;

m_pConnection->Open("DSN=ADOLinkMySQL;",   //数据源名称//
					"root",           //用户//
					"123456",0);      //密码//
					
以下是例子：
					
VC ADO链接MySql数据库

ODBC方式链接。

1、应用程序的stdafx.h头文件中（也可以在其他合适的地方）包含如下语句。

*/

#import "C:\\program files\\commonfiles\\system\\ado\\msado15.dll" no_namespacerename ("EOF","adoEOF")

 

//2、链接数据库

CoInitialize(NULL);                                          //初始化Com组件

_ConnectionPtr conPtr;                                      //数据库链接指针

     //conPtr.CreateInstance("ADODB.Connection");           //Connection用于与数据库服务器的链接

     conPtr.CreateInstance(__uuidof(Connection));            //Connection用于与数据库服务器的链接另一种方式

     /******************连接数据库********************/

     try

     {

         conPtr->ConnectionTimeout = 5;                          //设置连接时间

         // MySqlTest为数据源名 localhost表示本地root表示用户名 sa表示密码

         //链接方式1，使用这种方式链接时要注意在设置数据源时一定要选择一个数据库

         //conPtr->Open("DSN=MySqlTest;server=localhost;","root","sa",adModeUnknown);///连接MySql数据库(测试成功)

         //链接方式2采用这种方式链接时在创建数据源时没有必要选择一个数据库          conPtr->Open("DSN=MySqlTest;server=localhost;database=test;","root","sa",adModeUnknown);///连接MySql数据库(测试成功)

     }

     catch(_com_error e)                                              //捕捉异常

     {

         AfxMessageBox(e.ErrorMessage());

     }

     CoUninitialize();                                       //释放com组件//不知是否有必要

    

//3、访问数据库

_RecordsetPtr recordPtr;//数据集指针

recordPtr.CreateInstance(__uuidof(Recordset));

CString cmdStr=_T("selectname,stuno from student");//student为表名 该表中有两个字段name（字符型）stuno（int型）

     try

     {

              recordPtr->Open(_variant_t(cmdStr),

              conPtr.GetInterfacePtr(),

              adOpenDynamic,

              adLockOptimistic,

              adCmdText);

     }

     catch(_com_error e)

     {

         AfxMessageBox(e.Description());

     }

     while(!(recordPtr->adoEOF))

     {//获取记录

         CStringname=(CString)recordPtr->GetCollect(_variant_t("name")).bstrVal;

         intno=recordPtr->GetCollect(_variant_t("stuno")).intVal;

         recordPtr->MoveNext();

     }

     recordPtr->Close();//关闭记录集

     recordPtr.Release();//释放空间

     conPtr->Close();//关闭连接

     conPtr.Release();//释放空间

 

//非ODBC方式链接。

//链接数据库

CoInitialize(NULL);                                          //初始化Com组件

_ConnectionPtr conPtr;                                       //数据库链接指针

     //conPtr.CreateInstance("ADODB.Connection");            //Connection用于与数据库服务器的链接

     conPtr.CreateInstance(__uuidof(Connection));            //Connection用于与数据库服务器的链接另一种方式

// MySQL ODBC 5.1 Driver为驱动名称（取决于我们为MySql安装的驱动），localhost为服务器地址，test为数据库名，root为用户名（MySql默认用户名为root），sa为密码

     CStringconStr=_T("Driver={MySQL ODBC 5.1 Driver };Server= localhost;Database=test;");

     /******************连接数据库********************/

     try

     {

         conPtr->ConnectionTimeout = 5;                          //设置连接时间

         conPtr->Open((_bstr_t)conStr, _T("root"),_T("sa"), adModeUnknown);

     }

     catch(_com_error e)                                              //捕捉异常

     {

         AfxMessageBox(e.ErrorMessage());

     }

     CoUninitialize();                                       //释放com组件
/*
	 
获取记录集

获取记录集的方式与ODBC方式相同
*/