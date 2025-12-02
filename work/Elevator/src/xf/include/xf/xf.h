#ifndef XF_XF_H
#define XF_XF_H

#include "config/xf-config.h"

//
// What is seen only by the C++ compiler
//
#ifdef __cplusplus

#include "xf/interface/dispatcher.h"

/** \mainpage RSE Execution Framework Documentation
 *
 * \section sec_xf_intro Introduction
 *
 * This library implements an e<b>X</b>ecution <b>F</b>ramework (XF) to drive finite
 * state machines. Using this framework, it is possible to drive multiple state machines
 * in a concurrent manner (pseudo-parallel) without having an operating system.
 *
 * The XF defines an interface which gives the ability to reimplement parts of the XF to
 * make changes to an other target/platform.
 *
 * \section sec_xf_comp XF Component Diagram
 * The XF is split into two parts. One part is the **%XF core** containing classes which
 * never change across platforms. The other part is the **%XF port** containing classes
 * adjusted for a specific platform:
 *
 *  \image html comp-simple-xf.png "Component Class Diagram"
 *
 * The XF core together with a port forms a complete XF which can be integrated into
 * a software project.
 *
 * \section sec_xf_cmd XF Class Diagram
 *
 * Following class diagram shows the basic relations between the most important classes
 * in the RSE XF. It shall give you a basic understanding about the relations between
 * these classes:
 *
 * \image html cmd-xf-simplified.png "XF Class Diagram"
 *
 * \section sec_xf_start Starting Point
 *
 * A good point to jump into the definitions of the classes is the `XF` class. Other classes
 * of interest are `interface::XFDispatcher`, `interface::XFBehavior` and XFEvent.
 *
 * \section sec_xf_external_links Links to related Documentation
 * \if DOC_WITH_TESTBENCH
 * - <a href="../../../../../test-bench/doxygen/output/html/index.html">Test Bench Documentation</a>
 * \endif
 * - <a href="../../../../../src/mdw/trace/doxygen/output/html/index.html">Trace Documentation</a>
 *
 * \if DOC_WITH_RELEASE_HISTORY
 * \section sec_xf_releases XF Releases
 * \subsection xf_v_2_0_0 XF Version: 2.0.0 (2024-09)
 * - sth: Updated XF interface. Removed XFTimeout class. Now every XFEvent can be delayed.
 * - sth: Renamed 'Simplified %XF' to '%XF'
 * - sth: Two ports available: Zephyr and Qt 
 * \subsection xf_v_1_1_0 Simplified XF Version: 1.1.0 (2022-09)
 * - sth: Removed some no more neeted attributes and parameters from classes
 * - sth: Added support for MSVC compiler
 * - sth: Updated Qt port to support Qt library v6.x
 * - sth: Added for Qt port a TRACE_ENABLE_TIMESTAMP define to output traces with a timestamp
 * \subsection xf_v_1_0_0 Simplified XF Version: 1.0.0 (2021-07)
 * - sth: Removed from 'Full %XF' the OS support and created with that the 'Simplified %XF'
 * \endif
 */

/** @ingroup xf_core
 *  @{
 */

/**
 * @brief Static class grouping the basic methods for the XF together.
 */
class XF
{
public:
    /**
     * Initializes the e<b>X</b>ecution <b>F</b>ramework (XF). Call
     * this method prior to initialize the other objects of the system.
     *
     * @param timeInterval The interval in milliseconds with which the TimeoutManager should run.
     * @param argc Optional parameter to pass application argument number to XF.
     * @param argv Optional parameter to pass application arguments to XF.
     */
    static void initialize(int timeInterval = 10, int argc = 0, char * argv[] = nullptr);

    /**
     * Starts execution of the framework. This
     * results in processing the events in main loop.
     *
     * This method is blocking.
     */
    static int exec();

    /**
     * Executes once the dispatcher. To be called regularly in a while loop.
     *
     * This method is non-blocking.
     */
    static int execOnce();


    /**
     * @brief Returns true in case the XF is initialized and running.
     */
    static bool isRunning();

protected:
    static bool isInitialized_;         ///< Changes from false to true after calling method initialize(int). Used to handle multiple calls to init(int).
    static bool isRunning_;             ///< Tells if the XF is initialized and running.
};

#endif  // __cplusplus

//
// What is seen by the C and C++ compiler
//
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void XF_initialize(int timeInterval);       ///< Initializes XF within C code
void XF_exec();                             ///< Calls XF execution in C code (blocking call)
void XF_execOnce();                         ///< Calls XF execution in C code (non-blocking call)

#ifdef __cplusplus
}
#endif // __cplusplus

/** @} */ // end of xf_core group
#endif // XF_XF_H
