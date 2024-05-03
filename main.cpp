#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QRandomGenerator>
#include <QVector>
#include "qt2048.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Game2048 game;
    game.show();
    return app.exec();
}

#include "main.moc"
