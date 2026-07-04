#pragma once

#include "miniaudio.h"
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QStringList>

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
    Q_INVOKABLE void select_device(int index);
    Q_PROPERTY(QStringList devicenames MEMBER m_devicenames NOTIFY
                   device_names_changed);
    bool busy() const { return m_busy; }
  signals:
    void responseReceived(const QString &response);
    void error(const QString &errorstring);
    void busyChanged();
    void device_names_changed();
  private slots:
    void handleReply();

  private:
    QNetworkAccessManager m_networkmanager;
    QString m_apikey;
    QStringList m_devicenames;
    QByteArray m_audiodata;
    ma_context m_context;
    ma_device_info *m_captureinfos;
    ma_uint32 m_capturecount;
    ma_device m_device;
    ma_device_config m_config;
    static void call_back(ma_device *pDevice, void *pOutput, const void *pInput,
                          ma_uint32 frameCount);
    void enumerate_devices();
    bool m_busy = false;
    int m_selectedDevice;
};