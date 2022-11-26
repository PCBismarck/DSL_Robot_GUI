#include "ChatWidget.h"
#include <QFontMetrics>
#include <QPaintEvent>
#include <QDateTime>
#include <QPainter>
#include <QMovie>
#include <QLabel>
#include <QDebug>

ChatWidget::ChatWidget(QWidget *parent) : QWidget(parent)
{
    QFont te_font = this->font();
    te_font.setFamily("MicrosoftYaHei");
//    te_font.setFamily("Source Code Pro");
    te_font.setPointSize(12);
    this->setFont(te_font);
    m_leftPixmap = QPixmap(":/img/duolaRight.gif");
    m_rightPixmap = QPixmap(":/img/duolaLeft.gif");

}


void ChatWidget::setText(QString text, QString time, QSize allSize, ChatWidget::User_Type userType)
{
    m_msg = text;
    m_userType = userType;
    m_time = time;
    m_curTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_allSize = allSize;
    this->update();
}

QSize ChatWidget::fontRect(QString str)
{
    m_msg = str;
    int minHei = 30;
    int iconWH = 40;
    int iconSpaceW = 20;
    int iconRectW = 5;
    int iconTMPH = 10;
    int TriangleW = 6;
    int FrameTMP = 20;
    int textSpaceRect = 12;
    m_FrameWidth = this->width() - FrameTMP - 2*(iconWH+iconSpaceW+iconRectW);
    m_textWidth = m_FrameWidth - 2*textSpaceRect;
    m_spaceWid = this->width() - m_textWidth;
    m_iconLeftRect = QRect(iconSpaceW, iconTMPH, iconWH, iconWH);
    m_iconRightRect = QRect(this->width() - iconSpaceW - iconWH, iconTMPH, iconWH, iconWH);

    QSize size = getRealString(m_msg); // 整个的size

    qDebug() << "fontRect Size:" << size;
    int height = size.height() < minHei ? minHei : size.height();

    m_TriangleLeftRect = QRect(iconWH+iconSpaceW+iconRectW, m_lineHeight/2, TriangleW, height - m_lineHeight);
    m_TriangleRightRect = QRect(this->width() - iconRectW - iconWH - iconSpaceW - TriangleW, m_lineHeight/2, TriangleW, height - m_lineHeight);

    if(size.width() < (m_textWidth+m_spaceWid))
    {
        m_FrameLeftRect.setRect(m_TriangleLeftRect.x()+m_TriangleLeftRect.width(), m_lineHeight/4*3, size.width()-m_spaceWid+2*textSpaceRect, height-m_lineHeight);
        m_FrameRightRect.setRect(this->width() - size.width() + m_spaceWid - 2*textSpaceRect - iconWH - iconSpaceW - iconRectW - TriangleW,
                                 m_lineHeight/4*3, size.width()-m_spaceWid+2*textSpaceRect, height-m_lineHeight);
    }
    else
    {
        m_FrameLeftRect.setRect(m_TriangleLeftRect.x()+m_TriangleLeftRect.width(), m_lineHeight/4*3, m_FrameWidth, height-m_lineHeight);
        m_FrameRightRect.setRect(iconWH + FrameTMP + iconSpaceW + iconRectW - TriangleW, m_lineHeight/4*3, m_FrameWidth, height-m_lineHeight);
    }
    m_textLeftRect.setRect(m_FrameLeftRect.x()+textSpaceRect,m_FrameLeftRect.y()+iconTMPH,
                           m_FrameLeftRect.width()-2*textSpaceRect,m_FrameLeftRect.height()-2*iconTMPH);
    m_textRightRect.setRect(m_FrameRightRect.x()+textSpaceRect,m_FrameRightRect.y()+iconTMPH,
                            m_FrameRightRect.width()-2*textSpaceRect,m_FrameRightRect.height()-2*iconTMPH);

    return QSize(size.width(), height);
}

