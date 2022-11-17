#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QIntValidator>
#include <QMessageBox>

#include "sslserver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , users_(0)
{
    ui->setupUi(this);

    server_ = new SslServer;

    QIntValidator *port_validator = new QIntValidator;
    port_validator->setRange(0, 65536);
    ui->port_line_edit->setValidator(port_validator);

    auto UpdateWindowTitle = [this](QString ip, quint16 port, int users)
    {
        setWindowTitle(QString("ip:%1, port:%2, users:%3").arg(ip).arg(port).arg(users));
    };

    connect(ui->start_push_button, &QPushButton::clicked, [this, UpdateWindowTitle]
    {
        if (!server_->listen(QHostAddress::Any, ui->port_line_edit->text().toInt()))
        {
            QMessageBox::critical(nullptr, "Ошибка", "Ошибка запуска сервера");
            return;
        }

        ui->stacked_widget->setCurrentIndex(1);
        UpdateWindowTitle(server_->serverAddress().toString(), server_->serverPort(), users_);
    });
    connect(server_, &SslServer::UserAdded, [this, UpdateWindowTitle](QString const &username)
    {
        ui->log_text_edit->append(QString("Пользователь %1 зашёл на сервер.").arg(username));
        UpdateWindowTitle(server_->serverAddress().toString(), server_->serverPort(), ++users_);
    });
    connect(server_, &SslServer::UserRemoved, [this, UpdateWindowTitle](QString const &username)
    {
        ui->log_text_edit->append(QString("Пользователь %1 вышел с сервера.").arg(username));
        UpdateWindowTitle(server_->serverAddress().toString(), server_->serverPort(), --users_);
    });
    connect(server_, &SslServer::UserStatusChanged, [this](QString const &username, Status status)
    {
        QString string_status;
        switch (status)
        {
        case kAvailable:
            string_status = "Доступен";
            break;
        case kMovedAway:
            string_status = "Отошёл";
            break;
        case kDoNotDisturb:
            string_status = "Не беспокоить";
            break;
        }
        ui->log_text_edit->append(QString("Пользователь %1 меняет свой статус на %2.").arg(username).arg(string_status));
    });
    connect(server_, &SslServer::UsernameChanged, [this](QString const &old_username, QString const &new_username)
    {
        ui->log_text_edit->append(QString("Пользователь %1 меняет имя пользователя на %2.").arg(old_username).arg(new_username));
    });
    connect(server_, &SslServer::MessageReceived, [this](QString const &ip, QString const &username, QString const &date_time, QString const &msg)
    {
        ui->log_text_edit->append(QString("%1 %2(%3): %4").arg(date_time).arg(username).arg(ip).arg(msg));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

