/**
 * \file liblvgl/llemu.h
 * \ingroup c-llemu
 * 
 * Legacy LCD Emulator
 *
 * \details This file defines a high-level API for emulating the three-button, UART-based
 * VEX LCD, containing a set of functions that facilitate the use of a software-
 * emulated version of the classic VEX LCD module.
 *
 * Visit https://pros.cs.purdue.edu/v5/tutorials/topical/adi.html to learn more.
 *
 * This file should not be modified by users, since it gets replaced whenever
 * a kernel upgrade occurs.
 *
 * \copyright (c) 2017-2023, Purdue University ACM SIGBots.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * 
 * \defgroup c-llemu LLEMU C API
 * @{
 * LLEMU - <B>L</B>egacy <B>L</B>cd <B>EMU</B>lator
 * 
 * \image html llemu/llemu-3.8.png
 * 
 * LLEMU provides a virtual 40x8 LCD screen with 3 buttons. The user can set the
 * text of the screen and set create functions that are run when the buttons are
 * pressed. 
 * 
 * LLEMU is a emulation of the UART-based LCD screens that were available with 
 * VEX's cortex product line.
 * @}
 */

#ifndef _LIBLVGL_LLEMU_H_
#define _LIBLVGL_LLEMU_H_

#include <errno.h>
#include <stdbool.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include "liblvgl/lvgl.h"
#pragma GCC diagnostic pop

