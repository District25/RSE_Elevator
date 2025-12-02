#include "issuenotifier.h"

namespace security {

IssueNotifier * IssueNotifier::instance(nullptr);

IssueNotifier::IssueNotifier():
    observerCount(0),
    observer {}
{
    assert(!instance);      // Singleton pattern. Only one instance allowed
    instance = this;        // Store 'this' to static pointer
}

bool IssueNotifier::initialize()
{
    return true;
}

bool IssueNotifier::subscribe(SecurityIssueObserver *observer)
{
    if (observer != nullptr and observerCount < MAX_OBSERVERS)
    {
        for (int i = observerCount; i < MAX_OBSERVERS; i++)
        {
            if (this->observer[i] == nullptr)
            {
                this->observer[i] = observer;
                observerCount++;
                return true;
            }
        }
    }
    return false;
}

void IssueNotifier::unsubscribe(SecurityIssueObserver * observer)
{
    if (observerCount > 0 and observer != nullptr)
    {
        for (int i = 0; i < MAX_OBSERVERS; i++)
        {
            if (this->observer[i] == observer)
            {
                this->observer[i] = nullptr;
                observerCount--;
                break;
            }
        }
    }
}

void IssueNotifier::notifySecurityIssue(IssueNumber issueNumber)
{
        for (int i = 0; i < MAX_OBSERVERS; i++)
    {
        if (observer[i] != nullptr)
        {
            observer[i]->onSecurityIssue(issueNumber);
        }
    }
}

} // namespace security
