#include <cstdlib>
#include <iostream>
#include <memory>
#include <telux/data/DataFactory.hpp>
/**
 * @file: DataApp.cpp
 *
 * @brief: Simple application to start the data call on the given profile id
 */
// Response callback for start or stop dataCall
void responseCallback(const std::shared_ptr<telux::data::IDataCall> &dataCall, telux::common::ErrorCode errorCode) {
    std::cout << "startCallResponse: errorCode: " << static_cast<int>(errorCode) << std::endl;
}
// Implementation of IDataConnectionListener
class DataConnectionListener : public telux::data::IDataConnectionListener {
 public:
    void onDataCallInfoChanged(const std::shared_ptr<telux::data::IDataCall> &dataCall) override {
        std::cout << "\n onDataCallInfoChanged";
        logDataCallDetails(dataCall);
    }

 private:
    void logDataCallDetails(const std::shared_ptr<telux::data::IDataCall> &dataCall) {
        std::cout << " ** DataCall Details **\n";
        std::cout << " ProfileID: " << dataCall->getProfileId() << std::endl;
        std::cout << " interfaceName: " << dataCall->getInterfaceName() << std::endl;
        std::cout << " DataCallStatus: " << (int)dataCall->getDataCallStatus() << std::endl;
        std::cout << " DataCallEndReason: Type = " << static_cast<int>(dataCall->getDataCallEndReason().type)
                  << std::endl;
        std::list<telux::data::IpAddrInfo> ipAddrList = dataCall->getIpAddressInfo();
        for (auto &it : ipAddrList) {
            std::cout << "\n ifAddress: " << it.ifAddress << "\n primaryDnsAddress: " << it.primaryDnsAddress
                      << "\n secondaryDnsAddress: " << it.secondaryDnsAddress << '\n';
        }
        std::cout << " IpFamilyType: " << static_cast<int>(dataCall->getIpFamilyType()) << '\n';
        std::cout << " TechPreference: " << static_cast<int>(dataCall->getTechPreference()) << '\n';
        std::cout << " DataBearerTechnology: " << static_cast<int>(dataCall->getCurrentBearerTech()) << '\n';
    }
};
int main(int argc, char *argv[]) {
    // [1] Get the DataFactory
    auto &dataFactory = telux::data::DataFactory::getInstance();
    auto dataConnMgr = dataFactory.getDataConnectionManager();
    // [2] Check if data subsystem is ready
    bool subSystemStatus = dataConnMgr->isSubsystemReady();
    // [2.1] If data subsystem is not ready, wait for it to be ready
    if (!subSystemStatus) {
        std::cout << "DATA subsystem is not ready" << std::endl;
        std::cout << "wait unconditionally for it to be ready " << std::endl;
        std::future<bool> f = dataConnMgr->onSubsystemReady();
        // If we want to wait unconditionally for data subsystem to be ready
        subSystemStatus = f.get();
    }
    // [3] Exit the application, if SDK is unable to initialize data subsystems
    if (subSystemStatus) {
        std::cout << " *** DATA Sub System is Ready *** " << std::endl;
    } else {
        std::cout << " *** ERROR - Unable to initialize data subsystem *** " << std::endl;
        return 1;
    }
    // [4] Register for Data listener
    std::shared_ptr<telux::data::IDataConnectionListener> dataListener = std::make_shared<DataConnectionListener>();
    dataConnMgr->registerListener(dataListener);
    // [5] Start data call on the mentioned profile id
    if (argc == 2) {
        int profileId = std::atoi(argv[1]);
        telux::data::IpFamilyType ipFamilyType = telux::data::IpFamilyType::IPV4;
        dataConnMgr->startDataCall(profileId, ipFamilyType, responseCallback);
    } else {
        std::cout << "\n Invalid argument!!! \n\n";
        std::cout << "\n Sample command is: \n";
        std::cout << "\n\t ./data_app <profieId> \n";
        std::cout << "\n\t ./data_app 1    --> to start the data call on profile Id 1\n";
    }
    // [6] Exit logic for the application
    std::cout << "\n\nPress ENTER to exit!!! \n\n";
    std::cin.ignore();
    // [7] Cleanup
    dataConnMgr->deregisterListener(dataListener);
    dataConnMgr = nullptr;
    return 0;
}