#ifdef __cplusplus
extern "C" {
namespace pros {
#endif

typedef void (*lcd_btn_cb_fn_t)(void);

#define LCD_BTN_LEFT 4
#define LCD_BTN_CENTER 2
#define LCD_BTN_RIGHT 1

typedef struct lcd_s {
	lv_obj_t* frame;
	lv_obj_t* screen;
	lv_obj_t* lcd_text[8];
	lv_obj_t* btn_container;
	lv_obj_t* btns[3];             // < 0 => left; 1 => center; 2 => right
	lcd_btn_cb_fn_t callbacks[3];  // < 0 => left; 1 => center; 2 => right
	volatile uint8_t touch_bits;   // < 4 => left; 2 => center; 1 => right (no
	                               // multitouch support)
} lcd_s_t;

/**
 * \ingroup c-llemu 
 */

/**
 * \addtogroup c-llemu
 *  @{
 */

/**
 * \enum lcd_text_align_e
 * 
 * @brief Represents how to align the text in the LCD 
 */
typedef enum lcd_text_align_e {
	/// Align the text to the left side of LCD line
	LCD_TEXT_ALIGN_LEFT = 0,
	/// Align the text to the center of the LCD line
	LCD_TEXT_ALIGN_CENTER = 1,
	/// Align the text to the right side of the LCD line
	LCD_TEXT_ALIGN_RIGHT = 2
} text_align_e_t;

/// @}

#ifdef __cplusplus

/**
 * \ingroup c-llemu 
 */
namespace c {
#endif

/**
 * \ingroup c-llemu 
 */

/**
 * \addtogroup c-llemu
 *  @{
 */

/**
 * Checks whether the emulated three-button LCD has already been initialized.
 *
 * \return True if the LCD has been initialized or false if not.
 * 
 * \b Example
 * \code
 * if (pros::c::lcd_is_initialized()) {
 *   pros::c::lcd_print("LLEMU!");
 * }
 * else {
 *   printf("Error: LLEMU is not initialized\n");
 * }
 * \endcode
 */
bool lcd_is_initialized(void);

/**
 * Creates an emulation of the three-button, UART-based VEX LCD on the display.
 *
 * \return True if the LCD was successfully initialized, or false if it has
 * already been initialized.
 * 
 * \b Example
 * \code
 * #include "pros/llemu.h"
 * 
 * void initialize() {
 *   if (pros::c::lcd_initialize()) {
 *     pros::c::lcd_print("LLEMU!");	
 *   }
 *   else {
 * 	   printf("Error: LLEMU could not initailize\n");
 *   }
 * }
 * \endcode
 */
bool lcd_initialize(void);

/**
 * Turns off the Legacy LCD Emulator.
 *
 * Calling this function will clear the entire display, and you will not be able
 * to call any further LLEMU functions until another call to lcd_initialize.
 *
 * This function uses the following values of errno when an error state is
 * reached:
 * ENXIO - The LCD has not been initialized. Call lcd_initialize() first.
 *
 * \return True if the operation was successful, or false otherwise, setting
 * errno values as specified above.
 * 
 * \b Example
 * \code
 * #include "pros/llemu.h"
 * 
 * void disabled() {
 *   pros::c::lcd_shutdown();
 * }
 * \endcode
 */
bool lcd_shutdown(void);

/**
 * Displays a formatted string on the emulated three-button LCD screen.
 *
 * This function uses the following values of errno when an error state is
 * reached:
 * ENXIO  - The LCD has not been initialized. Call lcd_initialize() first.
 * EINVAL - The line number specified is not in the range [0-7]
 *
 * \param line
 *        The line on which to display the text [0-7]
 * \param fmt
 *        Format string
 * \param ...
 *        Optional list of arguments for the format string
 *
 * \return True if the operation was successful, or false otherwise, setting
 * errno values as specified above.
 * 
 * \b Example
 * \code
 * #include "pros/llemu.h"
 * 
 * void initialize() {
 *   pros::c::lcd_initialize();
 *   pros::c::lcd_print(0, "My formatted text: %d!", 2);
 * }
 * \endcode
 */
bool lcd_print(int16_t line, const char* fmt, ...);

/**
 * Displays a string on the emulated three-button LCD screen.
 *
 * This function uses the following values of errno when an error state is
 * reached:
 * ENXIO  - The LCD has not been initialized. Call lcd_initialize() first.
 * EINVAL - The line number specified is not in the range [0-7]
 *
 * \param line
 *        The line on which to display the text [0-7]
 * \param text
 *        The text to display
 *
 * \return True if the operation was successful, or false otherwise, setting
 * errno values as specified above.
 * 
 * \b Example
 * \code
 * #include "pros/llemu.h"
 * 
 * void initialize() {
 *   pros::c::lcd_initialize();
 *   pros::c::lcd_set_text(0, "My custom LLEMU text!");
 * }
 * \endcode
 */
bool lcd_set_text(int16_t line, const char* text);

/**
 * Clears the contents of the emulated three-button LCD screen.
 *
 * This function uses the following values of errno when an error state is
 * reached:
 * ENXIO  - The LCD has not been initialized. Call lcd_initialize() first.
 * EINVAL - The line number specified is not in the range [0-7]
 *
 * \return True if the operation was successful, or false otherwise, setting
 * errno values as specified above.
 * 
 * \b Example
 * \code
 * #include "pros/llemu.h"
 * 
 * void initialize() {
 *   pros::c::lcd_initialize();
 *   pros::c::lcd_clear(); // Clear the LCD screen
 * }
 * \endcode
 */
bool lcd_clear(void);

/**
 * Clears the contents of a line of the emulated three-button LCD screen.
 *
 * This function uses the following values of errno when an error state is
 * reached:
 * ENXIO  - The LCD has not been initialized. Call lcd_initialize() first.
 * EINVAL - The line number specified is not in the range [0-7]
 *
 * \param line
 *        The line to clear
 *
 * \return True if the operation was successful, or false otherwise, setting
 * errno values as specified above.
 * 
 * \b Example
 * \code
 * #include "pros/llemu.h"
 * 
 * void initialize() {
 *   pros::c::lcd_initialize();
 *   pros::c::lcd_clear_line(0); // Clear line 0
 * }
 * \endcode
 */
bool lcd_clear_line(int16_t line);

/**
 * Registers a callback function for the leftmost button.
 *
 * When the leftmost button on the emulated three-button LCD is pressed, the
 * user-provided callback function will be invoked.
 *
 * This function uses the following values of errno when an error state is
 * reached:
 * ENXIO  - The LCD has not been initialized. Call lcd_initialize() first.
 *
 * \param cb
 *        A callback function of type lcd_btn_cb_fn_t (void (*cb)(void))
 *
 * \return True if the operation was successful, or false otherwise, setting
 * errno values as specified above.
 * 
 * \b Example
 * \code
 * #include "pros/llemu.h"
 * 
 * void left_callback() {
 *   static int i = 0;
 *   
 *   i++;
 *   pros::c::lcd_print(0, "Left button pressed %i times", i);
 * }
 * 
 * void initialize() {
 *   pros::c::lcd_initialize();
 *   pros::c::lcd_register_btn0_cb();
 * }
 * \endcode
 */
bool lcd_register_btn0_cb(lcd_btn_cb_fn_t cb);

/**
 * Registers a callback function for the center button.
 *
 * When the center button on the emulated three-button LCD is pressed, the
 * user-provided callback function will be invoked.
 *
 * This function uses the following values of errno when an error state is
 * reached:
 * ENXIO  - The LCD has not been initialized. Call lcd_initialize() first.
 *
 * \param cb
 *        A callback function of type lcd_btn_cb_fn_t (void (*cb)(void))
 *
 * \return True if the operation was successful, or false otherwise, setting
 * errno values as specified above.
 * 
 * \b Example
 * \code
 * #include "pros/llemu.h"
 * 
 * void center_callback() {
 *   static int i = 0;
 *   
 *   i++;
 *   pros::c::lcd_print(0, "Center button pressed %i times", i);
 * }
 * 
 * void initialize() {
 *   pros::c::lcd_initialize();
 *   pros::c::lcd_register_btn1_cb();
 * }
 * \endcode
 */
bool lcd_register_btn1_cb(lcd_btn_cb_fn_t cb);

/**
 * Registers a callback function for the rightmost button.
 *
 * When the rightmost button on the emulated three-button LCD is pressed, the
 * user-provided callback function will be invoked.
 *
 * This function uses the following values of errno when an error state is
 * reached:
 * ENXIO  - The LCD has not been initialized. Call lcd_initialize() first.
 *
 * \param cb
 *        A callback function of type lcd_btn_cb_fn_t (void (*cb)(void))
 *
 * \return True if the operation was successful, or false otherwise, setting
 * errno values as specified above.
 * 
 * \b Example
 * \code
 * #include "pros/llemu.h"
 * 
 * void right_callback() {
 *   static int i = 0;
 *   
 *   i++;
 *   pros::c::lcd_print(0, "Right button pressed %i times", i);
 * }
 * 
 * void initialize() {
 *   pros::c::lcd_initialize();
 *   pros::c::lcd_register_btn2_cb();
 * }
 * \endcode
 */
bool lcd_register_btn2_cb(lcd_btn_cb_fn_t cb);

/**
 * Gets the button status from the emulated three-button LCD.
 *
 * The value returned is a 3-bit integer where 1 0 0 indicates the left button
 * is pressed, 0 1 0 indicates the center button is pressed, and 0 0 1
 * indicates the right button is pressed. 0 is returned if no buttons are
 * currently being pressed.
 *
 * Note that this function is provided for legacy API compatibility purposes,
 * with the caveat that the V5 touch screen does not actually support pressing
 * multiple points on the screen at the same time.
 *
 * \return The buttons pressed as a bit mask
 */
uint8_t lcd_read_buttons(void);

/**
 * Changes the alignment of text on the LCD background
 * 
 * \param alignment
 * 		An enum specifying the alignment. Available alignments are:
 * 			TEXT_ALIGN_LEFT
 * 			TEXT_ALIGN_RIGHT
 * 			TEXT_ALIGN_CENTER
 * 
 * \b Example
 * \code
 * #include "pros/llemu.h"
 *
 * void initialize() {
 *   pros::c::lcd_initialize();
 *   pros::c::lcd_set_alignment(pros::c::lcd_Text_Align::LEFT);
 *   pros::c::lcd_print(0, "Left Aligned Text");
 *   pros::c::lcd_set_alignment(pros::c::lcd_Text_Align::CENTER);
 *   pros::c::lcd_print(1, "Center Aligned Text");
 *   pros::c::lcd_set_alignment(pros::c::lcd_Text_Align::RIGHT);
 *   pros::c::lcd_print(2, "Right Aligned Text");
 * }
 * \endcode
 */
void lcd_set_text_align(text_align_e_t alignment);


///@}

#ifdef __cplusplus
}  // namespace c
}  // namespace pros
}  // extern "C"
#endif
#endif  // _LIBLVGL_LLEMU_H_