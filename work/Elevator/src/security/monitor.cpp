#include "issuenotifier.h"
#include "monitor.h"

namespace security {
Monitor::Monitor(const IssueNumber & issueNumber):
    ISSUE_NUMBER(issueNumber)
{}

void Monitor::notifySecurityIssue(IssueNumber issueNumber)
{
    IssueNotifier::getInstance().notifySecurityIssue(issueNumber);
}

} // namespace security
