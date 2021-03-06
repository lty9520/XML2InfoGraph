#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDir>
#include <QWebChannel>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9223");

    mView = new QWebEngineView(ui->widget);

    //QWebChannel * ch = new QWebChannel(this);
    //ch->registerObject("channel", (QObject*)channel::instance());
    //mView->page()->setWebChannel(ch);

    //qDebug() << html_path;
    //QString html_path = "file:///C:/Users/Administrator/Documents/EchartsDemo/res/tree-radial.html";
    QString html_applicationDirPath;
    //当前exe文件所在位置的地址
    html_applicationDirPath = QCoreApplication::applicationDirPath();
    QString html_path = html_applicationDirPath + "/res/tree-radial.html";


    mView->load(QUrl(html_path));
    mView->show();

    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(pushButton_clicked()));

    //ui->pushButton->click();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    mView->resize(this->size());
}

/*!
 * \brief MainWindow::addChildren   添加当前层的节点的子节点
 * \param children      当前节点的子节点组
 * \param fatherElem    当前节点，即当前父节点
 */
void MainWindow::addChildren(vector<InfoGraph>& children, cJSON * fatherElem)
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

/*!
 * \brief MainWindow::findNextChildren  寻找当前节点的所有子节点
 * \param originNode    当前节点组
 * \param father_name   当前节点的名字
 * \return  返回当前节点的子节点组
 */
vector<InfoGraph> MainWindow::findNextChildren(vector<InfoGraph>& originNode, string father_name)
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

/*!
 * \brief MainWindow::addNextChildren   添加当前节点的子节点的同级子节点
 * \param tempFatherElem    当前临时父节点
 * \param vecNode           当前子节点组
 * \param root_name         当前节点的名字
 */
void MainWindow::addNextChildren(cJSON * tempFatherElem, vector<InfoGraph>& vecNode, string root_name)
{

    cJSON * tempSubfatherElem = cJSON_CreateObject();

    while (!vecNode.empty() || tempFatherElem != NULL)
    {
        /**level one*/

        //vector<InfoGraph> children = findNextChildren(vecNode, root_name);
        //if(!children.empty())
        //{
        //    addChildren(children, temp_fatherElem);
        //    /**level one; children part*/
        //    temp_fatherElem = temp_fatherElem->child->next->child;
        //    root_name = temp_fatherElem->child->valuestring;
        //}

        /**next level(children)*/
        tempSubfatherElem = tempFatherElem;
        while(tempFatherElem != NULL)
        {
            vector<InfoGraph> children = findNextChildren(vecNode, root_name);
            if(!children.empty())
            {
                addChildren(children, tempFatherElem);

                tempFatherElem = tempFatherElem->child->next->child;
                addNextChildren(tempFatherElem, vecNode, tempFatherElem->child->valuestring);

                tempFatherElem = tempSubfatherElem;
                tempFatherElem = tempFatherElem->next;
                tempSubfatherElem = tempFatherElem;
                if(tempFatherElem != NULL)
                {
                   root_name = tempFatherElem->child->valuestring;
                }
                else
                {
                    return;
                }
            }
            else
            {
                tempFatherElem = tempFatherElem->next;
                if(tempFatherElem != NULL)
                {
                   root_name = tempFatherElem->child->valuestring;
                }
                else
                {
                    return;
                }
            }

        }
        //tempFatherElem = tempSubfatherElem;
        //tempFatherElem = tempFatherElem->child->next->child;
        //if(tempFatherElem != NULL)
        //{
        //   root_name = tempFatherElem->child->valuestring;
        //}
        //else
        //{
        //    continue;
        //}
        //root_name = temp_fatherElem->child->valuestring;






        /*
        if (temp_fatherElem->child->next->child->next == NULL)
        {
            vector<InfoGraph> children = findNextChildren(vecNode, root_name);
            addChildren(children, temp_fatherElem);

            temp_fatherElem = temp_fatherElem->child->next->child;
            root_name = temp_fatherElem->child->valuestring;
        }
        else
        {
            vector<InfoGraph> children = findNextChildren(vecNode, root_name);
            addChildren(children, temp_fatherElem);

            temp_fatherElem = temp_fatherElem->child->next->child->next;
            root_name = temp_fatherElem->child->valuestring;
        }
        */






    }
}

/*!
 * \brief MainWindow::Xml2Json  将解析XML文件得到的结果转换为需要的Json格式数据
 * \param vecNode   解析获得的节点组
 * \return  返回所得的Json格式
 */
QString MainWindow::Xml2Json(vector<InfoGraph>& vecNode)

