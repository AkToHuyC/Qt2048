#include "qt2048.h"

Tile::Tile(QWidget* parent) : QLabel(parent), value(0) {
    setText("");
    setAlignment(Qt::AlignCenter);
    updateColor();
}

void Tile::setTileValue(int value) {
    this->value = value;
    if (value == 0) {
        setText("");
    } else {
        setText(QString::number(value));
    }
    updateColor();
}

void Tile::updateColor() {
    QString color;
    switch (value) {
    case 0:
        color = "lightgray";
        break;
    case 2:
        color = "#eee4da";
        break;
    case 4:
        color = "#ede0c8";
        break;
    case 8:
        color = "#f2b179";
        break;
    case 16:
        color = "#f59563";
        break;
    case 32:
        color = "#f67c5f";
        break;
    case 64:
        color = "#f65e3b";
        break;
    case 128:
        color = "#edcf72";
        break;
    case 256:
        color = "#edcc61";
        break;
    case 512:
        color = "#edc850";
        break;
    case 1024:
        color = "#edc53f";
        break;
    case 2048:
        color = "#edc22e";
        break;
    default:
        color = "black";
        break;
    }
    setStyleSheet(QString("QLabel { background-color : %1; border-radius: 5px; font-size: 24px; font-weight: bold; }").arg(color));
}

Game2048::Game2048(QWidget* parent) : QWidget(parent), score(0) {
    setFixedSize(400, 400);
    setWindowTitle("2048");

    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(10);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            Tile* tile = new Tile(this);
            gridLayout->addWidget(tile, i, j);
            tiles[i][j] = tile;
        }
    }

    generateRandomTile();
    generateRandomTile();
}

void Game2048::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Left) moveLeft();
    else if (event->key() == Qt::Key_Right) moveRight();
    else if (event->key() == Qt::Key_Up) moveUp();
    else if (event->key() == Qt::Key_Down) moveDown();
}

void Game2048::generateRandomTile() {
    int x = QRandomGenerator::global()->bounded(4);
    int y = QRandomGenerator::global()->bounded(4);

    while (tiles[x][y]->text() != "") {
        x = QRandomGenerator::global()->bounded(4);
        y = QRandomGenerator::global()->bounded(4);
    }

    int value = QRandomGenerator::global()->bounded(10) == 0 ? 4 : 2;
    tiles[x][y]->setTileValue(value);
}

void Game2048::updateScore(int value) {
    score += value;
    qDebug() << "Score:" << score;
}

void Game2048::moveLeft() {
    bool moved = false;
    for (int i = 0; i < 4; ++i) {
        QVector<int> merged(4, 0);
        for (int j = 1; j < 4; ++j) {
            if (tiles[i][j]->text() != "") {
                int k = j;
                while (k > 0 && tiles[i][k - 1]->text() == "") {
                    tiles[i][k - 1]->setTileValue(tiles[i][k]->text().toInt());
                    tiles[i][k]->setTileValue(0);
                    --k;
                    moved = true;
                }
                if (k > 0 && tiles[i][k - 1]->text() == tiles[i][k]->text() && merged[k - 1] == 0) {
                    int value = tiles[i][k]->text().toInt() * 2;
                    tiles[i][k - 1]->setTileValue(value);
                    tiles[i][k]->setTileValue(0);
                    merged[k - 1] = 1;
                    updateScore(value);
                    moved = true;
                }
            }
        }
    }
    if (moved) generateRandomTile();
}

void Game2048::moveRight() {
    bool moved = false;
    for (int i = 0; i < 4; ++i) {
        QVector<int> merged(4, 0);
        for (int j = 2; j >= 0; --j) {
            if (tiles[i][j]->text() != "") {
                int k = j;
                while (k < 3 && tiles[i][k + 1]->text() == "") {
                    tiles[i][k + 1]->setTileValue(tiles[i][k]->text().toInt());
                    tiles[i][k]->setTileValue(0);
                    ++k;
                    moved = true;
                }
                if (k < 3 && tiles[i][k + 1]->text() == tiles[i][k]->text() && merged[k + 1] == 0) {
                    int value = tiles[i][k]->text().toInt() * 2;
                    tiles[i][k + 1]->setTileValue(value);
                    tiles[i][k]->setTileValue(0);
                    merged[k + 1] = 1;
                    updateScore(value);
                    moved = true;
                }
            }
        }
    }
    if (moved) generateRandomTile();
}

void Game2048::moveUp() {
    bool moved = false;
    for (int j = 0; j < 4; ++j) {
        QVector<int> merged(4, 0);
        for (int i = 1; i < 4; ++i) {
            if (tiles[i][j]->text() != "") {
                int k = i;
                while (k > 0 && tiles[k - 1][j]->text() == "") {
                    tiles[k - 1][j]->setTileValue(tiles[k][j]->text().toInt());
                    tiles[k][j]->setTileValue(0);
                    --k;
                    moved = true;
                }
                if (k > 0 && tiles[k - 1][j]->text() == tiles[k][j]->text() && merged[k - 1] == 0) {
                    int value = tiles[k][j]->text().toInt() * 2;
                    tiles[k - 1][j]->setTileValue(value);
                    tiles[k][j]->setTileValue(0);
                    merged[k - 1] = 1;
                    updateScore(value);
                    moved = true;
                }
            }
        }
    }
    if (moved) generateRandomTile();
}

void Game2048::moveDown() {
    bool moved = false;
    for (int j = 0; j < 4; ++j) {
        QVector<int> merged(4, 0);
        for (int i = 2; i >= 0; --i) {
            if (tiles[i][j]->text() != "") {
                int k = i;
                while (k < 3 && tiles[k + 1][j]->text() == "") {
                    tiles[k + 1][j]->setTileValue(tiles[k][j]->text().toInt());
                    tiles[k][j]->setTileValue(0);
                    ++k;
                    moved = true;
                }
                if (k < 3 && tiles[k + 1][j]->text() == tiles[k][j]->text() && merged[k + 1] == 0) {
                    int value = tiles[k][j]->text().toInt() * 2;
                    tiles[k + 1][j]->setTileValue(value);
                    tiles[k][j]->setTileValue(0);
                    merged[k + 1] = 1;
                    updateScore(value);
                    moved = true;
                }
            }
        }
    }
    if (moved) generateRandomTile();
}
