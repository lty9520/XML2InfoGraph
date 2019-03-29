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
struct InfoGraph
{
	string type;
	string text_str;
	string father_name;
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


void ReadParaXml(string m_strXmlPath, vector<InfoGraph>& vecNode)
{
	//初始化一個存放結果的結構體
	InfoGraph *pNode = new InfoGraph;

	//读取xml文件的文档
	TiXmlDocument* Document = new TiXmlDocument();

	
	if (!Document->LoadFile(m_strXmlPath.c_str()))
	{
		cerr << Document->ErrorDesc() << endl;
		
	}
	//Document->Print();
	//根目录--<VisioDocument>
	TiXmlElement* RootElement = Document->RootElement();
	//检测根元素存在性
	if (RootElement == NULL)
	{
		cerr << "Failed to load file: No root element." << endl;
		Document->Clear();
	}
	//根目录下的第一个节点层--<Pages>层
	TiXmlElement* PagesElement = RootElement->FirstChildElement();		
	//for(NextElement;NextElement;NextElement = NextElement->NextSiblingElement())
	while (PagesElement != NULL)
	{
		//读到<Pages>层
		if (PagesElement->ValueTStr() == "Pages")
		{
			//读到<Page>层
			TiXmlElement* PageElement = PagesElement->FirstChildElement();
			if (PageElement != NULL)
			{
				//读取<Page>的下一层
				TiXmlElement* ShapesElement = PageElement->FirstChildElement();
				while (ShapesElement != NULL)
				{
					//读到<Shapes>层
					if (ShapesElement->ValueTStr() == "Shapes")
					{
						//读到<Shape>层
						TiXmlElement* ShapeElement = ShapesElement->FirstChildElement();
						if (ShapeElement != NULL)
						{
							//判断有没有读完
							while (ShapeElement != NULL)
							{
								//读到Shape节点
								if (ShapeElement->ValueTStr() == "Shape")
								{
									//NextElement = NextElement->NextSiblingElement();
									//解析Shape的属性，获得当前节点的名字、类型、父级名字
									const char* attr_nameu;
									//获得当前<Shape>的属性的"NameU"参数
									attr_nameu = ShapeElement->Attribute("NameU");
									string type_nameu(attr_nameu);
									string::size_type idx = type_nameu.find("Dynamic connector");
									if (type_nameu.find("Dynamic connector") == 0)
									{
										ShapeElement = ShapeElement->NextSiblingElement();
									}
									else
									{
										//添加type_nameu参数
										pNode->type = type_nameu;
										//cout << attr_nameu << endl;
										//读取<Shape>的下一层，获取Text等参数
										TiXmlElement* InfoElement = ShapeElement->FirstChildElement();
										while (InfoElement != NULL)
										{
											//读到Text节点
											if (InfoElement->ValueTStr() == "Text")
											{
												//索引到Text节点
												//TiXmlElement* textElement = InfoElement->FirstChildElement();
												pNode->text_str = InfoElement->GetText();
												//cout << InfoElement->GetText() << endl;
												InfoElement = InfoElement->NextSiblingElement();
											}
											if (InfoElement->ValueTStr() == "Prop")
											{
												//TiXmlElement* ResElement = InfoElement->FirstChildElement();
												const char* attr_id;
												attr_id = InfoElement->Attribute("ID");
												if (strcmp(attr_id, "3") == 0)
												{
													TiXmlElement* valElement = InfoElement->FirstChildElement();
													while (valElement != NULL)
													{
														if (valElement->ValueTStr() == "Value")
														{
															pNode->father_name = valElement->GetText();
														}


														valElement = valElement->NextSiblingElement();

													}
												}


												//InfoElement = InfoElement->NextSiblingElement();

											}


											InfoElement = InfoElement->NextSiblingElement();




										}
										ShapeElement = ShapeElement->NextSiblingElement();
										//加入到向量中
										vecNode.push_back(*pNode);
									}
									
									
									
								}
								
								
							}
							
						}
						
						
						//ShapesElement = ShapesElement->NextSiblingElement();
						
						
					} 
					
					ShapesElement = ShapesElement->NextSiblingElement();
					
					
				}
			}
		} 
		
		
		PagesElement = PagesElement->NextSiblingElement();
		
		
		
	}
	

	//释放内存
	delete pNode;
	delete Document;
	cout << "读取xml完成" << endl;
	
}


int main()
{

	string xml_path = "test原.xml";
	vector<InfoGraph> vecNode;

	ReadParaXml(xml_path, vecNode);

	//cout << vecNode.size() << endl;
	for (int i = 0; i < vecNode.size(); i++)
	{
		cout << "ytpe_name = " << vecNode[i].type << endl;
		cout << "text_str = " << vecNode[i].text_str << endl;
		cout << "father_name = " << vecNode[i].father_name << endl;
	}

	string result = "";

	//result = Xml2Json(xml);

	system("pause");
	return 0;
}