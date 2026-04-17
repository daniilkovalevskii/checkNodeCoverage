#include "node.h"
#include <QDebug>
#include <QFile>
#include <QCoreApplication>

enum State {
    ABOVE_TARGET,
    SEARCHING,
    COVERED
};


struct Result {
    QVector<Node*> erroneous;
    QVector<Node*> redundant;
    QVector<Node*> uncoveredLeaves;
    QVector<Node*> missing;

    QVector<Node*> foundTargets;
    QString fileError;
};

struct Info {
    int uncovered;   // Число непокрытых листьев снизу
};


Info dfs(Node* node,
         const QSet<Node*>& marked,
         State state,
         Node* activeMarkedAncestor,
         Result& result)
{
    // Если нашли цель - включаем режим поиска дыр
    if (node->getShape() == DIAMOND) {
        result.foundTargets.push_back(node);

        if (result.foundTargets.size() > 1) {
            QStringList names;
            for (Node* t : result.foundTargets) names << t->getName();

            // Очищаем старую ошибку и пишем актуальный список
            result.fileError = QString("Ошибка: обнаружено несколько целевых узлов: %1\n")
                                   .arg(names.join(", "));
        }
        state = SEARCHING;
    }


    bool isMarked = marked.contains(node);
    bool isValidMarked = false;

    if (isMarked) {
        if (state == ABOVE_TARGET) {
            // Отмеченный узел выше цели - это ошибка
            result.erroneous.push_back(node);
        }
        else if (activeMarkedAncestor != nullptr) {
            // Узел избыточен, так как выше уже есть отмеченный узел
            result.fileError += QString("Ошибка: узел %1 избыточен, так как он находится в поддереве узла %2.\n")
                                    .arg(node->getName())
                                    .arg(activeMarkedAncestor->getName());
            result.redundant.push_back(node);
        }
        else {
            // Это корректный прямоугольник-защитник
            isValidMarked = true;
            state = COVERED;
            activeMarkedAncestor = node; // Теперь он становится "щитом" для детей
        }
    }

    // Обходим всех детей

    int totalUncovered = 0;
    for (Node* child : node->children) {
        Info childInfo = dfs(child,
                             marked,
                             state,
                             activeMarkedAncestor,
                             result);
        totalUncovered += childInfo.uncovered;
    }


    if (node->isLeaf()) {
        if (state == SEARCHING) {
            // Если мы в поиске и не встретили RECTANGLE — это дыра
            result.uncoveredLeaves.push_back(node);
            return {1}; // Сигнализируем наверх об 1 непокрытой точке
        }
        return {0};
    }


    if (state == SEARCHING && totalUncovered > 0) {

        // Проверяем, можем ли мы заменить всех детей текущим узлом
        bool canCollapse = (node->getShape() != DIAMOND) &&
                           (activeMarkedAncestor == nullptr) &&
                           (totalUncovered == node->children.size());

        if (canCollapse) {
            // Удаляем детей из временного списка
            int toRemove = qMin(totalUncovered, (int)result.uncoveredLeaves.size());
            for (int i = 0; i < toRemove; ++i) {
                result.uncoveredLeaves.pop_back();
            }

            // Теперь текущий узел сам становится "дыркой" для своего родителя
            result.uncoveredLeaves.push_back(node);
            return {1};
        }

        // Если схлопнуть нельзя (например, часть веток закрыта), только тогда переносим детей в финальный список missing
        auto it = result.uncoveredLeaves.begin();
        while (it != result.uncoveredLeaves.end()) {
            if ((*it)->parent == node) {
                result.missing.push_back(*it);
                it = result.uncoveredLeaves.erase(it);
            } else {
                ++it;
            }
        }
    }


    // Если узел был валидным RECTANGLE, он "гасит" все дыры под собой для родителя
    if (isValidMarked) {
        return {0};
    }

    return {totalUncovered};
}



int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    // Структура: R -> [ A(DIAMOND), B(DIAMOND), C(DIAMOND) ]
    Node* R = new Node("Root", DEFAULT);
    Node* A = new Node("Alpha", DIAMOND);
    Node* B = new Node("Beta", DIAMOND);
    Node* C = new Node("Gamma", DIAMOND);

    R->children = {A, B, C};
    A->parent = R; B->parent = R; C->parent = R;

    QSet<Node*> marked;



    Result result;

    dfs(R, marked, ABOVE_TARGET, nullptr, result);



    qDebug() << "All marked nodes:";
    for (Node* node : marked) {
        if (node) { // Проверка на nullptr
            qDebug() << node ->getName();
        }
    }

    qDebug() << "ERRORS:";
    qDebug() << result.fileError;

    qDebug() << "__________________________________________";

    qDebug() << "ERRONEOUS:";
    if (result.erroneous.isEmpty())
        qDebug() << "NO ERRONEOUS NODES";
    for (Node* node : result.erroneous) {
        if (node) { // Проверка на nullptr
            qDebug() << node ->getName();
        }
    }

    qDebug() << "REDUNDANT:";
    if (result.redundant.isEmpty())
        qDebug() << "NO REDUNDANT NODES";
    for (Node* node : result.redundant) {
        if (node) { // Проверка на nullptr
            qDebug() << node ->getName();
        }
    }

    qDebug() << "MISSING:";
    if (result.missing.isEmpty())
        qDebug() << "NO MISSING NODES";
    for (Node* node : result.missing) {
        if (node) { // Проверка на nullptr
            qDebug() << node ->getName();
        }
    }


    //return a.exec();
    return 0;

}
