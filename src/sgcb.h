/*
 * @Author: your name
 * @Date: 2021-06-17 20:23:55
 * @LastEditTime: 2021-06-17 22:32:10
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /iec61850/src/sgcb.h
 */
#ifndef __IEC_SGCB_H__
#define __IEC_SGCB_H__

#include "node.h"
#include <vector>

class ISgcbRW
{
public:
    ISgcbRW();
    ~ISgcbRW();
public:
    int writeData(IedConnection conn,const std::string &sg_ref, const FunctionalConstraint &fc, MmsValue *value);
    MmsValue *readData(IedConnection conn,const std::string &sg_ref,const FunctionalConstraint &fc);
};

class Sgcb : public INode,public ISgcbRW
{
public:
    explicit Sgcb(IedConnection conn,INode *parent = nullptr);
    virtual ~Sgcb();
public:
    virtual int init() final;
    int update();

    int writeNumSg(const uint8_t &numSg);
    int writeActSg(const uint8_t &actSg);
    int writeEditSg(const uint8_t &editSg);
    int writeConfirm(const bool &confirm);

    void setNumOfSg(const uint8_t &numOfSg);
    uint8_t getNumOfSg() const;

    void setActSg(const uint8_t &actSg);
    uint8_t getActSg() const;

    void setEditSg(const uint8_t &editSg);
    uint8_t getEditSg() const;

    void setConfirm(const bool &confirm);
    bool getConfirm() const;

    void setActTm(const time_t &actTm);
    time_t getActTm() const;

    void setActTmUs(const suseconds_t &actTmUs);
    suseconds_t getActTmUs() const;

    void setResvTms(const uint16_t &resvTms);
    uint16_t getResvTms() const;
private:
    uint8_t _numOfSg;
    uint8_t _actSg;
    uint8_t _editSg;
    bool _confirm;
    time_t _actTm;
    suseconds_t _actTmUs;
    uint16_t _resvTms;
};

class Sgcbs : public INode, public ISgcbRW
{
public:
    explicit Sgcbs(IedConnection conn,INode *model,INode *parent = nullptr);
    virtual ~Sgcbs();
public:
    virtual int init() final;
    int readSGCB(const std::string &ref,MmsValue *value);
    int writeSGCB(const std::string &ref,MmsValue *value,const uint8_t &num);
private:
    INode *getSgcbFromRef(const std::string &ref);
private:
    std::vector<INode *> _vecSg;
    INode *_model;
};

using SgcbsPtr = std::shared_ptr<Sgcbs>;

#endif // ! __IEC_SGCB_H__