/*
	MIT License

	Copyright (c) 2022 Julian Scheffers

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#ifndef MCH2022_KEYB_H
#define MCH2022_KEYB_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <driver/spi_master.h>
#include <hal/spi_types.h>

#include "pax_keyboard.h"
#include <ili9341.h>
#include <pca9555.h>

/*

Input mappings:

Joystick:    Navigate keyboard / selection.
Joy. center: Shift key.
Accept:      Keypress.
Back:        Backspace.
Home/Menu:   Exit and reject input.
Start:       Exit and accept input.
Select:      Select keyboard (letters, numbers, symbols).

*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// Simple callback to get an input with the default keyboard.
// Title is optional.
// Will dynamically allocate the output.
// Returns NULL if the operation was cancelled.
char *mch_keyb_simple(pax_buf_t *buf, ILI9341 *screen, PCA9555 *io_expander, char *title);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //MCH2022_KEYB_H
