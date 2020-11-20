/*
 *  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/**
 * @file       DataConnectionManagerImpl.hpp
 *
 * @brief      This is the implementation class for IDataConnectionManager
 *
 */
#ifndef DATACONNECTIONMANAGERIMPL_HPP
#define DATACONNECTIONMANAGERIMPL_HPP
#include <memory>
#include <mutex>
#include <string>
#include <telux/data/DataConnectionManager.hpp>

#include "common/AsyncTaskQueue.hpp"
#include "common/Logger.hpp"
#include "qmi/QmiClientFactory.hpp"
#ifdef FEATURE_DATA_QCMAP
#include "QcmClient.hpp"
#else
#include "DsiClient.hpp"
#endif
#include "DataCallImpl.hpp"
namespace telux {
namespace data {
class DataConnectionManagerImpl : public IDataConnectionManager,
                                  public IDataConnectionListener,
                                  public telux::qmi::IQmiWdsListener,
                                  public std::enable_shared_from_this<DataConnectionManagerImpl> {
 public:
    DataConnectionManagerImpl(int slotId);
#ifdef FEATURE_DATA_QCMAP
    DataConnectionManagerImpl(int slotId, std::shared_ptr<QcmClient> qcmapClient);
#endif
    ~DataConnectionManagerImpl();
    bool isSubsystemReady();
    std::future<bool> onSubsystemReady();
    telux::common::Status startDataCall(int profileId, IpFamilyType ipFamilyType = IpFamilyType::IPV4V6,
                                        DataCallResponseCb callback = nullptr,
                                        OperationType type = OperationType::DATA_LOCAL, std::string apn = "");
    telux::common::Status stopDataCall(int profileId, IpFamilyType ipFamilyType = IpFamilyType::IPV4V6,
                                       DataCallResponseCb callback = nullptr,
                                       OperationType type = OperationType::DATA_LOCAL, std::string apn = "");
    telux::common::Status registerListener(std::weak_ptr<IDataConnectionListener> listener);
    telux::common::Status deregisterListener(std::weak_ptr<IDataConnectionListener> listener);
    int getSlotId();
    telux::common::Status requestDataCallList(OperationType type, DataCallListResponseCb callback);
    telux::common::Status init();
    telux::common::Status cleanup();
    void onDataCallInfoChanged(const std::shared_ptr<IDataCall> &dataCall) override;
    void onServiceStatusChange(common::ServiceStatus status) override;
    void onWdsServiceStatusChange(telux::common::ServiceStatus status) override;

 private:
    std::mutex mtx_;
    std::condition_variable cv_;
    int slotId_ = DEFAULT_SLOT_ID;
    bool ready_ = false;
#ifdef FEATURE_DATA_QCMAP
    std::shared_ptr<QcmClient> qcmClient_ = nullptr;
#else
    std::shared_ptr<DsiClient> dsiClient_ = nullptr;
#endif
    std::vector<std::weak_ptr<IDataConnectionListener>> listeners_;
    std::shared_ptr<telux::qmi::WdsQmiClient> wdsQmiClient_ = nullptr;
    telux::common::AsyncTaskQueue<void> taskQ_;
    /**
     * Perform synchronous initialization
     *
     * @param [in] fromSsr        indicates whether this method is invoked after SSR or not
     */
    void initSync(bool fromSsr = false);
    void setSubsystemReady(bool status);
    bool waitForInitialization();
    void getAvailableListeners(std::vector<std::shared_ptr<IDataConnectionListener>> &listeners);
};
}  // namespace data
}  // namespace telux
#endif