#ifndef MODALGUI_H__
#define MODALGUI_H__

#include "WProgram.h"

int showModalGui(char *line1, char *line2, int numOptions = 2);
int showModalGui_p(PGM_P line1, PGM_P line2, int numOptions = 2);

char *getNameModalGui(char *line1);

#endif /* MODALGUI_H__ */
