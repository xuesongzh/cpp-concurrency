/*
 *  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "DataConnectionManagerImpl.hpp"

#include <algorithm>
#include <chrono>
namespace telux {
namespace data {
DataConnectionManagerImpl::DataConnectionManagerImpl(int slotId) : slotId_(slotId) {
    LOG(DEBUG, __FUNCTION__);
    dsiClient_ = DsiClient::getDsiClient();
    wdsQmiClient_ = qmi::QmiClientFactory::getInstance().getWdsQmiClient();
}
DataConnectionManagerImpl::~DataConnectionManagerImpl() {
    LOG(DEBUG, __FUNCTION__);
}
void DataConnectionManagerImpl::setSubsystemReady(bool status) {
    LOG(DEBUG, __FUNCTION__, " status: ", status);
    std::lock_guard<std::mutex> lk(mtx_);
    ready_ = status;
    cv_.notify_all();
}
telux::common::Status DataConnectionManagerImpl::init() {
    LOG(INFO, __FUNCTION__);
    if (!isSubsystemReady()) {
        auto f = std::async(std::launch::async, [this]() { this->initSync(); }).share();
        taskQ_.add(f);
    }
    return telux::common::Status::SUCCESS;
}
void DataConnectionManagerImpl::initSync(bool fromSsr) {
    LOG(DEBUG, __FUNCTION__);
    if (wdsQmiClient_) {
        bool wdsQmiReady = wdsQmiClient_->isReady();
        if (!wdsQmiReady) {
            // blocking infinite wait for subsystem to be ready
            std::future<bool> f = wdsQmiClient_->onReady();
            wdsQmiReady = f.get();
        }
        wdsQmiClient_->registerListener(shared_from_this());
    }
    dsiClient_->init(fromSsr);
    dsiClient_->registerListener(shared_from_this());
    setSubsystemReady(true);
}
telux::common::Status DataConnectionManagerImpl::cleanup() {
    LOG(DEBUG, __FUNCTION__);
    setSubsystemReady(false);
    if (dsiClient_) {
        dsiClient_->cleanup();
    }
    if (dsiClient_) {
        dsiClient_->deregisterListener(shared_from_this());
    }
    return telux::common::Status::SUCCESS;
}
bool DataConnectionManagerImpl::isSubsystemReady() {
    return ready_;
}
std::future<bool> DataConnectionManagerImpl::onSubsystemReady() {
    auto future = std::async(std::launch::async, [&] { return DataConnectionManagerImpl::waitForInitialization(); });
    return future;
}
bool DataConnectionManagerImpl::waitForInitialization() {
    LOG(INFO, __FUNCTION__);
    std::unique_lock<std::mutex> lock(mtx_);
    if (!isSubsystemReady()) {
        cv_.wait(lock);
    }
    return isSubsystemReady();
}
telux::common::Status DataConnectionManagerImpl::startDataCall(int profileId, IpFamilyType ipFamilyType,
                                                               DataCallResponseCb callback, OperationType oprType,
                                                               std::string apn) {
    LOG(INFO, __FUNCTION__, "Profile Id: ", profileId, ", IP family type: ", static_cast<int>(ipFamilyType),
        ", Operation type: ", static_cast<int>(oprType), ", apn: ", apn);
    if (!isSubsystemReady()) {
        LOG(ERROR, __FUNCTION__, " Data subsystem not ready");
        return telux::common::Status::NOTREADY;
    }
    if (profileId < 0) {
        LOG(ERROR, __FUNCTION__, " Invalid profile id");
        return telux::common::Status::INVALIDPARAM;
    }
    if (ipFamilyType != IpFamilyType::IPV4 && ipFamilyType != IpFamilyType::IPV6 &&
        ipFamilyType != IpFamilyType::IPV4V6) {
        LOG(ERROR, __FUNCTION__, " Invalid ip family type");
        return telux::common::Status::INVALIDPARAM;
    }
    // Remote is not allowed in DSI_netctrl library
    if (oprType == OperationType::DATA_REMOTE) {
        return telux::common::Status::NOTALLOWED;
    }
    return dsiClient_->startDataCall(profileId, static_cast<int>(ipFamilyType), callback);
}
telux::common::Status DataConnectionManagerImpl::stopDataCall(int profileId, IpFamilyType ipFamilyType,
                                                              DataCallResponseCb callback, OperationType oprType,
                                                              std::string apn) {
    LOG(INFO, __FUNCTION__, "Profile Id: ", profileId, ", IP family type: ", static_cast<int>(ipFamilyType),
        ", Operation type: ", static_cast<int>(oprType), ", apn: ", apn);
    if (!isSubsystemReady()) {
        LOG(ERROR, __FUNCTION__, " Data subsystem not ready");
        return telux::common::Status::NOTREADY;
    }
    if (profileId < 0) {
        LOG(ERROR, __FUNCTION__, " Invalid profile id");
        return telux::common::Status::INVALIDPARAM;
    }
    if (ipFamilyType != IpFamilyType::IPV4 && ipFamilyType != IpFamilyType::IPV6 &&
        ipFamilyType != IpFamilyType::IPV4V6) {
        LOG(ERROR, __FUNCTION__, " Invalid ip family type");
        return telux::common::Status::INVALIDPARAM;
    }
    // Remote is not allowed in DSI_netctrl library
    if (oprType == OperationType::DATA_REMOTE) {
        return telux::common::Status::NOTALLOWED;
    }
    return dsiClient_->stopDataCall(profileId, static_cast<int>(ipFamilyType), callback);
}
telux::common::Status DataConnectionManagerImpl::registerListener(std::weak_ptr<IDataConnectionListener> listener) {
    if (!isSubsystemReady()) {
        LOG(ERROR, __FUNCTION__, " Data subsystem not ready");
        return telux::common::Status::NOTREADY;
    }
    if (listener.lock()) {
        std::lock_guard<std::mutex> lock(mtx_);
        // Check whether the listener existed
        auto itr = std::find_if(std::begin(listeners_), std::end(listeners_),
                                [=](std::weak_ptr<IDataConnectionListener> listenerExisted) {
                                    return (listenerExisted.lock() == listener.lock());
                                });
        // if the given listener object is not available in the listeners_ list
        // then add new listener entry
        if (itr == std::end(listeners_)) {
            LOG(DEBUG, "Updating listener");
            listeners_.emplace_back(listener);  // store listener
        }
    } else {
        LOG(ERROR, "Null listener");
        return telux::common::Status::INVALIDPARAM;
    }
    return telux::common::Status::SUCCESS;
}
telux::common::Status DataConnectionManagerImpl::deregisterListener(std::weak_ptr<IDataConnectionListener> listener) {
    if (!isSubsystemReady()) {
        LOG(ERROR, __FUNCTION__, " Data subsystem not ready");
        return telux::common::Status::NOTREADY;
    }
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto it = listeners_.begin(); it != listeners_.end();) {
        auto sp = (*it).lock();
        if (!sp) {  //
            LOG(ERROR, "listener doesn't exists");
            it = listeners_.erase(it);
        } else if (sp == listener.lock()) {
            it = listeners_.erase(it);
            LOG(DEBUG, "successfully removed the listener");
            return telux::common::Status::SUCCESS;
        } else {
            ++it;
        }
    }
    LOG(ERROR, "deregisterListener: listener not found");
    return telux::common::Status::NOSUCH;
}
int DataConnectionManagerImpl::getSlotId() {
    return slotId_;
}
void DataConnectionManagerImpl::getAvailableListeners(
    std::vector<std::shared_ptr<IDataConnectionListener>> &listeners) {
    LOG(DEBUG, __FUNCTION__, " listeners size : ", listeners_.size());
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto it = listeners_.begin(); it != listeners_.end();) {
        auto sp = (*it).lock();
        if (sp) {
            listeners.emplace_back(sp);
            ++it;
        } else {
            LOG(DEBUG, "erased obsolete weak pointer from DataConnectionManagerImpl's listeners");
            it = listeners_.erase(it);
        }
    }
}
void DataConnectionManagerImpl::onDataCallInfoChanged(const std::shared_ptr<IDataCall> &dataCall) {
    LOG(DEBUG, __FUNCTION__);
    std::vector<std::shared_ptr<IDataConnectionListener>> applisteners;
    getAvailableListeners(applisteners);
    for (auto &listener : applisteners) {
        listener->onDataCallInfoChanged(dataCall);
    }
}
void DataConnectionManagerImpl::onServiceStatusChange(telux::common::ServiceStatus status) {
    LOG(DEBUG, __FUNCTION__);
    std::vector<std::shared_ptr<IDataConnectionListener>> applisteners;
    getAvailableListeners(applisteners);
    for (auto &listener : applisteners) {
        listener->onServiceStatusChange(status);
    }
}
void DataConnectionManagerImpl::onWdsServiceStatusChange(telux::common::ServiceStatus status) {
    LOG(DEBUG, __FUNCTION__);
    switch (status) {
        case telux::common::ServiceStatus::SERVICE_UNAVAILABLE:
            LOG(INFO, __FUNCTION__, " SERVICE UNAVAILABLE");
            if (dsiClient_) {
                dsiClient_->resetAndNotifyDataCalls();
            }
            cleanup();
            onServiceStatusChange(telux::common::ServiceStatus::SERVICE_UNAVAILABLE);
            break;
        case telux::common::ServiceStatus::SERVICE_AVAILABLE:
            LOG(INFO, __FUNCTION__, " SERVICE AVAILABLE");
            initSync(true);
            onServiceStatusChange(telux::common::ServiceStatus::SERVICE_AVAILABLE);
            break;
        default:
            LOG(DEBUG, __FUNCTION__, " Unknown service");
            break;
    }
}
telux::common::Status DataConnectionManagerImpl::requestDataCallList(OperationType type,
                                                                     DataCallListResponseCb callback) {
    LOG(DEBUG, __FUNCTION__);
    return telux::common::Status::NOTIMPLEMENTED;
}
}  // namespace data
}  // namespace telux