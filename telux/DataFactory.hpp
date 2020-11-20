/**
 * @file       DataFactory.hpp
 *
 * @brief      DataFactory is the central factory to create all data instances
 *
 */
#ifndef DATAFACTORY_HPP
#define DATAFACTORY_HPP
#include <map>
#include <memory>
#include <mutex>
#include <telux/common/CommonDefines.hpp>
#include <telux/data/DataConnectionManager.hpp>
#include <telux/data/DataFilterManager.hpp>
#include <telux/data/DataProfileManager.hpp>
#include <telux/data/IpFilter.hpp>
namespace telux {
namespace data {
/** @addtogroup telematics_data
 * @{ */
/**
 *@brief DataFactory is the central factory to create all data classes
 *
 */
class DataFactory {
 public:
    /**
     * Get Data Factory instance.
     */
    static DataFactory &getInstance();
    /**
     * Get Data Connection Manager
     *
     * @param [in] slotId    Unique identifier for the SIM slot
     *
     * @returns instance of IDataConnectionManager
     *
     */
    std::shared_ptr<IDataConnectionManager> getDataConnectionManager(int slotId = DEFAULT_SLOT_ID);
    /**
     * Get Data Profile Manager
     *
     * @param [in] slotId    Unique identifier for the SIM slot
     *
     * @returns instance of IDataProfileManager
     *
     */
    std::shared_ptr<IDataProfileManager> getDataProfileManager(int slotId = DEFAULT_SLOT_ID);
    /**
     * Get Data Filter Manager instance
     *
     * @param [in] slotId    Unique identifier for the SIM slot
     *
     * @returns instance of IDataFilterManager.
     *
     * @note    Eval: This is a new API and is being evaluated. It is subject to
     * change and could break backwards compatibility.
     */
    std::shared_ptr<IDataFilterManager> getDataFilterManager(int slotId = DEFAULT_SLOT_ID);
    /**
     * Get IIpFilter instance based on IP Protocol
     *
     * @param [in] proto    @ref telux::data::IpProtocol
     *                      Some sample protocol values are
     *                      ICMP = 1    # Internet Control Message Protocol - RFC 792
     *                      IGMP = 2    # Internet Group Management Protocol - RFC 1112
     *                      TCP = 6     # Transmission Control Protocol - RFC 793
     *                      UDP = 17    # User Datagram Protocol - RFC 768
     *                      ESP = 50    # Encapsulating Security Payload - RFC 4303
     *
     * @returns instance of IIpFilter based on IpProtocol filter (i.e TCP, UDP)
     *
     * @note     Eval: This is a new API and is being evaluated.It is subject to change and could
     *           break backwards compatibility.
     */
    std::shared_ptr<IIpFilter> getNewIpFilter(IpProtocol proto);

 private:
    // mutex to protect member variables
    std::mutex dataMutex_;
    std::shared_ptr<IDataConnectionManager> dataConnectionManager_;
    std::shared_ptr<IDataProfileManager> dataProfileManager_;
    DataFactory();
    ~DataFactory();
    DataFactory(const DataFactory &) = delete;
    DataFactory &operator=(const DataFactory &) = delete;
};
/** @} */ /* end_addtogroup telematics_data */
}  // namespace data
}  // namespace telux
#endif