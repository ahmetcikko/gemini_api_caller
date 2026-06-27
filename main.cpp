#include "call.h"
#include "inputhandler.h"
#include <QCoreApplication>
#include <QThread>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    APIClient client;
    QThread thread;
    QObject::connect(&thread, &QThread::started, [&]() {
        console_input_loop(&client);
        thread.quit();
    });
    thread.start();
    app.exec();
}
