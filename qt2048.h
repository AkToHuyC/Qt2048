#ifndef GAME2048_H
#define GAME2048_H

#include <QWidget>
#include <QGridLayout>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QVector>
#include <QDebug>
#include <QLabel>


class Tile : public QLabel {
public:
    Tile(QWidget* parent = nullptr);
};

class Game2048 : public QWidget {
    Q_OBJECT
public:
    Game2048(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

private:
    void generateRandomTile();
    void updateScore(int value);

    Tile* tiles[4][4];
    QGridLayout* gridLayout;
    int score;
};

#endif // GAME2048_H
