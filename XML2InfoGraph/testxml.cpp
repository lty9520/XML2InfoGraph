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
struct InfoGraph
{
	string type;
	string text_str;
	string father_name;
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


void ReadParaXml(string m_strXmlPath, vector<InfoGraph>& vecNode)
{
	//��ʼ��һ����ŽY���ĽY���w
	InfoGraph *pNode = new InfoGraph;

	//��ȡxml�ļ����ĵ�
	TiXmlDocument* Document = new TiXmlDocument();

	
	if (!Document->LoadFile(m_strXmlPath.c_str()))
	{
		cerr << Document->ErrorDesc() << endl;
		
	}
	//Document->Print();
	//��Ŀ¼--<VisioDocument>
	TiXmlElement* RootElement = Document->RootElement();
	//����Ԫ�ش�����
	if (RootElement == NULL)
	{
		cerr << "Failed to load file: No root element." << endl;
		Document->Clear();
	}
	//��Ŀ¼�µĵ�һ���ڵ��--<Pages>��
	TiXmlElement* PagesElement = RootElement->FirstChildElement();		
	//for(NextElement;NextElement;NextElement = NextElement->NextSiblingElement())
	while (PagesElement != NULL)
	{
		//����<Pages>��
		if (PagesElement->ValueTStr() == "Pages")
		{
			//����<Page>��
			TiXmlElement* PageElement = PagesElement->FirstChildElement();
			if (PageElement != NULL)
			{
				//��ȡ<Page>����һ��
				TiXmlElement* ShapesElement = PageElement->FirstChildElement();
				while (ShapesElement != NULL)
				{
					//����<Shapes>��
					if (ShapesElement->ValueTStr() == "Shapes")
					{
						//����<Shape>��
						TiXmlElement* ShapeElement = ShapesElement->FirstChildElement();
						if (ShapeElement != NULL)
						{
							//�ж���û�ж���
							while (ShapeElement != NULL)
							{
								//����Shape�ڵ�
								if (ShapeElement->ValueTStr() == "Shape")
								{
									//NextElement = NextElement->NextSiblingElement();
									//����Shape�����ԣ���õ�ǰ�ڵ�����֡����͡���������
									const char* attr_nameu;
									//��õ�ǰ<Shape>�����Ե�"NameU"����
									attr_nameu = ShapeElement->Attribute("NameU");
									string type_nameu(attr_nameu);
									string::size_type idx = type_nameu.find("Dynamic connector");
									if (type_nameu.find("Dynamic connector") == 0)
									{
										ShapeElement = ShapeElement->NextSiblingElement();
									}
									else
									{
										//���type_nameu����
										pNode->type = type_nameu;
										//cout << attr_nameu << endl;
										//��ȡ<Shape>����һ�㣬��ȡText�Ȳ���
										TiXmlElement* InfoElement = ShapeElement->FirstChildElement();
										while (InfoElement != NULL)
										{
											//����Text�ڵ�
											if (InfoElement->ValueTStr() == "Text")
											{
												//������Text�ڵ�
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
										//���뵽������
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
	

	//�ͷ��ڴ�
	delete pNode;
	delete Document;
	cout << "��ȡxml���" << endl;
	
}


int main()
{

	string xml_path = "testԭ.xml";
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