#include "backend.h"
#include "dr_wav.h"
#include "version.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrl>

Backend::Backend(QObject *parent)
    : QObject(parent), m_apikey("enter your own api key. dont be a prick :)") {
    m_config = ma_device_config_init(ma_device_type_capture);
    m_config.capture.format = ma_format_f32;
    m_config.capture.channels = 1;
    m_config.sampleRate = 16000;
    m_config.dataCallback = call_back;
    m_config.pUserData = this;
    ma_device_init(nullptr, &m_config, &m_device);
}
Backend::~Backend() { ma_device_uninit(&m_device); }
void Backend::prompt(const QString &prompt) {
    m_busy = true;
    emit busyChanged();
    QUrl url("https://generativelanguage.googleapis.com/v1beta/models/"
             "gemini-2.5-flash:generateContent");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("x-goog-api-key", m_apikey.toUtf8());
    QJsonObject body{
        {"contents",
         QJsonArray{QJsonObject{
             {"parts", QJsonArray{QJsonObject{{"text", prompt}}}}}}}};
    QByteArray data = QJsonDocument(body).toJson(QJsonDocument::Compact);
    QNetworkReply *reply = m_networkmanager.post(request, data);
    QObject::connect(reply, &QNetworkReply::finished, this,
                     &Backend::handleReply);
}
void Backend::handleReply() {
    auto *reply = qobject_cast<QNetworkReply *>((*this).sender());
    if (!reply)
        return;
    if ((*reply).error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson((*reply).readAll());
        QJsonObject root = doc.object();
        QJsonArray candidates = root["candidates"].toArray();
        if (candidates.isEmpty()) {
            m_busy = false;
            emit busyChanged();
            emit error("No candidates in Gemini response.");
            return;
        }
        QJsonObject firstcandidate = candidates.at(0).toObject();
        QJsonObject content = firstcandidate["content"].toObject();
        QJsonArray parts = content["parts"].toArray();
        QString complete_ai_text = "";
        for (const QJsonValue &partValue : parts) {
            QJsonObject partObj = partValue.toObject();
            complete_ai_text.append(partObj["text"].toString());
        }
        emit responseReceived(complete_ai_text);
    } else {
        emit error((*reply).errorString());
    }
    m_busy = false;
    emit busyChanged();
    (*reply).deleteLater();
}
void Backend::call_back(ma_device *pDevice, void *pOutput, const void *pInput,
                        ma_uint32 frameCount) {
    Backend *self = static_cast<Backend *>((*pDevice).pUserData);
    if (!self || !pInput)
        return;
    ma_uint32 byteCount =
        frameCount * ma_get_bytes_per_frame((*pDevice).capture.format,
                                            (*pDevice).capture.channels);
    (*self).m_audiodata.append(static_cast<const char *>(pInput), byteCount);
}
void Backend::startRecording() {
    m_audiodata.clear();
    ma_device_start(&m_device);
}
void Backend::stopRecording() {
    ma_device_stop(&m_device);
    m_busy = true;
    emit busyChanged();
    drwav wav;
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
    format.channels = 1;
    format.sampleRate = 16000;
    format.bitsPerSample = 32;
    void *pWavData = nullptr;
    size_t pDataSize;
    drwav_init_memory_write(
        &wav, &pWavData, &pDataSize, &format,
        nullptr); // Writing the data to memory with the metadata
    drwav_write_pcm_frames(
        &wav, static_cast<drwav_uint64>(m_audiodata.size() / sizeof(float)),
        m_audiodata.constData());
    drwav_uninit(&wav);
    QByteArray wavBytes(static_cast<const char *>(pWavData), pDataSize);
    drwav_free(pWavData, nullptr);
    QUrl url("https://generativelanguage.googleapis.com/v1beta/models/"
             "gemini-2.5-flash:generateContent");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("x-goog-api-key", m_apikey.toUtf8());
    QJsonObject body{
        {"contents",
         QJsonArray{QJsonObject{
             {"parts",
              QJsonArray{
                  QJsonObject{
                      {"inline_data",
                       QJsonObject{{"mimeType", "audio/wav"},
                                   {"data", QString(wavBytes.toBase64())}}}},
                  QJsonObject{{"text", "Respond conversationally to what the "
                                       "user says."}}}}}}}};
    QByteArray data = QJsonDocument(body).toJson(QJsonDocument::Compact);
    QNetworkReply *reply = m_networkmanager.post(request, data);
    QObject::connect(reply, &QNetworkReply::finished, this,
                     &Backend::handleReply);
}
QString Backend::version() const { return APP_VERSION; }
