#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setFixedSize(705,670);
    this->setWindowTitle("未连接(服务器不可用)");
    client = new QTcpSocket;
    connect(client, &QTcpSocket::readyRead, this, &MainWindow::recv_msg);
    connect(client, &QAbstractSocket::connected, this, &MainWindow::recv_connected_signal);
    connect(client, &QAbstractSocket::disconnected, this, &MainWindow::recv_disconnected_signal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::recv_connected_signal()
{
    QString title = "已连接 ";
    title += HOSTADD;
    title += " ";
    title += QString::number(HOSTPORT);
    this->setWindowTitle(title);
}

void MainWindow::recv_disconnected_signal()
{
    QString title = "未连接(服务器不可用)";
    this->setWindowTitle(title);
}

void MainWindow::recv_msg()
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QByteArray bitdata = client->readAll();
    QString msg = QString::fromLocal8Bit(bitdata);
    qDebug() << msg;
    dealMessageTime(time);
    ChatWidget* messageW = new ChatWidget(ui->listWidget->parentWidget());
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    dealMessage(messageW, item, msg, time, ChatWidget::User_She);
    ui->listWidget->scrollToBottom();
}

void MainWindow::on_connectButton_clicked()
{
    if (client->state() == QAbstractSocket::ConnectedState)
    {
        QMessageBox::information(this, tr("连接消息"),
                tr("当前已连接到服务器"),
                QMessageBox::Yes);
        return;
    }
    client->connectToHost(HOSTADD, HOSTPORT);
}


void MainWindow::on_sendButton_clicked()
{
    if (client->state() != QAbstractSocket::ConnectedState)
    {
        this->setWindowTitle(tr("未连接(服务器不可用)"));
        QMessageBox::information(this, tr("发送失败"), tr("服务器未连接"),
                                 QMessageBox::Yes);
        return ;
    }
    QString msg = ui->textEdit->toPlainText();
    ui->textEdit->setText("");
    qDebug() << msg;
    client->write(msg.toLocal8Bit());
    client->waitForBytesWritten();
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    if(!msg.isEmpty())
    {
        dealMessageTime(time);
        ChatWidget* messageW = new ChatWidget(ui->listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        dealMessage(messageW, item, msg, time, ChatWidget::User_Me);
    }
//    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    ui->listWidget->scrollToBottom();
}


void MainWindow::on_disconnectButton_clicked()
{
    if (client->state() != QAbstractSocket::ConnectedState)
    {
        this->setWindowTitle("未连接(服务器不可用)");
        return ;
    }
    this->setWindowTitle("未连接(服务器不可用)");
    QMessageBox::information(this, tr("已断开"), tr("服务器已断开"),
                             QMessageBox::Yes);
    client->close();
}


void MainWindow::on_cleanButton_clicked()
{
    ui->listWidget->clear();
}
//*******************************************************************************************************

void MainWindow::dealMessage(ChatWidget *messageW, QListWidgetItem *item, QString text, QString time,  ChatWidget::User_Type type)
{
    messageW->setFixedWidth(this->width());
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type);
    ui->listWidget->setItemWidget(item, messageW);
}

void MainWindow::dealMessageTime(QString curMsgTime)
{
    bool isShowTime = false;
    if(ui->listWidget->count() > 0)
    {
        QListWidgetItem* lastItem = ui->listWidget->item(ui->listWidget->count() - 1);
        ChatWidget* messageW = (ChatWidget*)ui->listWidget->itemWidget(lastItem);
        int lastTime = QDateTime::fromString(messageW->time(), "yyyy-MM-dd hh:mm:ss").toSecsSinceEpoch();
        int curTime = QDateTime::fromString(curMsgTime, "yyyy-MM-dd hh:mm:ss").toSecsSinceEpoch();
        qDebug() << "curTime lastTime:" << curTime << lastTime << this->whenTimeShow;
        isShowTime = ((curTime - lastTime) > 60 || \
                      (curTime - this->whenTimeShow) > 180); // 两个消息相差一分钟或者距离上次显示时间超过三分钟

    }
    else
    {
        isShowTime = true;
    }
    if(isShowTime)
    {
        this->whenTimeShow = QDateTime::currentSecsSinceEpoch();
        ChatWidget* messageTime = new ChatWidget(ui->listWidget->parentWidget());
        QListWidgetItem* itemTime = new QListWidgetItem(ui->listWidget);
        QSize size = QSize(this->width(), 40);
        messageTime->resize(size);
        itemTime->setSizeHint(size);
        messageTime->setText(curMsgTime, curMsgTime, size, ChatWidget::User_Time);
        ui->listWidget->setItemWidget(itemTime, messageTime);
    }
}
