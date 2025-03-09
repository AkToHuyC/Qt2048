#ifndef GAME2048_H
#define GAME2048_H

#include <QWidget>
#include <QGridLayout>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QVector>
#include <QDebug>
#include <QLabel>
#include <QPair>

class Tile : public QLabel {
    Q_OBJECT
public:
    explicit Tile(QWidget* parent = nullptr);
    void setTileValue(int newValue);
    int tileValue() const { return value; }
    void updateColor();

private:
    int value;
};

class Game2048 : public QWidget {
    Q_OBJECT
public:
    // Здесь задаём размер поля
    static constexpr int BOARD_SIZE = 6;

    explicit Game2048(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void newGame();                  // перезапуск игры

private:
    enum class Direction { Left, Right, Up, Down };

    void generateRandomTile();
    bool canMove() const;            // можно ли сделать хоть один ход
    bool checkWin() const;           // достигнут ли 2048
    void moveTiles(Direction dir);   // универсальное перемещение во все стороны
    bool slideAndMergeLine(QVector<int> &line);
    void copyToLine(int index, bool horizontal, QVector<int>& line) const;
    void copyFromLine(int index, bool horizontal, const QVector<int>& line);
    void updateScore(int val);
    void checkGameState();

    void resetBoard();               // очистка поля (для newGame)

    // Храним тайлы в статическом массиве, размеры которого зависят от BOARD_SIZE
    Tile* tiles[BOARD_SIZE][BOARD_SIZE];
    QGridLayout* gridLayout;
    int score;
    int bestScore;
    bool hasWon;
};

#endif // GAME2048_H
