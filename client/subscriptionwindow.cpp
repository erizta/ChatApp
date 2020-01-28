/* Copyright (C) Sami SKHIRI, Inc - All Rights Reserved
 * Unauthorized using of this file for commercial purpose, via any medium is strictly prohibited
 * Written by Sami SKHIRI <skhiri.samy@gmail.com>, September 2014
 */

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCryptographicHash>

#include "consts.h"
#include "subscriptionwindow.h"
#include "connectionmanager.h"

//TODO: Create a string list of the different error messages
// And managing the user locale
SubscriptionWindow::SubscriptionWindow(QWidget* parent) : QWidget(parent)
{
    drawInterface();
}

void SubscriptionWindow::closeWindow()
{
    this->close();
}

void SubscriptionWindow::sendData(const QString& message) const
{
    // TODO: display an error message in case the message wasn't sent
    ConnectionManager::Instance()->sendData(message);
}

void SubscriptionWindow::readData(const QString& receivdMessage)
{
    qDebug() << "readData = " + receivdMessage;
    if (receivdMessage.isEmpty())
    {
        m_valide_PB->setEnabled(true);
        return;
    }

    QStringList result = receivdMessage.split(_tcpSeparator);
    if (result[0] == _serverToken)
    {
        if (result[1] == _newAccount && result[2] == _serverOk)
        {
            QMessageBox::information(this, "info", QString::fromUtf8("User successfully added"));
            m_name_LE->clear();
            m_password_LE->clear();
            m_valide_PB->setEnabled(true);
            closeWindow();
        }
        else
        {
            QMessageBox::information(this, "Error", QString::fromLatin1("the requested login is already used"));
            m_valide_PB->setEnabled(true);
        }
    }
    qDebug() << "/readData";
}

void SubscriptionWindow::newAccountRequest()
{
    qDebug() << "newAccount";
    if (m_name_LE->text().isEmpty() || m_password_LE->text().isEmpty())
        QMessageBox::critical(this, "Error", "incorrect login or password");

    else
    {
        ConnectionManager::Instance()->connect(this, 0, "127.0.0.1", 6606);
        QCryptographicHash passwordMd5 (QCryptographicHash::Md5);
        QByteArray input;
        input.append(m_password_LE->text());
        passwordMd5.addData(input);
        QString passwordStr = passwordMd5.result().toHex();
        QString msg = _newAccount + _tcpSeparator + m_name_LE->text() + _tcpSeparator + passwordStr;

        if (ConnectionManager::Instance()->sendData(msg))
            m_valide_PB->setEnabled(false);
    }
    qDebug() << "/newAccount";
}

void SubscriptionWindow::displayError(const QString& error)
{
    QMessageBox::information(this, "Client", error);
    m_valide_PB->setEnabled(true);
}

void SubscriptionWindow::drawInterface()
{
    setWindowTitle("Register");
    this->setWindowIcon(QIcon(":/img/app_icon.png"));
    setFixedSize(260, 400);
    setGeometry(600, 200, 100, 385);

    // background image
    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(QImage(":/img/registration_background.jpg")));
    this->setPalette(palette);

    // label username
    QLabel* login_label = new QLabel("Username :", this);
    login_label->setGeometry(5, 149, 250, 23);
    m_name_LE = new QLineEdit(this);
    m_name_LE->setGeometry(5, 168, 250, 20);
    m_name_LE->setFocus();

    // label password
    QLabel* password_label = new QLabel("Password:", this);
    password_label->setGeometry(5, 194, 250, 23);
    m_password_LE = new QLineEdit(this);
    m_password_LE->setEchoMode(QLineEdit::Password);
    m_password_LE->setGeometry(5, 219, 250, 20);

    // validasi
    m_valide_PB = new QPushButton("OK", this);
    m_valide_PB->setGeometry(5, 285, 250, 23);

    // keluar
    m_annuler_PB = new QPushButton("Cancel", this);
    m_annuler_PB->setGeometry(5, 314, 250, 23);

    QObject::connect(m_annuler_PB, SIGNAL(clicked()), this, SLOT(closeWindow()));
    QObject::connect(m_valide_PB, SIGNAL(clicked()), this, SLOT(newAccountRequest()));
}
