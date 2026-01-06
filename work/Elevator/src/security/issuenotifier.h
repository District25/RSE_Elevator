#ifndef SECURITY_ISSUE_NOTIFIER_H
#define SECURITY_ISSUE_NOTIFIER_H

#include <cstdint>
#include <cassert>
#include "interface/securityissueobserver.h"

namespace security {

/**
 * @brief The SecurityIssueNotifier class is responsible for notifying other parts of the system about security issues.
 * 
 * Implements the Singleton pattern.
 */
class IssueNotifier
{
    friend class interface::SecurityIssueObserver;  // Using subscribe() and unsubscribe()
    friend class Monitor;   // Only classes inheritting from Monitor are allowed to use the IssueNotifier

    using SecurityIssueObserver = interface::SecurityIssueObserver;
public:
    IssueNotifier();

    bool initialize();

    static IssueNotifier & getInstance() { assert(instance); return *instance; }    ///< Access to single instance.

    // Methods used by the SecurityIssueObserver
protected:
    bool subscribe(SecurityIssueObserver * observer);                   ///< Registers a new observer. Returns true on success.
    void unsubscribe(SecurityIssueObserver * observer);                 ///< Un-subscribes a registered observer.

    // Methods used by the security::Monitor and its specialisations
protected:
    void notifySecurityIssue(IssueNumber issueNumber);                  ///< Notifies observers about a security issue.

protected:
    static IssueNotifier * instance;                                    ///< Pointer to single instance.

    static constexpr const uint8_t MAX_OBSERVERS = 8;                   ///< Maximum allowed observers.
    uint8_t observerCount;                                              ///< Stores how many observers are subscribed.
    interface::SecurityIssueObserver * observer[MAX_OBSERVERS];         ///< Array holding subscribed observers.

private:
    bool issueAlreadySent = false;
};  

} // namespace security
#endif // SECURITY_ISSUE_NOTIFIER_H
