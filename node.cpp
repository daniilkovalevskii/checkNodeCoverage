#include "node.h"

Node::Node() {}

QString Node::getName()
{
    return name;
}

NodeShape Node::getShape()
{
    return shape;
}

bool Node::isLeaf() const
{
    if (children.isEmpty())
        return true;

    return false;
}


Node::Node(QString newName, NodeShape newShape)
{
    name = newName;
    shape = newShape;
}

bool Node::operator== (Node &other)
{
    if (name == other.getName()
        && shape == other.getShape())
    {
        if (parent == other.parent)
        {
            if (children == other.children)
                return true;
        }
    }
    return false;
}
