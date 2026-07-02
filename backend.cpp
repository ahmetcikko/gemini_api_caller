#include "backend.h"
#include "version.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrl>

Backend::Backend(QObject *parent)
    : QObject(parent), m_apikey("enter your own key, dont be a prick :)") {}
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
QString Backend::version() const { return APP_VERSION; }
