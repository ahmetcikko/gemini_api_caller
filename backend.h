#pragma once

#include "miniaudio.h"
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>

class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

  public:
    explicit Backend(QObject *parent = nullptr);
    ~Backend();
    Q_INVOKABLE void prompt(const QString &prompt);
    Q_INVOKABLE QString version() const;
    Q_INVOKABLE void startRecording();
    Q_INVOKABLE void stopRecording();
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
    QByteArray m_audiodata;
    ma_device m_device;
    ma_device_config m_config;
    static void call_back(ma_device *pDevice, void *pOutput, const void *pInput,
                          ma_uint32 frameCount);
    bool m_busy = false;
};