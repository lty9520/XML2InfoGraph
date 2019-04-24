#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QPushButton>
#include <QString>

#include <vector>
#include "tinystr.h"
#include "tinyxml.h"
#include "cJSON.h"

#include <string>
#include <stdio.h>

using namespace std;

//存放xml文件的读取结果的结构体
/*!
 * \brief The InfoGraph struct
 * type:        the type of this node (Name|NameU in xml)
 * text_str:    the valuestring of this node (name)
 * father_name: the father name of this node
 */
struct InfoGraph
{
    string type;
    string text_str;
    string father_name;
};


//Q_DECLARE_METATYPE(InfoGraph)



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    //参数结构体


    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //自适应窗体窗口
    void resizeEvent(QResizeEvent *event);
    //添加当前层的节点的下一层的子节点（只包含子节点的首层，name层）
    void addChildren(vector<InfoGraph>& children, cJSON * fatherElem);
    //寻找当前节点的所有子节点
    vector<InfoGraph> findNextChildren(vector<InfoGraph>& originNode, string father_name);
    //添加当前层节点的下一层子节点的第二节点（只能添加next层，子节点的下一层）
    void addNextChildren(cJSON * tempFatherElem, vector<InfoGraph>& vecNode, string root_name);
    //将XML文件解析得到的结果转换成需要的Json格式数据
    QString Xml2Json(vector<InfoGraph>& vecNode);
    //读取XML文件并解析所需要的部分
    void ReadParaXml(string m_strXmlPath, vector<InfoGraph>& vecNode);

private slots:
    void pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QWebEngineView * mView;

    //QPushButton * pushButton;
};

#endif // MAINWINDOW_H
