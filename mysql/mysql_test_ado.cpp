
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
					
���������ӣ�
					
VC ADO����MySql���ݿ�

ODBC��ʽ���ӡ�

1��Ӧ�ó����stdafx.hͷ�ļ��У�Ҳ�������������ʵĵط�������������䡣

*/

#import "C:\\program files\\commonfiles\\system\\ado\\msado15.dll" no_namespacerename ("EOF","adoEOF")

 

//2���������ݿ�

CoInitialize(NULL);                                          //��ʼ��Com���

_ConnectionPtr conPtr;                                      //���ݿ�����ָ��

     //conPtr.CreateInstance("ADODB.Connection");           //Connection���������ݿ������������

     conPtr.CreateInstance(__uuidof(Connection));            //Connection���������ݿ��������������һ�ַ�ʽ

     /******************�������ݿ�********************/

     try

     {

         conPtr->ConnectionTimeout = 5;                          //��������ʱ��

         // MySqlTestΪ����Դ�� localhost��ʾ����root��ʾ�û��� sa��ʾ����

         //���ӷ�ʽ1��ʹ�����ַ�ʽ����ʱҪע������������Դʱһ��Ҫѡ��һ�����ݿ�

         //conPtr->Open("DSN=MySqlTest;server=localhost;","root","sa",adModeUnknown);///����MySql���ݿ�(���Գɹ�)

         //���ӷ�ʽ2�������ַ�ʽ����ʱ�ڴ�������Դʱû�б�Ҫѡ��һ�����ݿ�          conPtr->Open("DSN=MySqlTest;server=localhost;database=test;","root","sa",adModeUnknown);///����MySql���ݿ�(���Գɹ�)

     }

     catch(_com_error e)                                              //��׽�쳣

     {

         AfxMessageBox(e.ErrorMessage());

     }

     CoUninitialize();                                       //�ͷ�com���//��֪�Ƿ��б�Ҫ

    

//3���������ݿ�

_RecordsetPtr recordPtr;//���ݼ�ָ��

recordPtr.CreateInstance(__uuidof(Recordset));

CString cmdStr=_T("selectname,stuno from student");//studentΪ���� �ñ����������ֶ�name���ַ��ͣ�stuno��int�ͣ�

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

     {//��ȡ��¼

         CStringname=(CString)recordPtr->GetCollect(_variant_t("name")).bstrVal;

         intno=recordPtr->GetCollect(_variant_t("stuno")).intVal;

         recordPtr->MoveNext();

     }

     recordPtr->Close();//�رռ�¼��

     recordPtr.Release();//�ͷſռ�

     conPtr->Close();//�ر�����

     conPtr.Release();//�ͷſռ�

 

//��ODBC��ʽ���ӡ�

//�������ݿ�

CoInitialize(NULL);                                          //��ʼ��Com���

_ConnectionPtr conPtr;                                       //���ݿ�����ָ��

     //conPtr.CreateInstance("ADODB.Connection");            //Connection���������ݿ������������

     conPtr.CreateInstance(__uuidof(Connection));            //Connection���������ݿ��������������һ�ַ�ʽ

// MySQL ODBC 5.1 DriverΪ�������ƣ�ȡ��������ΪMySql��װ����������localhostΪ��������ַ��testΪ���ݿ�����rootΪ�û�����MySqlĬ���û���Ϊroot����saΪ����

     CStringconStr=_T("Driver={MySQL ODBC 5.1 Driver };Server= localhost;Database=test;");

     /******************�������ݿ�********************/

     try

     {

         conPtr->ConnectionTimeout = 5;                          //��������ʱ��

         conPtr->Open((_bstr_t)conStr, _T("root"),_T("sa"), adModeUnknown);

     }

     catch(_com_error e)                                              //��׽�쳣

     {

         AfxMessageBox(e.ErrorMessage());

     }

     CoUninitialize();                                       //�ͷ�com���
/*
	 
��ȡ��¼��

��ȡ��¼���ķ�ʽ��ODBC��ʽ��ͬ
*/