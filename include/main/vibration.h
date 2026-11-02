#ifndef INCLUDE_MAIN_VIBRATION_H
#define INCLUDE_MAIN_VIBRATION_H

void Vibration_Set_Weak(int duration);
void VibrationEngine(void);

/* Functions and data used by other translation units. */
void Vibration_Stop(void);
void Vibration_Set_Strong(int start, int end, int duration);

#endif /* INCLUDE_MAIN_VIBRATION_H */