{
    QString strJson;

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
    //cJSON * temp_subfatherElem = cJSON_CreateObject();
    vector<InfoGraph> root_children = findNextChildren(vecNode, root_name);
    addChildren(root_children, rootElem);
    /**root level; children part*/
    temp_fatherElem = rootElem->child->next->child;
    root_name = temp_fatherElem->child->valuestring;

    addNextChildren(temp_fatherElem, vecNode, root_name);

    /*
    while (!vecNode.empty())
    {
        //level one

        //vector<InfoGraph> children = findNextChildren(vecNode, root_name);
        //if(!children.empty())
        //{
        //    addChildren(children, temp_fatherElem);
        //    //level one; children part
        //    temp_fatherElem = temp_fatherElem->child->next->child;
        //    root_name = temp_fatherElem->child->valuestring;
        //}

        //next level(children)
        temp_subfatherElem = temp_fatherElem;
        while(temp_fatherElem != NULL)
        {
            vector<InfoGraph> children = findNextChildren(vecNode, root_name);
            addChildren(children, temp_fatherElem);



            temp_fatherElem = temp_fatherElem->next;
            if(temp_fatherElem != NULL)
            {
               root_name = temp_fatherElem->child->valuestring;
            }
            else
            {
                continue;
            }

        }
        temp_fatherElem = temp_subfatherElem;
        temp_fatherElem = temp_fatherElem->child->next->child;
        if(temp_fatherElem != NULL)
        {
           root_name = temp_fatherElem->child->valuestring;
        }
        else
        {
            continue;
        }
        //root_name = temp_fatherElem->child->valuestring;







        if (temp_fatherElem->child->next->child->next == NULL)
        {
            vector<InfoGraph> children = findNextChildren(vecNode, root_name);
            addChildren(children, temp_fatherElem);

            temp_fatherElem = temp_fatherElem->child->next->child;
            root_name = temp_fatherElem->child->valuestring;
        }
        else
        {
            vector<InfoGraph> children = findNextChildren(vecNode, root_name);
            addChildren(children, temp_fatherElem);

            temp_fatherElem = temp_fatherElem->child->next->child->next;
            root_name = temp_fatherElem->child->valuestring;
        }







    }
    */
    //cout << "!" << endl;

    /*char* temp = cJSON_Print(rootElem);
    cout << string(temp) << endl;*/

    //cJSON_Print(rootElem);

    char * result_char = cJSON_Print(rootElem);

    //cJSON_Delete(rootElem);
    //cJSON_Delete(temp_fatherElem);


    strJson =  QString(QLatin1String(result_char));

    return strJson;

}

/*!
 * \brief MainWindow::ReadParaXml   读取XML文件并解析所需要的数据
 * \param m_strXmlPath  XMl文件路径
 * \param vecNode       解析结果结构体组
 */
void MainWindow::ReadParaXml(string m_strXmlPath, vector<InfoGraph>& vecNode)
{
    //qDebug << QString::fromStdString("1");
    //qDebug() << QString::fromStdString(m_strXmlPath);
    //初始化一個存放結果的結構體
    InfoGraph *pNode = new InfoGraph;

    //读取xml文件的文档
    TiXmlDocument* Document = new TiXmlDocument();


    if (!Document->LoadFile(m_strXmlPath.c_str()))
    {
        QMessageBox::information(NULL, "Error", "Can not open file.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        QApplication::exit(0);
    }
    //Document->Print();
    //根目录--<VisioDocument>
    TiXmlElement* RootElement = Document->RootElement();
    //检测根元素存在性
    if (RootElement == NULL)
    {
        QMessageBox::information(NULL, "Error", "Failed to load file: No root element.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

        Document->Clear();
        QApplication::exit(0);
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

    QMessageBox::information(NULL, "Success", "Finished Reading XML file.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

    //cout << "读取xml完成" << endl;

}


void MainWindow::pushButton_clicked()
{
    //string xml_path = "C:/Users/Administrator/Documents/EchartsDemo/res/xml/data.xml";
    QString filePath;
    filePath = QFileDialog::getOpenFileName(this, "请选择所需要的XML文件", "/", "XML文件(*.xml)");
    //xml_applicationDirPath = QCoreApplication::applicationDirPath();
    string xml_path = filePath.toStdString();


    //QString xml_path = "file:///" + QDir::currentPath() + xml_filename;
    vector<InfoGraph> vecNode;

    ReadParaXml(xml_path, vecNode);

    QString result_Json = "";

    result_Json = Xml2Json(vecNode);

    // 把json字符串中所有双引号前面再加上一个转义符
    // 因为这个字符串到了js里面还要被parseJSON一次，所以\\不可少
    //result_Json.replace(QRegExp("\""), "\\\"");
    //{\n\t\"name\":\t\"120\",\n\t\"children\":\t[{\n\t\t\t\"name\":\t\"you\",\n\t\t\t\"children\":\t[{\n\t\t\t\t\t\"name\":\t\"1\"\n\t\t\t\t}, {\n\t\t\t\t\t\"name\":\t\"2\"\n\t\t\t\t}, {\n\t\t\t\t\t\"name\":\t\"3\"\n\t\t\t\t}]\n\t\t}, {\n\t\t\t\"name\":\t\"he\"\n\t\t}, {\n\t\t\t\"name\":\t\"me\"\n\t\t}]\n}
    //QString test = "{\"name\":\"120\",\"children\":[{\"name\":\"you\",\"children\":[{\"name\":\"1\"}, {\"name\":\"2\"}, {\"name\":\"3\"}]}, {\"name\":\"he\"}, {\"name\":\"me\"}]}";

    // callfromqt是js中的函数名 str是其入参
    QString strJSON = QString("callfromqt(%1);").arg(result_Json);
    qDebug() << strJSON;


    QMessageBox::information(NULL, "Success", "Start JS Phase.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    // 调用js中的函数
    mView->page()->runJavaScript(strJSON);



}
