#include "mainwindow_test.h"
#include "ui_mainwindow_test.h"

MainWindow_test::MainWindow_test(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow_test)
{
    ui->setupUi(this);
    //设置初始化信息
    table = NULL;
    ui->name->setText(tester.name);
    ui->count->setText(QString::number(tester.counted));
    ui->level->setText(QString::number(tester.level));
}

MainWindow_test::~MainWindow_test()
{
    this->deleteLater();
    delete table;
    delete ui;
}
//提交单词按钮
void MainWindow_test::on_submit_clicked()
{
    QString text = ui->lineEdit->text();
    if(text == "")
        QMessageBox::critical(this,tr("提示"),tr("请输入单词"));
    else
    {
        QString msg = "submit_word " + text;
        tcpsocket->write(msg.toUtf8().data());

        //让它停在这里几分钟(程序还在进行)，得到更新后的submit
        QEventLoop eventloop1;
        QTimer::singleShot(500,&eventloop1,SLOT(quit()));
        eventloop1.exec(0);

        if(submit == 1) //单词不存在就要更新词库
        {
            ++(tester.counted);
            tester.Level_up();//出题者升级策略
            ui->count->setText(QString::number(tester.counted));
            ui->level->setText(QString::number(tester.level));
            //*******************************
            msg = "update_tester " + tester.name +' '+QString::number(tester.counted)
                    +' '+QString::number(tester.level);
            tcpsocket->write(msg.toUtf8().data());  //出题成功就更新一次
            //*******************************
            ui->lineEdit->clear();
            submit = -1;
        }
        else if(submit == 0)
            QMessageBox::information(this,tr("提示"),tr("单词已存在"));
    }
}

