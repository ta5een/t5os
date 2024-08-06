#pragma once

/**
 * Initializes the Boostrap Processor (BSP).
 *
 * Since this OS only supports one core at the moment, this is the only
 * initialization process required.
 */
void
cpu_init(void);
