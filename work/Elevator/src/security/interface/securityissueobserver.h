#ifndef INTERFACE_SECURITY_ISSUE_OBSERVER_H
#define INTERFACE_SECURITY_ISSUE_OBSERVER_H

#include "config/security-config.h"

namespace security { class IssueNotifier; }

namespace interface {

/**
 * @brief Interface to be provided by the class who wants to be notified about security issues.
 */
class SecurityIssueObserver
{
    friend class security::IssueNotifier;
public:
    virtual ~SecurityIssueObserver() = default;

protected:
    bool subscribeToNotifier();
    void unsubscribeFromNotifier();

protected:
    virtual void onSecurityIssue(IssueNumber issueNumber) = 0;      ///< Is called if a security problem has been detected.

protected:
    SecurityIssueObserver() = default;                              ///< Not allowing to instantiate object of interface.
};

} // namespace interface
#endif // INTERFACE_SECURITY_ISSUE_OBSERVER_H
