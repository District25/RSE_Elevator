#ifndef SECURITY_MONITOR_H
#define SECURITY_MONITOR_H

#include "config/security-config.h"

namespace security {

/**
 * @brief Base class for all security monitors.
 */
class Monitor
{
public:
    Monitor(const IssueNumber & issueNumber);
    virtual ~Monitor() = default;

protected:
    void notifySecurityIssue(IssueNumber issueNumber);      ///< Reports a security issue.

    const IssueNumber ISSUE_NUMBER;                         ///< To be set in the derived monitor class.
};

} // namespace security
#endif // SECURITY_MONITOR_H
