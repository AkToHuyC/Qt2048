#include "qt2048.h"
#include <QMessageBox>
#include <algorithm>

// ------------ Tile ------------
Tile::Tile(QWidget* parent)
    : QLabel(parent),
    value(0)
{
    setText("");
    setAlignment(Qt::AlignCenter);
    updateColor();
}

void Tile::setTileValue(int newValue) {
    value = newValue;
    setText(value == 0 ? "" : QString::number(value));
    updateColor();
}

void Tile::updateColor() {
    QString color;
    switch (value) {
    case 0:    color = "lightgray";  break;
    case 2:    color = "#eee4da";    break;
    case 4:    color = "#ede0c8";    break;
    case 8:    color = "#f2b179";    break;
    case 16:   color = "#f59563";    break;
    case 32:   color = "#f67c5f";    break;
    case 64:   color = "#f65e3b";    break;
    case 128:  color = "#edcf72";    break;
    case 256:  color = "#edcc61";    break;
    case 512:  color = "#edc850";    break;
    case 1024: color = "#edc53f";    break;
    case 2048: color = "#edc22e";    break;
    default:   color = "#3c3a32";    // значения свыше 2048
        break;
    }

    setStyleSheet(QString("QLabel { "
                          "background-color : %1; "
                          "border-radius: 5px; "
                          "font-size: 24px; "
                          "font-weight: bold; }").arg(color));
}

// ------------ Game2048 ------------
Game2048::Game2048(QWidget* parent)
    : QWidget(parent),
    score(0),
    bestScore(0)
{
    setFixedSize(400, 400);
    setWindowTitle("2048");

    gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(10, 10, 10, 10);

    // Создаём тайлы
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            auto* tile = new Tile(this);
            gridLayout->addWidget(tile, i, j);
            tiles[i][j] = tile;
        }
    }

    // Запускаем новую игру
    newGame();
}

void Game2048::keyPressEvent(QKeyEvent* event) {
    bool moved = false;
    switch (event->key()) {
    case Qt::Key_Left:
        moveTiles(Direction::Left);
        moved = true;
        break;
    case Qt::Key_Right:
        moveTiles(Direction::Right);
        moved = true;
        break;
    case Qt::Key_Up:
        moveTiles(Direction::Up);
        moved = true;
        break;
    case Qt::Key_Down:
        moveTiles(Direction::Down);
        moved = true;
        break;
    case Qt::Key_R:  // например, клавишей R можно перезапускать игру
        newGame();
        break;
    default:
        break;
    }

    if (moved) {
        // Если был ход, генерируем новую плитку и проверяем состояние игры
        generateRandomTile();
        checkGameState();
    }
}

void Game2048::newGame() {
    resetBoard();
    score = 0;
    hasWon = false;

    // Генерируем первые две плитки
    generateRandomTile();
    generateRandomTile();
}

// Генерация новой плитки (2 или 4) в свободной ячейке
void Game2048::generateRandomTile() {
    // Собираем список пустых ячеек
    QVector<QPair<int,int>> emptyPositions;
    emptyPositions.reserve(BOARD_SIZE * BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (tiles[i][j]->tileValue() == 0) {
                emptyPositions.append(qMakePair(i, j));
            }
        }
    }
    if (emptyPositions.isEmpty()) return;

    // Случайный выбор из пустых
    int rndIndex = QRandomGenerator::global()->bounded(emptyPositions.size());
    auto pos = emptyPositions.at(rndIndex);

    // С вероятностью 10% ставим 4, иначе 2
    int value = (QRandomGenerator::global()->bounded(10) == 0) ? 4 : 2;
    tiles[pos.first][pos.second]->setTileValue(value);
}

// Можно ли ещё сделать ход
bool Game2048::canMove() const {
    // (1) Есть ли свободные клетки
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (tiles[i][j]->tileValue() == 0) {
                return true;
            }
        }
    }
    // (2) Можно ли слить соседние
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            int val = tiles[i][j]->tileValue();
            // Проверяем соседа справа
            if (j < BOARD_SIZE - 1 && val == tiles[i][j+1]->tileValue()) return true;
            // Проверяем соседа снизу
            if (i < BOARD_SIZE - 1 && val == tiles[i+1][j]->tileValue()) return true;
        }
    }
    return false;
}

