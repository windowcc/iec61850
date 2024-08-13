#include "node.h"

INode::INode(IedConnection conn,const NODE_TYPE &nodeType,INode * parent)
    :_type(nodeType),
    _parent(parent),
    _conn(conn)
{
}

INode::~INode()
{
    if(_parent)
    {
        _parent = nullptr;
    }
    if(!_childrens.empty())
    {
        for (auto it = _childrens.begin(); it != _childrens.end(); it++)
        {
            delete *it;
            *it = nullptr;
        }
        _childrens.clear();
    }
}

void INode::setRef(const std::string &ref)
{
    if(_ref != ref)
    {
        _ref = ref;
    }
}

std::string INode::getRef() const
{
    return _ref;
}

std::string INode::getFullRef() const
{
    std::string fullRef = "";
    std::string split = "";
    split = _parent->getType() == NODE_LD ? "/" : ".";
    if(_parent)
    {
        fullRef = _parent->getParent() ? _parent->getFullRef() + split + _ref : _ref;
    }
    return fullRef;
}

void INode::setType(const NODE_TYPE &type)
{
    if(_type != type)
    {
        _type = type;
    }
}

NODE_TYPE INode::getType() const
{
    return _type;
}

void INode::setParent(INode *parent)
{
    if(_parent != parent)
    {
        _parent = parent;
    }
}

INode * INode::getParent() const
{
    return _parent;
}

bool INode::empty()
{
    return _childrens.empty();
}

void INode::setChildrens(const std::list<INode *> &listChildren)
{
    _childrens = listChildren;
}

std::list<INode *> INode::getChildrens() const
{
    return _childrens;
}

std::list<INode *> INode::getChildrensFromType(const NODE_TYPE &type)
{
    std::list<INode *> lst;
    for (auto it = _childrens.begin(); it != _childrens.end(); it++)
    {
        auto node = *it;
        if(node->getType() == type)
        {
            lst.push_back(node);
        }
        else
        {
            auto tmplst = node->getChildrensFromType(type);
            lst.merge(tmplst);
        }
    }
    return lst;
}

INode *INode::findParentNodeFromType(const NODE_TYPE &type)
{
    INode *parent = nullptr;
    while (_parent)
    {
        if(_parent->getType() == type)
        {
            parent = _parent;
            break;
        }
    }
    return parent;
}

INode *INode::getSubNodeFromRef(const std::string &ref)
{
    INode *result = nullptr;
    for (auto it = _childrens.begin(); it != _childrens.end(); it++)
    {
        if (ref == (*it)->getRef())
        {
            result = *it;
            break;
        }
    }
    return result;
}