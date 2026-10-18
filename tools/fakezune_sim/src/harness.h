#ifndef FAKEZUNE_HARNESS_H
#define FAKEZUNE_HARNESS_H

// Hooks the stubs use to report what the screens did back to the test runner.
void harness_note_open(const char *screen_name);
void harness_note_feedback(int kind);

#endif
