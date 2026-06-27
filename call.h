#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>

class APIClient : public QObject {
    Q_OBJECT

  public:
    explicit APIClient(QObject *parent = nullptr);
    void prompt(const QString &prompt);
  signals:
    void responseReceived(const QString &response);
    void error(const QString &errorstring);
  private slots:
    void handleReply();

  private:
    QNetworkAccessManager m_networkmanager;
    QString m_apikey;
};