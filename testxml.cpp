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

cJSON* array = cJSON_CreateArray();
cJSON_AddItemToObject(root, "testarray", array);

for (int i = 0; i < 5; i++){

cJSON*arrayobj = cJSON_CreateObject();

cJSON_AddItemToArray(array, arrayobj);

cJSON_AddItemToObject(arrayobj, "arrayobjnum", cJSON_CreateNumber(i));

}

*/

void addChildren(vector<InfoGraph>& children, cJSON * fatherElem)
{
	//create children
	cJSON * first_childElem = cJSON_CreateArray();

	cJSON_AddItemToObject(fatherElem, "children", first_childElem);

	
	for (auto iter = children.cbegin(); iter != children.cend();)
	{
		cJSON * temp_child = cJSON_CreateObject();
		cJSON_AddItemToArray(first_childElem, temp_child);
		cJSON_AddItemToObject(temp_child, "name", cJSON_CreateString(iter->text_str.c_str()));
		iter = children.erase(iter);
		
		temp_child = cJSON_CreateObject();
	}
	
	
	

	//cJSON_Delete(temp_child);
	//cJSON_Delete(first_childElem);

	//return fatherElem;
}

vector<InfoGraph> findNextChildren(vector<InfoGraph>& originNode, string father_name)
{
	string temp_root_name = father_name;
	//findd children 
	vector<InfoGraph> child;
	string fir_child_name;
	for (auto iter = originNode.cbegin(); iter != originNode.cend();)
	{
		if (iter->father_name == temp_root_name)
		{
			InfoGraph temp = *iter;
			child.push_back(temp);
			fir_child_name = iter->text_str;
			iter = originNode.erase(iter);
		}
		else
			iter++;
	}
	return child;
}

string Xml2Json(vector<InfoGraph>& vecNode)

{
	string strJson;

	cJSON * rootElem = cJSON_CreateObject();
	string root_name;
	//find root elem and create it 
	for (auto iter = vecNode.cbegin(); iter != vecNode.cend();)
	{
		if (iter->type.find("External entity") == 0 && iter->father_name == "0")
		{
			cJSON_AddItemToObject(rootElem, "name", cJSON_CreateString(iter->text_str.c_str()));
			root_name = iter->text_str;
			iter = vecNode.erase(iter);
			
		}
		else
			iter++;
		
			
	}

	/*char* temp =  cJSON_Print(rootElem);
	cout << string(temp) << endl;*/

	cJSON * temp_fatherElem = cJSON_CreateObject();
	temp_fatherElem = rootElem;
	if (!vecNode.empty())
	{
		
		vector<InfoGraph> children = findNextChildren(vecNode, root_name);
		addChildren(children, temp_fatherElem);
		temp_fatherElem = temp_fatherElem->child->next->child->child;
		root_name = temp_fatherElem->valuestring;
		

		
	}

	//cout << "!" << endl;
	
	char* temp = cJSON_Print(rootElem);
	cout << string(temp) << endl;

	cJSON_Print(rootElem);

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
	/*cJSON* root = cJSON_CreateObject();


	char*temjson = cJSON_Print(root);

	printf("%s\n", temjson);*/


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
	cout << endl;

	string result = "";

	result = Xml2Json(vecNode);

	system("pause");
	return 0;
}