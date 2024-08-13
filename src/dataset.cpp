#include "dataset.h"
#include "model.h"
#include "internal/log.h"

Fcda::Fcda(INode *parent)
    :INode(nullptr,NODE_FCDA,parent)
{
}

Fcda::~Fcda()
{
}

int Fcda::init()
{
    auto startPos = _ref.find_last_of("[");
    auto endPos = _ref.find_last_of("]");
    if(startPos == std::string::npos ||
        startPos == std::string::npos ||
        startPos >= endPos)
    {
        LOG_ERROR("Prase fcda ref error");
        return -1;
    }
    auto fc = _ref.substr(startPos + 1,endPos - 1);
    _fc = FunctionalConstraint_fromString(fc.c_str());
    _ref = _ref.substr(0,startPos);
    return 0;
}

void Fcda::setFc(const FunctionalConstraint &fc)
{
    if(_fc != fc)
    {
        _fc = fc;
    }
}

FunctionalConstraint Fcda::getFc() const
{
    return _fc;
}


Dataset::Dataset(IedConnection conn,INode *parent)
    :INode(conn,NODE_DATASET,parent)
{
}

Dataset::~Dataset()
{
}

int Dataset::init()
{
    IedClientError err;
    auto ref = getFullRef();
    auto lst = IedConnection_getDataSetDirectory(_conn,&err,
        getFullRef().c_str(),&_deletable);
    if (err != IED_ERROR_OK)
    {
        LOG_ERROR("Ied connection get datasets directory failed,the error code is : " + std::to_string(err));
        if (lst != NULL)
            LinkedList_destroy(lst);
        return -1;
    }
    if(!lst)
    {
        LOG_ERROR("Ied connection get datasets directory failed,the result is null.");
        return -1;
    }
    bool ret = 0;
    auto item = LinkedList_getNext(lst);
    while (item != NULL)
    {
        auto fcda = new Fcda();
        fcda->setRef((const char *)item->data);
        if(fcda->init() == -1)
        {
            LOG_ERROR("FCDA init failed.");
            ret = -1;
            break;
        }
        _childrens.push_back(fcda);
        item = ::LinkedList_getNext(item);
    }
    LinkedList_destroy(lst);
    return ret;
}

void Dataset::setDeletable(const bool &deletable)
{
    if(_deletable != deletable)
    {
        _deletable = deletable;
    }
}

bool Dataset::getDeletable() const
{
    return _deletable;
}

Datasets::Datasets(IedConnection conn,INode *model,INode *parent)
    :INode(conn,NODE_INVALID,parent),
    _model(model)
{

}

Datasets::~Datasets()
{
}

bool Datasets::datasetIsExist(const std::string &ref)
{
    auto isExist = false;
    for (auto it = _childrens.begin() ; it != _childrens.end(); it++)
    {
        if((*it)->getFullRef() == ref)
        {
            isExist = !isExist;
            break;
        }
    }
    return isExist;
}

Dataset *Datasets::getDataset(const std::string &ref)
{
    Dataset *dataset = nullptr;
    for (auto it = _childrens.begin(); it != _childrens.end(); it++)
    {
        if((*it)->getFullRef() == ref)
        {
            dataset = (Dataset *)(*it);
            break;
        }
    }
    return dataset;
}

int Datasets::init()
{
    IedClientError err;
    auto lst = ((Model *)_model)->getChildrens();
    for (auto it = lst.begin();it != lst.end();it++)
    {
        auto ld = *it;
        if(!ld)
        {
            LOG_WARN("Failed to obtain device node information through model.");
            continue;
        }
        auto ref = ld->getFullRef();
        auto lnlst = ld->getChildrens();
        for(auto it2 = lnlst.begin(); it2 != lnlst.end(); it2++)
        {
            auto ln = *it2;
            auto lst = IedConnection_getLogicalNodeDirectory(_conn,&err,
                ln->getFullRef().c_str(),ACSI_CLASS_DATA_SET);
            if (err != IED_ERROR_OK)
            {
                LOG_ERROR("Ied connection get LD failed.the error code is : " + std::to_string(err));
                if (lst != NULL)
                    LinkedList_destroy(lst);
                return -1;
            }
            if(!lst)
            {
                LOG_ERROR("Ied connection get LD failed.the LD is null.");
            }
            int ret = 0;
            auto item = LinkedList_getNext(lst);
            while (item != NULL)
            {
                auto dataset = new Dataset(_conn,this);
                auto ref = ln->getFullRef() + "." +(const char *)item->data;
                dataset->setRef(ref);
                if(dataset->init() == -1)
                {
                    LOG_ERROR("Init Datasets failed.");
                    ret = -1;
                    break;
                }
                _childrens.push_back(dataset);
                item = LinkedList_getNext(item);
            }
            LinkedList_destroy(lst);
            if(ret == -1)
            {
                return -1;
            }
        }
    }
    return 0;
}