/*
 * @Author: your name
 * @Date: 2021-06-17 21:54:59
 * @LastEditTime: 2021-06-17 22:30:07
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /iec61850/src/dataset.h
 */
#ifndef __IEC_DATASET_H__
#define __IEC_DATASET_H__

#include "node.h"

class Fcda : public INode
{
public:
    Fcda(INode *parent = nullptr);
    virtual ~Fcda();
public:
    virtual int init() final;

    void setFc(const FunctionalConstraint &fc);
    FunctionalConstraint getFc() const;
private:
    FunctionalConstraint _fc;
};

using FcdaPtr = std::shared_ptr<Fcda>;

class Dataset : public INode
{
public:
    Dataset(IedConnection conn,INode *parent = nullptr);
    virtual ~Dataset();
public:
    virtual int init() final;

    void setDeletable(const bool &deletable);
    bool getDeletable() const;
private:
    std::string _name;          // 数据集名称
    std::string _desc;          // 
    bool _deletable;            // 数据集是否可以手动删除
};

using DataSetPtr = std::shared_ptr<Dataset>;

class Datasets : public INode
{
public:
    Datasets(IedConnection conn,INode *model,INode *parent = nullptr);
    virtual ~Datasets();
public:
    bool datasetIsExist(const std::string &ref);
    Dataset *getDataset(const std::string &ref);
public:
    virtual int init() final;
private:
    INode *_model;
};

using DatasetsPtr = std::shared_ptr<Datasets>;

#endif // !__IEC_DATASET_H__