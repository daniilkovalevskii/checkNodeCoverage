#ifndef NODE_H
#define NODE_H
#include <QList>

enum NodeShape { DIAMOND, RECTANGLE, DEFAULT };

class Node
{
public:
    Node();
    Node(QString newName, NodeShape newShape);
    Node* parent;
    QVector<Node*> children;
    bool isLeaf() const;
    NodeShape getShape();
    QString getName();
    bool operator== (Node &other);

private:
    QString name;
    NodeShape shape;
};

#endif // NODE_H
