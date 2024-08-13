/*
 * @Author: your name
 * @Date: 2021-06-16 22:19:49
 * @LastEditTime: 2021-06-17 21:52:59
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /iec61850/src/Node.h
 */
#ifndef __IEC_INODE_H__
#define __IEC_INODE_H__

#include <memory>
#include <list>
#include <string>
#include <libiec61850/iec61850_client.h>

// extern IedConnection IedConn;

typedef enum
{
    NODE_INVALID = 0,
    NODE_LD,
    NODE_LN,
    NODE_DO,
    NODE_DA,
    NODE_DATASET,
    NODE_FCDA,
    NODE_BRCB,
    NODE_URCB,
    NODE_SGCB,
    NODE_SGCBS
} NODE_TYPE;

// 节点基类
class INode : std::enable_shared_from_this<INode>
{
    friend class Model;
    friend class LDa;
    friend class LDo;
    friend class LNode;
    friend class LDevice;
    friend class Datasets;
    friend class Dataset;
    friend class Fcda;
    friend class Reports;
    friend class Rcb;
    friend class Brcb;
    friend class Urcb;
    friend class Sgcb;
    friend class Sgcbs;
public:
    explicit INode(IedConnection conn = nullptr,const NODE_TYPE &nodeType = NODE_INVALID,INode *parent = nullptr);
    virtual ~INode();

    virtual int init() = 0;
public:
    void setRef(const std::string &ref);
    std::string getRef() const;
    std::string getFullRef() const;

    void setType(const NODE_TYPE &type);
    NODE_TYPE getType() const;

    void setParent(INode *parent);
    INode *getParent() const;

    bool empty();
    void setChildrens(const std::list<INode *> &listChildren);
    std::list<INode *> getChildrens() const;
    
    std::list<INode *> getChildrensFromType(const NODE_TYPE &type);

    INode * findParentNodeFromType(const NODE_TYPE &type);
    INode * getSubNodeFromRef(const std::string &ref);
private:
    std::string _ref;                       // 引用
    NODE_TYPE _type;                        // 节点类型
    INode *_parent;                         // 父节点
    std::list<INode *> _childrens;          // 子节点列表

    IedConnection _conn;                    // 句柄
};

#endif // ! __IEC_INODE_H__