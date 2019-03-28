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

//box参数结构体
struct BoxSize
{
	int xMin;
	int yMin;
	int xMax;
	int yMax;
};

// 依赖cJSon，递归

/*

<doc><a a1="1" a2="2">123</a></doc> 转 {"doc": {"a": {"-a1": "1","-a2": "2","#text": "123"}}}

*/

string Xml2Json(string strXml)

{
	//取得xml字符串
	string pNext = strXml;
	//定义Json存储对象
	cJSON * reJson = cJSON_CreateObject();
	//定义Json数组
	cJSON * jsonArray = cJSON_CreateArray();
	//数组键值
	string strArrayKey = "";
	//定位字符串位置标识
	int nPos = 0;

	//寻找<Shapes>标签（所有visio图控件都在<Shapes>里存放）
	if (nPos = pNext.find("<Shapes>") != pNext.npos)
	{
		pNext += 8;
		while ((nPos = pNext.find("<")) != pNext.npos)

		{

			// 获取第一个节点，如：<doc><a a1="1" a2="2">123</a></doc>

			int nPosS = pNext.find("<");

			int nPosE = pNext.find(">");

			if (nPosS < 0 || nPosE < 0)

			{

				printf("key error!");

			}

			//获得键值
			string strKey = pNext.substr(nPosS + 1, nPosE - nPosS - 1);

			// 解释属性，如：<a a1="1" a2="2">

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


					// 去除转义字符 \"

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

			// 获取节点内容，如<a a1="1" a2="2">123</a> 或 123

			string strVal = pNext.substr(nPosE + 1, nPosKeyE - nPosE - 1);

			if ((nPos = strVal.find("<")) >= 0)

			{

				// 包含子节点，如<a a1="1" a2="2">123</a>

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

				// 不包含子节点，如123

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


			// 获取下一个节点

			pNext = pNext.substr(nPosKeyE + strKey.size() + 3);


			// 根据下一节点判断是否为数组

			int nPosNext = pNext.find("<");

			int nPosNextSame = pNext.find("<" + strKey + ">");

			if (strArrayKey != "" || (nPosNext >= 0 && nPosNextSame >= 0 && nPosNext == nPosNextSame))

			{

				// 数组

				cJSON_AddItemToArray(jsonArray, jsonVal);

				strArrayKey = strKey;

			}

			else

			{

				// 非数组

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
	//初始化一個存放結果的結構體
	BoxSize *pNode = new BoxSize;

	//读取xml文件的文档
	TiXmlDocument* Document = new TiXmlDocument();

	
	if (!Document->LoadFile(m_strXmlPath.c_str()))
	{
		cerr << Document->ErrorDesc() << endl;
		return false;
	}
	//Document->Print();
	//根目录
	TiXmlElement* RootElement = Document->RootElement();
	//检测根元素存在性
	if (RootElement == NULL)
	{
		cerr << "Failed to load file: No root element." << endl;
		Document->Clear();
	}
	//根目录下的第一个节点层
	TiXmlElement* NextElement = RootElement->FirstChildElement();		
	//for(NextElement;NextElement;NextElement = NextElement->NextSiblingElement())
	//判断有没有读完
	while (NextElement != NULL)		
	{
		//读到object节点
		if (NextElement->ValueTStr() == "Shapes")		
		{
			//NextElement = NextElement->NextSiblingElement();

			TiXmlElement* BoxElement = NextElement->FirstChildElement();
			//读到box节点
			while (BoxElement->ValueTStr() != "Shape")		
			{
				BoxElement = BoxElement->NextSiblingElement();
			}
			//索引到xmin节点
			TiXmlElement* xminElemeng = BoxElement->FirstChildElement();
			{
				//分别读取四个数值
				pNode->xMin = atof(xminElemeng->GetText());
				TiXmlElement* yminElemeng = xminElemeng->NextSiblingElement();
				pNode->yMin = atof(yminElemeng->GetText());
				TiXmlElement* xmaxElemeng = yminElemeng->NextSiblingElement();
				pNode->xMax = atof(xmaxElemeng->GetText());
				TiXmlElement* ymaxElemeng = xmaxElemeng->NextSiblingElement();
				pNode->yMax = atof(ymaxElemeng->GetText());

				//加入到向量中
				vecNode.push_back(*pNode);
			}
		}
		NextElement = NextElement->NextSiblingElement();
	}

	//释放内存
	delete pNode;
	delete Document;
	cout << "完成xml的读取" << endl;
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