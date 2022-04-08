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

#include "mch2022_keyb.h"
#include <string.h>

// PCA9555 IO expander
#define PCA9555_PIN_BTN_START     5
#define PCA9555_PIN_BTN_SELECT    6
#define PCA9555_PIN_BTN_MENU      7
#define PCA9555_PIN_BTN_HOME      8
#define PCA9555_PIN_BTN_JOY_LEFT  9
#define PCA9555_PIN_BTN_JOY_PRESS 10
#define PCA9555_PIN_BTN_JOY_DOWN  11
#define PCA9555_PIN_BTN_JOY_UP    12
#define PCA9555_PIN_BTN_JOY_RIGHT 13
#define PCA9555_PIN_BTN_BACK      14
#define PCA9555_PIN_BTN_ACCEPT    15

// Simple callback to get an input with the default keyboard.
// Title is optional.
// Will dynamically allocate the output.
// Returns NULL if the operation was cancelled.
char *mch_keyb_simple(pax_buf_t *buf, ILI9341 *screen, PCA9555 *io_expander, char *title) {
	// Setup.
	char *retval = NULL;
	pkb_ctx_t kb_ctx;
	pkb_init(buf, &kb_ctx);
	
	// Input setup.
	bool last_up    = true;
	bool last_down  = true;
	bool last_left  = true;
	bool last_right = true;
	bool last_shift = true;
	bool last_acc   = true;
	bool last_back  = true;
	bool last_sel   = true;
	bool last_start = false;
	
	if (title) {
		// Calculate the title size.
		float      title_size = 18;
		pax_vec1_t dims       = pax_text_size(kb_ctx.text_font, title_size, title);
		if (dims.x > buf->width || dims.y > 18) {
			title_size = 9;
		}
		// Draw the title.
		dims = pax_text_size(kb_ctx.text_font, title_size, title);
		pax_background(buf, kb_ctx.bg_col);
		pax_draw_text(
			buf, kb_ctx.text_col,
			kb_ctx.text_font, title_size,
			(buf->width - dims.x) / 2, 0,
			title
		);
		pax_draw_line(buf, kb_ctx.text_col, 1, dims.y + 1, buf->width - 1, dims.y + 1);
		// Move the keyboard out of the way.
		kb_ctx.y      += dims.y + 2;
		kb_ctx.height -= dims.y + 2;
	}
	// Initial drawing.
	pkb_render(buf, &kb_ctx);
	ili9341_write(screen, buf->buf);
	
	while (1) {
		// Get inputs.
		bool up    = last_up;
		bool down  = last_down;
		bool left  = last_left;
		bool right = last_right;
		bool shift = last_shift;
		bool acc   = last_acc;
		bool back  = last_back;
		bool sel   = last_sel;
		bool start = last_start;
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_JOY_UP,    &up);
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_JOY_DOWN,  &down);
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_JOY_LEFT,  &left);
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_JOY_RIGHT, &right);
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_JOY_PRESS, &shift);
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_ACCEPT,    &acc);
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_BACK,      &back);
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_SELECT,    &sel);
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_START,     &start);
		
		// Handle inputs for the keyboard.
		if (!last_up    && up)    pkb_press(&kb_ctx, PKB_UP);
		if (!last_down  && down)  pkb_press(&kb_ctx, PKB_DOWN);
		if (!last_left  && left)  pkb_press(&kb_ctx, PKB_LEFT);
		if (!last_right && right) pkb_press(&kb_ctx, PKB_RIGHT);
		if (!last_shift && shift) pkb_press(&kb_ctx, PKB_SHIFT);
		if (!last_acc   && acc)   pkb_press(&kb_ctx, PKB_CHARSELECT);
		if (!last_back  && back)  pkb_press(&kb_ctx, PKB_DELETE_BEFORE);
		if (!last_sel   && sel)   pkb_press(&kb_ctx, PKB_MODESELECT);
		if (last_up    && !up)    pkb_release(&kb_ctx, PKB_UP);
		if (last_down  && !down)  pkb_release(&kb_ctx, PKB_DOWN);
		if (last_left  && !left)  pkb_release(&kb_ctx, PKB_LEFT);
		if (last_right && !right) pkb_release(&kb_ctx, PKB_RIGHT);
		if (last_shift && !shift) pkb_release(&kb_ctx, PKB_SHIFT);
		if (last_acc   && !acc)   pkb_release(&kb_ctx, PKB_CHARSELECT);
		if (last_back  && !back)  pkb_release(&kb_ctx, PKB_DELETE_BEFORE);
		if (last_sel   && !sel)   pkb_release(&kb_ctx, PKB_MODESELECT);
		
		// Handle redrawing the keyboard.
		pkb_loop(&kb_ctx);
		if (kb_ctx.dirty) {
			pkb_redraw(buf, &kb_ctx);
			ili9341_write(screen, buf->buf);
		}
		
		// Handle exit conditions.
		bool home = false;
		bool menu = false;
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_HOME, &home);
		pca9555_get_gpio_value(io_expander, PCA9555_PIN_BTN_MENU, &menu);
		if (home || menu) goto rejected;
		if ((kb_ctx.input_accepted && !acc) || (!start && last_start)) goto accepted;
		
		last_up    = up;
		last_down  = down;
		last_left  = left;
		last_right = right;
		last_shift = shift;
		last_acc   = acc;
		last_back  = back;
		last_sel   = sel;
		last_start = start;
	}
	
	accepted:
	retval = strdup(kb_ctx.content);
	
	rejected:
	pkb_destroy(&kb_ctx);
	
	return retval;
}
