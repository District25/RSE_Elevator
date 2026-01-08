#ifndef SECURITY_FACTORY_H
#define SECURITY_FACTORY_H

#include "issuenotifier.h"
#include "maximumliftmonitor.h"
#include "motorencodermonitor.h"
#include "floorlimitswitchesmonitor.h"   // NEW

namespace security {

class IssueNotifier;
class MaximumLiftMonitor;
class MotorEncoderMonitor;
class FloorLimitSwitchesMonitor;         // NEW

/**
 * @brief Factory class for the security package.
 */
class Factory
{
public:
    Factory() = delete;

    static void preInitialize();        ///< First stage initialization of important components.
    static void initialize();           ///< Initializes the factory components and its relations.
    static void build();                ///< Builds and starts the factory components.

protected:
    static IssueNotifier & getIssueNotifier();
    static MaximumLiftMonitor & getMaximumLiftMonitor();
    static MotorEncoderMonitor & getMotorEncoderMonitor();
    static FloorLimitSwitchesMonitor & getFloorLimitSwitchesMonitor();   // NEW
};

} // namespace security
#endif // SECURITY_FACTORY_H
