#include "inputhandler.h"
#include "call.h"
#include <QCoreApplication>
#include <QEventLoop>
#include <iostream>
#include <print>

void console_input_loop(APIClient *client) {
    QEventLoop *currentloop = nullptr;

    QObject::connect(
        client, &APIClient::responseReceived,
        [&currentloop](const QString &response) {
            std::print("\nResponse:\n{}\n", response.toStdString());
            std::print("--------------------------------------------------\n");
            if (currentloop)
                (*currentloop).quit();
        });
    QObject::connect(
        client, &APIClient::error, [&currentloop](const QString &err) {
            std::print(stderr, "\nGemini Error:\n{}\n", err.toStdString());
            std::print(stderr,
                       "--------------------------------------------------\n");
            if (currentloop)
                (*currentloop).quit();
        });

    std::print(
        "Gemini AI Console Client\n"
        "Enter in prompts and get answers from AI.\n"
        "Type '//send' on a new line to send. Type '//exit' to quit.\n\n");

    while (true) {
        std::print("You:\n");
        std::string accumulatedinput;
        std::string currentline;
        while (true) {
            std::getline(std::cin, currentline);
            if (currentline == "//exit") {
                std::print("Exiting application...\n");
                QCoreApplication::quit();
                return;
            }
            if (currentline == "//send")
                break;
            accumulatedinput += currentline + "\n";
        }
        QString finalprompt =
            QString::fromStdString(accumulatedinput).trimmed();
        if (!finalprompt.isEmpty()) {
            std::print("\n[Sending prompt to Gemini...]\n");
            QEventLoop loop;
            currentloop = &loop;
            QMetaObject::invokeMethod(
                client,
                [client, finalprompt]() { (*client).prompt(finalprompt); },
                Qt::QueuedConnection);
            loop.exec();
            currentloop = nullptr;
        } else {
            std::print("Prompt was empty. Try again.\n\n");
        }
    }
}
