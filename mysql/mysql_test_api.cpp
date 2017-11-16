/*
��VC�·��� MySql5.5���ݿ�ķ�����


����һ��
       ͨ��ODBC����Դ+ADO�ķ���

��������Դ: 
��ʼ�˵���>���ã�>������壭>�����ߣ�>����Դ(ODBC)��>ϵͳDSN��>��ӣ�>ѡ��MySQL ODBC 3.51 Driver(�����ص�MyODBC�汾��3.51)

Ȼ���ڳ��ֵ�Connector/ODBC Configure Data Source Name�Ի���������

Data Source Name---------ADOLinkMySQL    //����Դ����

Server-------------------localhost      //���ݿ����������

User---------------------root           //�û���(mysqlĬ�ϵ��û�����root)

Password-----------------123456         //����(��MySQL root�û�����)

DataName-----------------adolinkmysql   //��ղŽ��������ݿ�����

���������Ժ󣬵��Test���������Success;connection was made!��ʾ��������Դ�ɹ������ɡ�������һ��

4�����Ӧ�ó���Ϳ���ͨ��ADO������MySQL�ˡ�ע�����������ݿ���Ҫ�����´���������

_ConnectionPtr m_pConnection;

m_pConnection->Open("DSN=ADOLinkMySQL;",   //����Դ����//
					"root",           //�û�//
					"123456",0);      //����//
					

��������
      	ͨ��MySql��API����������MySql���ݿ⡣				
					
VC������ʹ��mysql API��������MySQL���ݿ� 
1.       ��װMySQL�������棻���ڰ�װ�����п���ѡ��
2.       ��C:\Program Files\MySQL\MySQL Server 5.0\lib\optĿ¼�µġ�libmysql.lib���ļ����Ƶ�������VC����Ŀ¼�£�
3.       �����а���ͷ�ļ���������ļ���
*/

#include <winsock.h>

#include "C:\Program Files\MySQL\MySQL Server 5.0\include\mysql.h"  //�����ݿ�İ�װ·����



#pragma comment(lib,"libmysql.lib")



//4.       ��������

#include "stdafx.h"

#include <winsock.h>

#include "C:\Program Files\MySQL\MySQL Server 5.0\include\mysql.h" 

#pragma comment(lib,"libmysql.lib") 

int main(int argc, char* argv[])

{
	
	MYSQL mysql;
	
	MYSQL_RES *result;
	
	MYSQL_ROW row;
	
	mysql_init(&mysql); //��ʼ��mysql�ṹ
	
	if(!mysql_real_connect(&mysql,"MyXp","UserDB","123456","userdata",0,NULL,0))//���ӷ�����
		
	{
		
		fprintf(stderr,"Failed to connect to database: Error: %s\n",
			
			mysql_error(&mysql));
		
	}
	
	//////////////////////////////////////////////////////////////////////////
	
	/////////////////////////��������/////////////////////////////////////////        
	
	if(mysql_query(&mysql,"insert into UserDataTable values('rm','209','209','2153@qq.com',
		
		'23213','yjiang','26.222.171.2','2010-04-01 10:53:12')"))//���в�ѯ
		
	{
		
		fprintf(stdout,"mysql_query() insert failed!\n");
		
	}    
	
	//////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////  ��ѯ����  ///////////////////////////////                 
	
	if(mysql_query(&mysql,"select * from UserDataTable"))
		
	{
		
		fprintf(stdout,"mysql_query() select failed!\n");
		
	}
	
	result = mysql_store_result(&mysql);//��ȡһ�����ͻ��Ĳ�ѯ��ȫ�������
	
	//����һ��MYSQL_RES�ṹ�����Ұѽ���Ž�����ṹ�С�(��ý����)
	
	while (row = mysql_fetch_row(result))//ȡ��������м�¼
		
	{
		
		fprintf(stdout,"UserName:%s password: %s Email:%s City:%s \n",row[0],row[1],row[3],row[5]);//������м�¼
		
	}
	
	mysql_free_result(result);   //�ͷŽ����
	
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////ɾ������///////////////////////////////////    
	
	if(mysql_query(&mysql,"delete from UserDataTable where UserName = 'chz'"))
		
	{
		
		fprintf(stdout,"mysql_query() delete failed!\n");
		
	}  
	
	//////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////��������/////////////////////////////////////
	
	if(mysql_query(&mysql,"update UserDataTable set PassWord = 'lyj0662',ConPass = 'lyj0662' where UserName = 'lyj'"))
		
	{
		
		fprintf(stdout,"mysql_query() updata failed!\n");
		
	} 
	
	mysql_close(&mysql);//�ͷ�����
	
	return 0;
	
}