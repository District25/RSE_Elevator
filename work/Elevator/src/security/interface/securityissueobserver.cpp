#include "security/issuenotifier.h"
#include "securityissueobserver.h"

namespace interface {

bool SecurityIssueObserver::subscribeToNotifier()
{
    return security::IssueNotifier::getInstance().subscribe(this);
}

void SecurityIssueObserver::unsubscribeFromNotifier()
{
    security::IssueNotifier::getInstance().unsubscribe(this);
}

} // namespace interface
