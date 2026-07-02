#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

  public:
    explicit Backend(QObject *parent = nullptr);
    Q_INVOKABLE void prompt(const QString &prompt);
    Q_INVOKABLE QString version() const;
    bool busy() const { return m_busy; }
  signals:
    void responseReceived(const QString &response);
    void error(const QString &errorstring);
    void busyChanged();
  private slots:
    void handleReply();

  private:
    QNetworkAccessManager m_networkmanager;
    QString m_apikey;
    bool m_busy = false;
};