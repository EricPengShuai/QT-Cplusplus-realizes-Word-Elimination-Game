#include "mainwindow.h"
#include "ui_mainwindow.h"

int TIME = 3000;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->answer->setEnabled(false); //时间未到无法答题
    //tcpsocket->write(QString(player.name + ' '+ "label").toUtf8().data());
    table = NULL;
    //connect(table,&QTableWidget::destroyed,[&] { qDebug()<<"NULL"; table = NULL; });
    //改变用户名，并且更换为初始信息
    ui->user->setText(player.name);
    ui->count->setText(QString::number(player.counted));
    ui->progressBar->setValue(player.exp);
    ui->level->setText(QString::number(player.level));
}

MainWindow::~MainWindow()
{
    delete m_pTimer1;
    delete m_pTime;
    delete table;
    delete ui;
    deleteLater();
}
//计时器
void MainWindow::Mytimer()
{
    m_pTimer1 = new QTimer(this);
    m_pTime = new QTime();
    connect(m_pTimer1, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    //设置初始单词
    QString msg = "request_word 0";
    tcpsocket->write(msg.toUtf8().data());

    //让它停在这里几分钟(程序还在进行)Sleep好像不行
    QEventLoop eventloop1;
    QTimer::singleShot(500,&eventloop1,SLOT(quit()));
    eventloop1.exec(0);

    strlist_word = words.split(' ');
    strlist_word.removeAt(strlist_word.count() - 1);

    ans_word = strlist_word.at(player.counted);
    ui->label->setText(ans_word);
    m_pTimer1->start(TIME);
}
//超时函数处理单词消失
void MainWindow::handleTimeout()
{
    if(m_pTimer1->isActive())
    {
        ui->label->hide();
        m_pTimer1->stop();
        ui->answer->setEnabled(true);
        m_pTime->start();
    }
}

//提交答案按钮
void MainWindow::on_submit_clicked()
{
    //每次都重新获取单词，以免词库中有新的单词
    QString msg = "request_word 1";
    tcpsocket->write(msg.toUtf8().data());
    QEventLoop eventloop;
    QTimer::singleShot(500,&eventloop,SLOT(quit()));
    eventloop.exec(0);
    strlist_word = words.split(" ");
    strlist_word.removeAt(strlist_word.count() - 1);

    if(ui->answer->text() == ans_word)  //回答正确
    {
        ++(player.counted);
        TIME -= 20;//每答一题就会减少显示时间
        int time = m_pTime->elapsed();
        //更新玩家信息
        ui->count->setText(QString::number(player.counted));
        //进度条考虑一下**************************
        //经验值策略
        if(time/1000 >= 5) player.Add_exp(5,ans_word.length()/5);
        else player.Add_exp(time/1000,ans_word.length()/5);
        ui->progressBar->setValue(player.exp);
        //升级策略
        player.Level_up();
        ui->level->setText(QString::number(player.level));
        //*****************************
        msg = "update_player " + player.name +' '+ QString::number(player.level)
            +' '+ QString::number(player.exp) +' '+ QString::number(player.counted);
        tcpsocket->write(msg.toUtf8().data());  //答对一题更新一次
        //*****************************
        //答对之后更换单词
        ans_word = strlist_word.at(player.counted);
        ui->label->setText(ans_word);
        ui->label->show();
        m_pTimer1->start(TIME);
        ui->answer->clear();
    }
    else    //回答错误
    {
        msg = "player_quit " + player.name;
        tcpsocket->write(msg.toUtf8().data());
        //critical信息框需要点击之后才能返回
        QMessageBox::critical(NULL,"提示","GAME OVER");
        close();//最后要结束游戏
    }
    ui->answer->setEnabled(false);
}
//退出游戏
void MainWindow::on_Quit_clicked()
{
    QString msg = "player_quit " + player.name;
    tcpsocket->write(msg.toUtf8().data());
    QEventLoop eventloop;
    QTimer::singleShot(500,&eventloop,SLOT(quit()));
    eventloop.exec(0);
    close();
}
//查询玩家信息
void MainWindow::on_pushButton_player_clicked()
{
    //获取玩家信息
    QString str = "check_player";
    tcpsocket->write(str.toUtf8().data());

    //程序要等一会才能得到新的all_data
    QEventLoop eventloop;
    QTimer::singleShot(500,&eventloop,SLOT(quit()));
    eventloop.exec(0);

    QStringList strList = all_data.split("\n");
    strList.removeAt(strList.count() - 1);
    qDebug() << strList;

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
    int j = 0;//每列标号
    for(int i = 0; i < strList.count(); i++)
    {
        QString str_row = strList.at(i);
        QStringList element = str_row.split(" ");//拆分成每个小段信息

        QTableWidgetItem *item_level= new QTableWidgetItem;
        QTableWidgetItem *item_exp= new QTableWidgetItem;
        QTableWidgetItem *item_count= new QTableWidgetItem;
        QTableWidgetItem *item_land= new QTableWidgetItem;

        table->setItem(i,j,new QTableWidgetItem(element.at(0)));j++;
        //以下要将读取的字符串转化为数字后再填入表格
        item_level->setData(Qt::DisplayRole,(element.at(1)).toInt());
        table->setItem(i,j,item_level);j++;
        item_exp->setData(Qt::DisplayRole,(element.at(2)).toInt());
        table->setItem(i,j,item_exp);j++;
        item_count->setData(Qt::DisplayRole,(element.at(3)).toInt());
        table->setItem(i,j,item_count);j++;
        item_land->setData(Qt::DisplayRole,(element.at(4)).toInt());
        table->setItem(i,j,item_land);
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
//查询出题者信息
void MainWindow::on_pushButton_tester_clicked()
{
    //获取出题者信息
    QString str = "check_tester";
    tcpsocket->write(str.toUtf8().data());
    //程序要等一会才能得到新的all_data
    QEventLoop eventloop;
    QTimer::singleShot(800,&eventloop,SLOT(quit()));
    eventloop.exec(0);

    QStringList strList = all_data.split('\n');
    strList.removeAt(strList.count() - 1);
    qDebug() << strList;
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
    int j=0;//每列标号
    for(int i = 0; i < strList.count(); i++)
    {
        QString str_row = strList.at(i);
        QStringList element = str_row.split(" ");//拆分成每个小段信息

        QTableWidgetItem *item_level= new QTableWidgetItem;
        QTableWidgetItem *item_count= new QTableWidgetItem;
        QTableWidgetItem *item_land= new QTableWidgetItem;

        table->setItem(i,j,new QTableWidgetItem(element.at(0)));j++;
        //以下要将读取的字符串转化为数字后再填入表格
        item_level->setData(Qt::DisplayRole,(element.at(1)).toInt());
        table->setItem(i,j,item_level);j++;
        item_count->setData(Qt::DisplayRole,(element.at(2)).toInt());
        table->setItem(i,j,item_count);j++;
        item_land->setData(Qt::DisplayRole,(element.at(3)).toInt());
        table->setItem(i,j,item_land);
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
void MainWindow::ScreenInfo(QTableWidget *source, QString text)
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
void MainWindow::on_pushButton_index_clicked()
{
    if(table == NULL)
    {
        QMessageBox::information(this,"Tip","请先打开表格");
    }
    else
    {
        ScreenInfo(table,ui->lineEdit->text());
        //ui->lineEdit->clear();
    }
}
//battle 在线玩家
void MainWindow::on_battle_clicked()
{
    QString msg = player.name + " battle " + ui->battlename->text();
    tcpsocket->write(msg.toUtf8().data());
}
//是否发起battle
void MainWindow::make_battle(QString info)
{
    qDebug() << info;
    QStringList str_info = info.split(' ');
    if(str_info.at(2) == "battling")
    {
        if(QMessageBox::Yes == QMessageBox::information(this,"Battle", info, QMessageBox::Yes |QMessageBox::No))
        {
            qDebug() << "yes";
            hide();
            battle = new Battle(player.name, str_info.at(3));
            battle->show(); //控制battle的信息槽
            connect(this, SIGNAL(ready_signal()), battle, SLOT(ready()));//battle就绪信号槽
            connect(this, SIGNAL(v_signal()), battle, SLOT(victory()));//battle胜利信号槽
            connect(this, SIGNAL(f_signal()), battle, SLOT(fail()));//battle失败信号槽
            connect(battle, SIGNAL(exit_signal()), this, SLOT(close()));//battle退出信号槽
            connect(this, SIGNAL(quit_signal()), battle, SLOT(quit_info()));//管理battle双方信号槽
        }
        else
        {
            qDebug() << "no";
        }
    }
}
//是否接受battle
void MainWindow::accept_battle(QString info)
{
    qDebug() << info;
    QStringList str_info = info.split(' ');
    if(str_info.at(2) == "accepting")
    {
        if(QMessageBox::Yes == QMessageBox::information(this,"Battle", info, QMessageBox::Yes |QMessageBox::No))
        {
            qDebug() << "yes";
            hide();
            battle = new Battle(str_info.at(3), player.name);
            battle->show();
            connect(this, SIGNAL(ready_signal()), battle, SLOT(ready()));
            connect(this, SIGNAL(v_signal()), battle, SLOT(victory()));
            connect(this, SIGNAL(f_signal()), battle, SLOT(fail()));
            connect(battle, SIGNAL(exit_signal()), this, SLOT(close()));
            connect(this, SIGNAL(quit_signal()), battle, SLOT(quit_info()));
        }
        else
        {
            qDebug() << "no";
        }
    }
}

void MainWindow::ready_make()
{
    emit ready_signal();
}

void MainWindow::victory_make()
{
    emit v_signal();
}

void MainWindow::fail_make()
{
    emit f_signal();
}

void MainWindow::quit_make()
{
    emit quit_signal();
}

void MainWindow::cannot_battle()
{
    QMessageBox::critical(this,"Tip", "对方不在线或者用户名不存在", QMessageBox::Yes |QMessageBox::No);
}
