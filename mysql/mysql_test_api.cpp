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
					

方法二：
      	通过MySql的API函数集访问MySql数据库。				
					
VC环境下使用mysql API函数访问MySQL数据库 
1.       安装MySQL的完整版；（在安装过程中可以选择）
2.       将C:\Program Files\MySQL\MySQL Server 5.0\lib\opt目录下的“libmysql.lib”文件复制到创建的VC工程目录下；
3.       程序中包括头文件，连入库文件：
*/

#include <winsock.h>

#include "C:\Program Files\MySQL\MySQL Server 5.0\include\mysql.h"  //（数据库的安装路径）



#pragma comment(lib,"libmysql.lib")



//4.       程序例子

#include "stdafx.h"

#include <winsock.h>

#include "C:\Program Files\MySQL\MySQL Server 5.0\include\mysql.h" 

#pragma comment(lib,"libmysql.lib") 

int main(int argc, char* argv[])

{
	
	MYSQL mysql;
	
	MYSQL_RES *result;
	
	MYSQL_ROW row;
	
	mysql_init(&mysql); //初始化mysql结构
	
	if(!mysql_real_connect(&mysql,"MyXp","UserDB","123456","userdata",0,NULL,0))//连接服务器
		
	{
		
		fprintf(stderr,"Failed to connect to database: Error: %s\n",
			
			mysql_error(&mysql));
		
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	/////////////////////////插入数据/////////////////////////////////////////        
	
	if(mysql_query(&mysql,"insert into UserDataTable values('rm','209','209','2153@qq.com',
		
		'23213','yjiang','26.222.171.2','2010-04-01 10:53:12')"))//进行查询
		
	{
		
		fprintf(stdout,"mysql_query() insert failed!\n");
		
	}    
	
	//////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////  查询数据  ///////////////////////////////                 
	
	if(mysql_query(&mysql,"select * from UserDataTable"))
		
	{
		
		fprintf(stdout,"mysql_query() select failed!\n");
		
	}
	
	result = mysql_store_result(&mysql);//读取一个到客户的查询的全部结果，
	
	//分配一个MYSQL_RES结构，并且把结果放进这个结构中。(获得结果集)
	
	while (row = mysql_fetch_row(result))//取出结果集中记录
		
	{
		
		fprintf(stdout,"UserName:%s password: %s Email:%s City:%s \n",row[0],row[1],row[3],row[5]);//输出这行记录
		
	}
	
	mysql_free_result(result);   //释放结果集
	
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////删除数据///////////////////////////////////    
	
	if(mysql_query(&mysql,"delete from UserDataTable where UserName = 'chz'"))
		
	{
		
		fprintf(stdout,"mysql_query() delete failed!\n");
		
	}  
	
	//////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////更新数据/////////////////////////////////////
	
	if(mysql_query(&mysql,"update UserDataTable set PassWord = 'lyj0662',ConPass = 'lyj0662' where UserName = 'lyj'"))
		
	{
		
		fprintf(stdout,"mysql_query() updata failed!\n");
		
	} 
	
	mysql_close(&mysql);//释放连接
	
	return 0;
	
}