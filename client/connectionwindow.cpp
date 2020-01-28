/* Copyright (C) Sami SKHIRI, Inc - All Rights Reserved
 * Unauthorized using of this file for commercial purpose, via any medium is strictly prohibited
 * Written by Sami SKHIRI <skhiri.samy@gmail.com>, September 2014
 */

#include <QLabel>
#include <QMessageBox>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QCryptographicHash>

#include "consts.h"
#include "connectionwindow.h"
#include "subscriptionwindow.h"
#include "connectionmanager.h"
#include "chatwindow.h"

ConnectionWindow::ConnectionWindow() : QWidget()
{
    m_subsWindow = new SubscriptionWindow();
    drawInterface();
}

ConnectionWindow::~ConnectionWindow()
{
    if (m_subsWindow)
        delete m_subsWindow;

    ConnectionManager::Instance()->kill();
}

void ConnectionWindow::connection()
{
    if (m_login_LE->text().isEmpty() || m_password_LE->text().isEmpty())
    {
        QMessageBox::critical(this, "Error", "Password and username can not be empty");
    }
    else
    {
        ConnectionManager::Instance()->connect(this, 1, "127.0.0.1", 6606);
        QCryptographicHash passwordMd5 (QCryptographicHash::Md5);
        QByteArray input;
        input.append(m_password_LE->text());
        passwordMd5.addData(input);
        QString passwordStr = passwordMd5.result().toHex();
        QString msg = _connect + _tcpSeparator + m_login_LE->text() + _tcpSeparator + passwordStr;

        if (ConnectionManager::Instance()->sendData(msg))
            m_connect_PB->setEnabled(false);
    }
}

void ConnectionWindow::subscription()
{
    if (!m_subsWindow)
        return;

    m_subsWindow->show();
}

void ConnectionWindow::readData(const QString& receivdMessage)
{
    if (receivdMessage.isEmpty())
    {
        m_connect_PB->setEnabled(true);
        return;
    }

    QStringList result = receivdMessage.split(_tcpSeparator);

    if (result[0] != _serverToken)
        return;

    if (result[1] == _connect && result[2] == _serverOk)
    {
        // chatWindow is automatically delated after it is closed due to the WA_DeleteOnClose attribute
        ChatWindow* cWindow = new ChatWindow(m_login_LE->text());
        cWindow->show();
        connect(cWindow, SIGNAL(hideWindow()), this, SLOT(displayWindow()));
        this->hide();
    }
    else
    {
        QMessageBox::information(this, "Error", result[2]);
        m_connect_PB->setEnabled(true);
    }
}

void ConnectionWindow::displayError(const QString& error)
{
    QMessageBox::information(this, "Client", error);
    m_connect_PB->setEnabled(true);
}

void ConnectionWindow::displayWindow()
{
    m_login_LE->clear();
    m_password_LE->clear();
    m_connect_PB->setEnabled(true);
    this->show();
}

void ConnectionWindow::drawInterface()
{
    setWindowTitle("Chat App Vol. 9");
    this->setWindowIcon(QIcon(":/img/app_icon.png"));
    setFixedSize(260, 400);
    setGeometry(600, 200, 100, 385);

    // background image
    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(QImage(":/img/connection_background.jpg")));
    this->setPalette(palette);

    // userLogo
    m_userlogo_PB = new QPushButton(this);
    m_userlogo_PB->setIcon(QIcon(":/img/user_group.png"));
    m_userlogo_PB->setIconSize(QSize(140, 110));
    m_userlogo_PB->setGeometry(57, 10, 141, 111);
    m_userlogo_PB->setFlat(true);

    // login_LE
    QLabel* user = new QLabel("Username :", this);
    user->setStyleSheet("QLabel { color : white; }");
    user->setGeometry(5, 149, 250, 23);
    m_login_LE = new QLineEdit(this);
    m_login_LE->setGeometry(5, 168, 250, 20);
    m_login_LE->setFocus();

    // password
    QLabel* pass = new QLabel("Password :", this);
    pass->setGeometry(5, 194, 250, 23);
    pass->setStyleSheet("QLabel { color : white; }");
    m_password_LE = new QLineEdit(this);
    m_password_LE->setGeometry(5, 219, 250, 20);
    m_password_LE->setEchoMode(QLineEdit::Password);

    // connect
    m_connect_PB = new QPushButton("Log in", this);
    m_connect_PB->setGeometry(5, 285, 250, 23);

    // Quit
    m_quitter_PB = new QPushButton("Quit", this);
    m_quitter_PB->setGeometry(5, 314, 250, 23);

    // subscription
    m_inscription_PB = new QPushButton("Register", this);
    m_inscription_PB->setGeometry(5, 350, 250, 23);

    //Ip & Port configuration
//    m_configuration_PB = new QPushButton("Configuration", this);
//    m_configuration_PB->setGeometry(1, 350, 75, 23);

    // connexions
    QObject::connect(m_quitter_PB, SIGNAL(clicked()), qApp, SLOT(quit()));
    QObject::connect(m_connect_PB, SIGNAL(clicked()), this, SLOT(connection()));  // verifier!!
    QObject::connect(m_inscription_PB, SIGNAL(clicked()), this, SLOT(subscription()));
//    QObject::connect(m_inscription_PB, SIGNAL(clicked()), this, SLOT(subscription()));
    QObject::connect(m_password_LE, SIGNAL(returnPressed()), this, SLOT(connection()));
}
