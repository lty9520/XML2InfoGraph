#include "cJSON.h"
#include "cJSON.c"
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>


using namespace std;

//box�����ṹ��
struct BoxSize
{
	int xMin;
	int yMin;
	int xMax;
	int yMax;
};

// ����cJSon���ݹ�

/*

<doc><a a1="1" a2="2">123</a></doc> ת {"doc": {"a": {"-a1": "1","-a2": "2","#text": "123"}}}

*/

string Xml2Json(string strXml)

{
	//ȡ��xml�ַ���
	string pNext = strXml;
	//����Json�洢����
	cJSON * reJson = cJSON_CreateObject();
	//����Json����
	cJSON * jsonArray = cJSON_CreateArray();
	//�����ֵ
	string strArrayKey = "";
	//��λ�ַ���λ�ñ�ʶ
	int nPos = 0;

	//Ѱ��<Shapes>��ǩ������visioͼ�ؼ�����<Shapes>���ţ�
	if (nPos = pNext.find("<Shapes>") != pNext.npos)
	{
		pNext += 8;
		while ((nPos = pNext.find("<")) != pNext.npos)

		{

			// ��ȡ��һ���ڵ㣬�磺<doc><a a1="1" a2="2">123</a></doc>

			int nPosS = pNext.find("<");

			int nPosE = pNext.find(">");

			if (nPosS < 0 || nPosE < 0)

			{

				printf("key error!");

			}

			//��ü�ֵ
			string strKey = pNext.substr(nPosS + 1, nPosE - nPosS - 1);

			// �������ԣ��磺<a a1="1" a2="2">

			cJSON * jsonVal = NULL;

			if ((nPos = strKey.find("=")) > 0)

			{

				jsonVal = cJSON_CreateObject();

				int nPos = strKey.find(" ");

				string temp = strKey.substr(nPos + 1);

				strKey = strKey.substr(0, nPos);

				while ((nPos = temp.find("=")) > 0)

				{

					int nPos1 = temp.find("=");

					int nPos2 = temp.find("\" ", nPos1 + 1);


					string strSubKey = temp.substr(0, nPos1);

					string strSubVal = temp.substr(nPos1 + 1);

					if (nPos2 > 0)

						strSubVal = temp.substr(nPos1 + 1, nPos2 - nPos1 - 1);


					// ȥ��ת���ַ� \"

					if ((int)(nPos = strSubVal.find("\"")) >= 0)

					{

						int nEnd = strSubVal.find("\\", nPos + 1);

						strSubVal = strSubVal.substr(nPos + 1, nEnd - nPos - 1);

					}

					cJSON_AddItemToObject(jsonVal, ("-" + strSubKey).c_str(), cJSON_CreateString(strSubVal.c_str()));


					if (nPos2 < 0)

						break;


					temp = temp.substr(nPos2 + 2);

				}

			}


			int nPosKeyE = pNext.find("</" + strKey + ">");

			if (nPosKeyE < 0)

			{

				printf("key error!");

			}

			// ��ȡ�ڵ����ݣ���<a a1="1" a2="2">123</a> �� 123

			string strVal = pNext.substr(nPosE + 1, nPosKeyE - nPosE - 1);

			if ((nPos = strVal.find("<")) >= 0)

			{

				// �����ӽڵ㣬��<a a1="1" a2="2">123</a>

				strVal = Xml2Json(strVal);


				if (jsonVal)

				{

					if (strVal != "")

						cJSON_AddItemToObject(jsonVal, "#text", cJSON_Parse(strVal.c_str()));

				}

				else

				{

					jsonVal = cJSON_Parse(strVal.c_str());

				}

			}

			else

			{

				// �������ӽڵ㣬��123

				if (jsonVal)

				{

					if (strVal != "")

						cJSON_AddItemToObject(jsonVal, "#text", cJSON_CreateString(strVal.c_str()));

				}

				else

				{

					jsonVal = cJSON_CreateString(strVal.c_str());

				}

			}


			// ��ȡ��һ���ڵ�

			pNext = pNext.substr(nPosKeyE + strKey.size() + 3);


			// ������һ�ڵ��ж��Ƿ�Ϊ����

			int nPosNext = pNext.find("<");

			int nPosNextSame = pNext.find("<" + strKey + ">");

			if (strArrayKey != "" || (nPosNext >= 0 && nPosNextSame >= 0 && nPosNext == nPosNextSame))

			{

				// ����

				cJSON_AddItemToArray(jsonArray, jsonVal);

				strArrayKey = strKey;

			}

			else

			{

				// ������

				cJSON_AddItemToObject(reJson, strKey.c_str(), jsonVal);

			}

		}
	}
	



	if (strArrayKey != "")

	{

		cJSON_AddItemToObject(reJson, strArrayKey.c_str(), jsonArray);

	}


	string strJson = cJSON_Print(reJson);


	if (reJson)

	{

		cJSON_Delete(reJson);

		reJson = NULL;

	}


	return strJson;

}


bool ReadParaXml(string m_strXmlPath, vector<BoxSize>& vecNode)
{
	//��ʼ��һ����ŽY���ĽY���w
	BoxSize *pNode = new BoxSize;

	//��ȡxml�ļ����ĵ�
	TiXmlDocument* Document = new TiXmlDocument();

	
	if (!Document->LoadFile(m_strXmlPath.c_str()))
	{
		cerr << Document->ErrorDesc() << endl;
		return false;
	}
	//Document->Print();
	//��Ŀ¼
	TiXmlElement* RootElement = Document->RootElement();
	//����Ԫ�ش�����
	if (RootElement == NULL)
	{
		cerr << "Failed to load file: No root element." << endl;
		Document->Clear();
	}
	//��Ŀ¼�µĵ�һ���ڵ��
	TiXmlElement* NextElement = RootElement->FirstChildElement();		
	//for(NextElement;NextElement;NextElement = NextElement->NextSiblingElement())
	//�ж���û�ж���
	while (NextElement != NULL)		
	{
		//����object�ڵ�
		if (NextElement->ValueTStr() == "Shapes")		
		{
			//NextElement = NextElement->NextSiblingElement();

			TiXmlElement* BoxElement = NextElement->FirstChildElement();
			//����box�ڵ�
			while (BoxElement->ValueTStr() != "Shape")		
			{
				BoxElement = BoxElement->NextSiblingElement();
			}
			//������xmin�ڵ�
			TiXmlElement* xminElemeng = BoxElement->FirstChildElement();
			{
				//�ֱ��ȡ�ĸ���ֵ
				pNode->xMin = atof(xminElemeng->GetText());
				TiXmlElement* yminElemeng = xminElemeng->NextSiblingElement();
				pNode->yMin = atof(yminElemeng->GetText());
				TiXmlElement* xmaxElemeng = yminElemeng->NextSiblingElement();
				pNode->xMax = atof(xmaxElemeng->GetText());
				TiXmlElement* ymaxElemeng = xmaxElemeng->NextSiblingElement();
				pNode->yMax = atof(ymaxElemeng->GetText());

				//���뵽������
				vecNode.push_back(*pNode);
			}
		}
		NextElement = NextElement->NextSiblingElement();
	}

	//�ͷ��ڴ�
	delete pNode;
	delete Document;
	cout << "���xml�Ķ�ȡ" << endl;
	return true;
}


int main()
{

	string xml_path = "test.xml";
	vector<BoxSize> vecNode;

	ReadParaXml(xml_path, vecNode);

	string result = "";

	//result = Xml2Json(xml);

	system("pause");
	return 0;
}