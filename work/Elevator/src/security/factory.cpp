#include "trace/trace.h"
#include "issuenotifier.h"
#include "app/factory.h"
#include "factory.h"

namespace security {

void Factory::preInitialize()
{
    // Note: The security::IssueNotifier needs to be created and initialised in
    //       an early stage. It may get called by observers trying to subscribe
    //       to security issues.
    getIssueNotifier().initialize();
}

// static
void Factory::initialize()
{
    Trace::out("Factory: Initializing security components...");

}

// static
void Factory::build()
{
    Trace::out("Factory: Starting security components...");

}

// static
IssueNotifier & Factory::getIssueNotifier()
{
    static IssueNotifier issueNotifier;
    return issueNotifier;
}


} // namespace security