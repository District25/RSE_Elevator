#ifndef SECURITY_FACTORY_H
#define SECURITY_FACTORY_H

namespace security {

class IssueNotifier;

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
    
    // TODO: Add static methods returning the monitor objects here
};

} // namespace security
#endif // SECURITY_FACTORY_H