QSize ChatWidget::getRealString(QString src)
{
    qDebug() << src;
    QFontMetricsF fm(this->font());
    m_lineHeight = fm.lineSpacing();
    int nCount = src.count("\n");
    int nMaxWidth = 0;
    nMaxWidth = fm.boundingRect(src).width()+2;
    if(nCount == 0)
    {
        QString value = src;
        if(nMaxWidth > m_textWidth)
        {
            nMaxWidth = m_textWidth;
            int size = m_textWidth / (fm.boundingRect(src).width()+2);
            int num = (fm.boundingRect(src).width()+2) / m_textWidth;
//            num = ( fm.boundingRect(value).width()) / m_textWidth;
            nCount += num;
            QString temp = "";
            for(int i = 0; i < num; i++)
            {
                temp += value.mid(i*size, (i+1)*size) + "\n";
            }
            src.replace(value, temp);
            qDebug() << temp;
        }
    }
    else
    {
        for(int i = 0; i < (nCount + 1); i++)
        {
            QString value = src.split("\n").at(i);
            nMaxWidth = (fm.boundingRect(value).width()+2) > nMaxWidth ? (fm.boundingRect(value).width()+2) : nMaxWidth;
            if(fm.boundingRect(value).width() > m_textWidth)
            {
                nMaxWidth = m_textWidth;
                int size = m_textWidth / (fm.boundingRect(value).width()+2);
                int num = (fm.boundingRect(value).width()+2) / m_textWidth;
                num = ((i+num)*fm.boundingRect(value).width() + fm.boundingRect(value).width()) / m_textWidth;
                nCount += num;
                QString temp = "";
                for(int i = 0; i < num; i++)
                {
                    temp += value.mid(i*size, (i+1)*size) + "\n";
                }
                src.replace(value, temp);
                qDebug() << temp;
            }
        }
    }
    return QSize(nMaxWidth+m_spaceWid, (nCount + 1) * m_lineHeight+2*m_lineHeight);
}

void ChatWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);//消锯齿
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::gray));

    if(m_userType == User_Type::User_She)
    { // 用户
        //头像
        painter.drawPixmap(m_iconLeftRect, m_leftPixmap);

        //框加边
        QColor col_FrameB(234, 234, 234);
        painter.setBrush(QBrush(col_FrameB));
        painter.drawRoundedRect(m_FrameLeftRect.x()-1,m_FrameLeftRect.y()-1,m_FrameLeftRect.width()+2,m_FrameLeftRect.height()+2,4,4);
        //框
        QColor col_Frame(255,255,255);
        painter.setBrush(QBrush(col_Frame));
        painter.drawRoundedRect(m_FrameLeftRect,4,4);

        //三角
        QPointF points[3] =
        {
            QPointF(m_TriangleLeftRect.x(), 30),
            QPointF(m_TriangleLeftRect.x()+m_TriangleLeftRect.width(), 25),
            QPointF(m_TriangleLeftRect.x()+m_TriangleLeftRect.width(), 35),
        };
        QPen pen;
        pen.setColor(col_Frame);
        painter.setPen(pen);
        painter.drawPolygon(points, 3);

        //三角加边
        QPen penTriangleBian;
        penTriangleBian.setColor(col_FrameB);
        painter.setPen(penTriangleBian);
        painter.drawLine(QPointF(m_TriangleLeftRect.x() - 1, 30), QPointF(m_TriangleLeftRect.x()+m_TriangleLeftRect.width(), 24));
        painter.drawLine(QPointF(m_TriangleLeftRect.x() - 1, 30), QPointF(m_TriangleLeftRect.x()+m_TriangleLeftRect.width(), 36));

        //内容
        QPen penText;
        penText.setColor(QColor(51,51,51));
        painter.setPen(penText);
        QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        painter.setFont(this->font());
        painter.drawText(m_textLeftRect, m_msg,option);
    }  else if(m_userType == User_Type::User_Me) { // 自己
        //头像
        painter.drawPixmap(m_iconRightRect, m_rightPixmap);

        //框
        QColor col_Frame(75,164,242);
        painter.setBrush(QBrush(col_Frame));
        painter.drawRoundedRect(m_FrameRightRect,4,4);

        //三角
        QPointF points[3] =
        {
            QPointF(m_TriangleRightRect.x()+m_TriangleRightRect.width(), 30),
            QPointF(m_TriangleRightRect.x(), 25),
            QPointF(m_TriangleRightRect.x(), 35),
        };
        QPen pen;
        pen.setColor(col_Frame);
        painter.setPen(pen);
        painter.drawPolygon(points, 3);

        //内容
        QPen penText;
        penText.setColor(Qt::white);
        painter.setPen(penText);
        QTextOption option(Qt::AlignLeft | Qt::AlignVCenter);
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        painter.setFont(this->font());
        painter.drawText(m_textRightRect,m_msg,option);
    }
    else if(m_userType == User_Type::User_Time)
    { // 时间
        QPen penText;
        penText.setColor(QColor(153,153,153));
        painter.setPen(penText);
        QTextOption option(Qt::AlignCenter);
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        QFont te_font = this->font();
        te_font.setFamily("MicrosoftYaHei");
//        te_font.setFamily("Source Code Pro");
        te_font.setPointSize(10);
        painter.setFont(te_font);
        painter.drawText(this->rect(),m_curTime,option);
    }
}
