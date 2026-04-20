#include "node.h"
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <QRegularExpression>


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

bool parse(const QStringList& text)
{
    /*  Порядок проверок
     *  0. пустая строка
     *  1. forbidden
     *  2. graphDeclaration
     *  3. graphEnd
     *  4. attributes
     *  5. edge
     *  6. defaultNodeDeclaration
     */
    QRegularExpression graphDeclaration(R"(^\s*digraph\s+\w+\s*\{)");
    QRegularExpression graphEnd(R"(^\s*})");
    QRegularExpression defaulNodeDeclaration(R"(^\s*(\w+)\s*;)");
    QRegularExpression edge(R"(^\s*(\w+(?:\s*->\s*\w+)+)\s*;)");
    QRegularExpression attributes(R"(^\s*\b(?<nodeName>\w+)\b\s*\[\s*shape\s*\=\s*\b(?<nodeShape>\w+)\b\s*\]\s*;)");
    QRegularExpression forbidden(R"(^\s*(subgraph|cluster|graph|node|edge)\b)");

    for (int i = 0; i < text.size(); i++)
    {
        QString line = text[i];
        int lineNumber = i + 1;
        QRegularExpressionMatch match;

        if (line.trimmed().isEmpty())
        {
            qDebug() << QString("line %1 is empty").arg(lineNumber);
        }
        else if ((match = forbidden.match(line)).hasMatch())
        {
            qDebug() << "Обнаружена запрещённая структура или сложный тип узла в строке " << lineNumber << ":" << line;
            qDebug() << "Используйте только плоский digraph и простые формы.";
        }
        else if ((match = graphDeclaration.match(line)).hasMatch())
        {
            qDebug() << "Graph Started";
        }
        else if ((match = graphEnd.match(line)).hasMatch())
        {
            qDebug() << "Graph ended";
        }
        else if ((match = attributes.match(line)).hasMatch())
        {
            qDebug() << QString("Node: %1; shape = %2").arg(match.captured("nodeName"), match.captured("nodeShape"));
        }
        else if ((match = edge.match(line)).hasMatch())
        {
            qDebug() << "Edge:" << match.captured(1);
        }
        else if ((match = defaulNodeDeclaration.match(line)).hasMatch())
        {
            qDebug() << "Default node:" << match.captured(1);
        }
        else {
            qDebug() << "Синтаксическая ошибка в описании графа DOT в строке " << lineNumber << ":" << line;
        }

    }

    return true;
}

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    // TEST DATA
    QStringList ex =
    {
        {"graph G {"},
        {""},
        {"  first[shape = diamond];"},
        {"  fourth[shape = rectangle];"},
        {"  fifth[shape = rectangle];"},
        {"  sixth[shape = rectangle];"},
        {""},
        {""},
        {"  first -> second;"},
        {"  first -> third;"},
        {"  third - fourth;"},
        {"  second -> fifth;"},
        {"  second -> sixth;"},
        {""},
        {"  twenty;"},
        {"}"}
    };


    parse(ex);

    //return a.exec();
    return 0;

}