// Проверяем, достигнут ли 2048
bool Game2048::checkWin() const {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (tiles[i][j]->tileValue() >= 16) {
                return true;
            }
        }
    }
    return false;
}

// Универсальное перемещение (вместо 4 отдельных методов)
void Game2048::moveTiles(Direction dir) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        // Извлекаем строку/столбец
        QVector<int> line(BOARD_SIZE);
        bool horizontal = (dir == Direction::Left || dir == Direction::Right);

        copyToLine(i, horizontal, line);

        // Для сдвига вправо/вниз - разворачиваем вектор
        if (dir == Direction::Right || dir == Direction::Down) {
            std::reverse(line.begin(), line.end());
        }

        // "сжимаем" и объединяем
        slideAndMergeLine(line);

        // Возвращаем порядок для записи
        if (dir == Direction::Right || dir == Direction::Down) {
            std::reverse(line.begin(), line.end());
        }

        // Записываем обратно
        copyFromLine(i, horizontal, line);
    }
}

// Сжатие и слияние одной строки (либо преобразованного столбца)
bool Game2048::slideAndMergeLine(QVector<int> &line) {
    bool changed = false;

    // 1) "Сжатие" влево (убираем нули)
    {
        QVector<int> tmp;
        tmp.reserve(BOARD_SIZE);
        for (int v : line) {
            if (v != 0) {
                tmp.append(v);
            }
        }
        while (tmp.size() < BOARD_SIZE) {
            tmp.append(0);
        }
        if (tmp != line) {
            changed = true;
            line = tmp;
        }
    }

    // 2) Слияние соседних
    for (int i = 0; i < BOARD_SIZE - 1; ++i) {
        if (line[i] != 0 && line[i] == line[i + 1]) {
            int mergedValue = line[i] * 2;
            line[i] = mergedValue;
            line[i + 1] = 0;
            updateScore(mergedValue);
        }
    }

    // 3) Повторное "сжатие"
    {
        QVector<int> tmp;
        tmp.reserve(BOARD_SIZE);
        for (int v : line) {
            if (v != 0) {
                tmp.append(v);
            }
        }
        while (tmp.size() < BOARD_SIZE) {
            tmp.append(0);
        }
        if (tmp != line) {
            changed = true;
            line = tmp;
        }
    }

    return changed;
}

// Копируем вектор из строки/столбца
void Game2048::copyToLine(int index, bool horizontal, QVector<int> &line) const {
    for (int k = 0; k < BOARD_SIZE; ++k) {
        if (horizontal) {
            line[k] = tiles[index][k]->tileValue();
        } else {
            line[k] = tiles[k][index]->tileValue();
        }
    }
}

// Записываем в строку/столбец из вектора
void Game2048::copyFromLine(int index, bool horizontal, const QVector<int> &line) {
    for (int k = 0; k < BOARD_SIZE; ++k) {
        if (horizontal) {
            tiles[index][k]->setTileValue(line[k]);
        } else {
            tiles[k][index]->setTileValue(line[k]);
        }
    }
}

void Game2048::updateScore(int val) {
    score += val;
    if (score > bestScore) {
        bestScore = score;
    }
    qDebug() << "Score:" << score << "| Best:" << bestScore;
}

// Проверяем выигрыш/проигрыш
void Game2048::checkGameState() {
    // Если ещё не поздравляли и 2048 достигнута:
    if (!hasWon && checkWin()) {
        QMessageBox::information(this, tr("Поздравляем!"),
                                 tr("Вы собрали 2048!"));
        // Устанавливаем флаг, чтобы больше не показывать это сообщение
        hasWon = true;

        // Если хотите автоматически завершать игру, можно вызвать здесь newGame().
        // Или оставить, чтобы игрок продолжал дальше.
    }
    // Если нет ходов, значит проиграли
    else if (!canMove()) {
        QMessageBox::information(this, tr("Игра окончена"),
                                 tr("Ходов больше нет!"));
        newGame();
    }
}

// Очистка поля для новой игры
void Game2048::resetBoard() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            tiles[i][j]->setTileValue(0);
        }
    }
}
