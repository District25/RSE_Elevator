#ifndef MCU_CRITICAL_H
#define MCU_CRITICAL_H

#ifdef __cplusplus
extern "C" {
#endif

int inISR();                ///< Returns true if code is executed in an ISR (interrupt service routine).
void enterCritical();       ///< Disables interrupts globally.
void exitCritical();        ///< Enables interrupts globally.

#ifdef __cplusplus
}
#endif
#endif /* MCU_CRITICAL_H */
