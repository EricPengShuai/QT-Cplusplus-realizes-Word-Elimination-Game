#include "battle.h"
#include "ui_battle.h"

extern int TIME;

Battle::Battle(QString plA, QString plB, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Battle)
{
    ui->setupUi(this);
    ui->answer->setEnabled(false);  //时间未到无法答题
    ui->playerA->setText(plA);
    ui->playerB->setText(plB);
    ui->lcd->setText("0");
    ui->user->setText(player.name);
    ui->count->setText(QString::number(player.counted));
    ui->progressBar->setValue(player.exp);
    ui->level->setText(QString::number(player.level));
    Time = new QTime();
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(handleout()));
}

Battle::~Battle()
{
    delete ui;
    delete Time;
    delete m_pTimer;
    deleteLater();
}
//单词出现和消失的槽函数
void Battle::handleout()
{
    if(m_pTimer->isActive())
    {
        ui->word->hide();
        m_pTimer->stop();
        ui->answer->setEnabled(true);
        Time->start();
    }
}
//准备就绪按钮
void Battle::on_ready_clicked()
{
    QString msg = "ready "+ player.name;
    tcpsocket->write(msg.toUtf8().data());
    ui->ready->setEnabled(false);
    ui->answer->setEnabled(false);
}
//准备就绪
void Battle::ready()
{
    make_word();
    ui->word->show();
    ui->lcd->setText("0");
}
//产生单词
void Battle::make_word()
{
    QStringList strlist_word = words.split(' ');
    strlist_word.removeAt(strlist_word.count() - 1);

    srand(time(NULL));
    int i = rand() % strlist_word.count();  //随机产生数
    qDebug() << strlist_word.at(i) << i;
    QString ans_word = strlist_word.at(i);
    ui->word->setText(ans_word);
    m_pTimer->start(TIME);
}
//答题按钮
void Battle::on_submit_clicked()
{
    if(ui->ready->isEnabled() == true)
    {
        QMessageBox::information(this,"Info", "请先点击准备按钮！", QMessageBox::Yes |QMessageBox::No);
        return;
    }
    t = Time->elapsed();
    if(ui->answer->text() == ui->word->text())
    {
        ui->lcd->setText(QString::number(t));   //显示答题所用时间
        QString msg = "battle_res " + player.name +' ' + QString::number(t);
        tcpsocket->write(msg.toUtf8().data());
    }
    else
    {
        t = RAND_MAX;
        qDebug() << t;
        ui->lcd->setText(QString::number(t));   //显示答题所用时间
        QString msg = "battle_res " + player.name +' ' + QString::number(t);
        tcpsocket->write(msg.toUtf8().data());
    }
    ui->answer->clear();
}
//胜利信息
void Battle::victory()
{
    ++(player.counted);
    //更新玩家信息
    ui->count->setText(QString::number(player.counted));
    //经验值策略,增加较多--5
    if(t/1000 >= 5)
        player.Add_exp(5, 5);
    else
        player.Add_exp(t/1000, 5);
    ui->progressBar->setValue(player.exp);
    //升级策略
    player.Level_up();
    ui->level->setText(QString::number(player.level));
    //*****************************
    QString msg = "update_player " + player.name +' '+ QString::number(player.level)
        +' '+ QString::number(player.exp) +' '+ QString::number(player.counted);
    tcpsocket->write(msg.toUtf8().data());  //答对一题更新一次
    //*****************************
    m_pTimer->start(TIME);

    //停一会？然后开始
    ui->ready->setEnabled(true);
}
//失败信息
void Battle::fail()
{
    //更新玩家信息
    ui->count->setText(QString::number(player.counted));
    //经验值策略,输一次减去5
    player.Rem_exp(5, 5);
    ui->progressBar->setValue(player.exp);
    //升级策略
    player.Level_up();
    ui->level->setText(QString::number(player.level));
    //*****************************
    QString msg = "update_player " + player.name +' '+ QString::number(player.level)
        +' '+ QString::number(player.exp) +' '+ QString::number(player.counted);
    tcpsocket->write(msg.toUtf8().data());  //答错一题更新一次
    //*****************************
    m_pTimer->start(TIME);

    //停一会？然后开始
    ui->ready->setEnabled(true);
}
//结束battle
void Battle::on_Quit_clicked()
{
    QString name_str = "";
    if(player.name == ui->playerA->text())
        name_str = ui->playerB->text();
    else
        name_str = ui->playerA->text();
    QString msg = "battle_quit " + player.name +' '+ name_str;
    tcpsocket->write(msg.toUtf8().data());
    QEventLoop eventloop;
    QTimer::singleShot(500,&eventloop,SLOT(quit()));
    eventloop.exec(0);
    emit exit_signal();
    qDebug() << "quit click";
    close();
}
//退出battle
void Battle::quit_info()
{
    if(QMessageBox::Yes == QMessageBox::critical(this,"Quit", "对方已经退出,您无法继续游戏，请退出!", QMessageBox::Yes |QMessageBox::No))
    {
        qDebug() << "quit info";
        QString msg = "player_quit " + player.name;
        tcpsocket->write(msg.toUtf8().data());
        QEventLoop eventloop;
        QTimer::singleShot(500,&eventloop,SLOT(quit()));
        eventloop.exec(0);
        emit exit_signal();
        close();
    }
}