//退出游戏
void MainWindow_test::on_quit_clicked()
{
    QString msg = "tester_quit " + tester.name;
    tcpsocket->write(msg.toUtf8().data());
    QEventLoop eventloop1;
    QTimer::singleShot(500,&eventloop1,SLOT(quit()));
    eventloop1.exec(0);
    close();
}
//查询玩家信息
void MainWindow_test::on_pushButton_player_clicked()
{
    //获取玩家信息
    QString str = "check_player";
    tcpsocket->write(str.toUtf8().data());

    //程序要等一会才能得到新的all_data
    QEventLoop eventloop;
    QTimer::singleShot(600,&eventloop,SLOT(quit()));
    eventloop.exec(0);

    QStringList strList = all_data.split("\n");
    strList.removeAt(strList.count() - 1);
    //表格外观
    table = new QTableWidget;
    table->setColumnCount(5);
    table->setRowCount(strList.count());
    table->setWindowTitle("Player Information");
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);//只可读
    table->setShowGrid(true);//显示格子线
    //table->verticalHeader()->setVisible(false); //隐藏列表头
    //table->horizontalHeader()->setVisible(false); //隐藏行表头
    //表格表头
    QStringList header;
    header <<"玩家"<<"等级"<<"经验值"<<"已经闯关数"<<"is_land";
    table->setHorizontalHeaderLabels(header);
    //表格内容
    int j=0;//每列标号
    for(int i = 0; i < strList.count(); i++)
    {
        QString str_row = strList.at(i);
        QStringList element = str_row.split(" ");//拆分成每个小段信息

        QTableWidgetItem *item_level= new QTableWidgetItem;
        QTableWidgetItem *item_exp= new QTableWidgetItem;
        QTableWidgetItem *item_count= new QTableWidgetItem;
        QTableWidgetItem *item_land= new QTableWidgetItem;

        table->setItem(i, j, new QTableWidgetItem(element.at(0)));j++;
        //以下要将读取的字符串转化为数字后再填入表格
        item_level->setData(Qt::DisplayRole,(element.at(1)).toInt());
        table->setItem(i, j, item_level);j++;
        item_exp->setData(Qt::DisplayRole,(element.at(2)).toInt());
        table->setItem(i, j, item_exp);j++;
        item_count->setData(Qt::DisplayRole,(element.at(3)).toInt());
        table->setItem(i, j, item_count);j++;
        item_land->setData(Qt::DisplayRole,(element.at(4)).toInt());
        table->setItem(i, j, item_land);
        j = 0;
    }
    table->show();
    //点击进行排序操作
    QHeaderView *headerGood1 = table->horizontalHeader();
    //SortIndicator为水平标题栏文字旁边的三角指示器
    headerGood1->setSortIndicator(1, Qt::AscendingOrder);
    headerGood1->setSortIndicatorShown(true);
    connect(headerGood1,SIGNAL(sectionDoubleClicked(int)),table,SLOT(sortByColumn(int)));
}
//查询出题者信息
void MainWindow_test::on_pushButton_tester_clicked()
{
    //Update();
    //获取出题者信息
    QString str = "check_tester";
    tcpsocket->write(str.toUtf8().data());

    //程序要等一会才能得到新的all_data
    QEventLoop eventloop;
    QTimer::singleShot(600,&eventloop,SLOT(quit()));
    eventloop.exec(0);

    QStringList strList = all_data.split("\n");
    strList.removeAt(strList.count() - 1);
    //表格外观
    table = new QTableWidget;
    table->setColumnCount(4);
    table->setRowCount(strList.count());
    table->setWindowTitle("Tester Information");
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);//只可读
    table->setShowGrid(true);//显示格子线
    //table->verticalHeader()->setVisible(false); //隐藏列表头
    //table->horizontalHeader()->setVisible(false); //隐藏行表头
    //表格表头
    QStringList header;
    header <<"出题者"<<"等级"<<"已经出题数"<<"is_land";
    table->setHorizontalHeaderLabels(header);
    //表格内容
    int j = 0;//每列标号
    for(int i = 0; i < strList.count(); i++)
    {
        QString str_row = strList.at(i);
        QStringList element = str_row.split(" ");//拆分成每个小段信息

        QTableWidgetItem *item_level= new QTableWidgetItem;
        QTableWidgetItem *item_count= new QTableWidgetItem;
        QTableWidgetItem *item_land= new QTableWidgetItem;

        table->setItem(i, j, new QTableWidgetItem(element.at(0)));j++;
        //以下要将读取的字符串转化为数字后再填入表格
        item_level->setData(Qt::DisplayRole,(element.at(1)).toInt());
        table->setItem(i, j, item_level);j++;
        item_count->setData(Qt::DisplayRole,(element.at(2)).toInt());
        table->setItem(i, j, item_count);j++;
        item_land->setData(Qt::DisplayRole,(element.at(3)).toInt());
        table->setItem(i, j, item_land);
        j=0;
    }
    table->show();
    //点击进行排序操作
    QHeaderView *headerGood1 = table->horizontalHeader();
    //SortIndicator为水平标题栏文字旁边的三角指示器
    headerGood1->setSortIndicator(1, Qt::AscendingOrder);
    headerGood1->setSortIndicatorShown(true);
    connect(headerGood1,SIGNAL(sectionDoubleClicked(int)),table,SLOT(sortByColumn(int)));
}
//索引查找
void MainWindow_test::ScreenInfo(QTableWidget *source, QString text)
{
    int rC = source->rowCount();//获得行号
    if ("" == text)//判断输入是否为空
    {
        for (int i = 0; i < rC; i++)
        {
            source->setRowHidden(i, false);//显示所有行
        }
    }
    else
    {
        //获取符合条件的cell索引
        QList <QTableWidgetItem *> item = source->findItems(text, Qt::MatchContains);

        for (int i = 0; i < rC; i++)
        {
            source->setRowHidden(i, true);//隐藏所有行
        }

        if (!item.isEmpty())//不为空
        {
            for (int i = 0; i < item.count(); i++)
            {
                source->setRowHidden(item.at(i)->row(),false);//item.at(i).row()输出行号
            }
        }
    }
}
//索引查找按钮
void MainWindow_test::on_pushButton_index_clicked()
{
    if(table == NULL)
    {
        QMessageBox::information(this,"Tip","请先打开表格");
    }
    else
    {
        ScreenInfo(table,ui->index->text());
        ui->index->clear();
        //table = NULL;
    }
}
