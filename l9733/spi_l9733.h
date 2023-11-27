#include <stdint.h>
/* include spi.h
 * spi_t type represents SPI controller
 * assume there are read/write operations available
 */

/* L9733 driver configuration overview
 * 2 configuration methods: discrete or SPI...
 * ==== DISCRETE INPUTS ====
 * The pins In6, In7, and In8 allows outputs 6, 7, and 8 to be enabled,
 * without SPI inputs.
 * Suitable for non-inductive loads that are pulse width modulated control.
 * NOTE: that these pins are logically OR'ed with SPI outputs.
 *
 * ==== SPI ====
 * The L9733 has a serial peripheral interface (SPI) consisting of
 *
 * Serial clock (SCLK):
 *
 * Serial data out (DO): When CS is logic 0
 * - transmit 16 bits (fault register) to digital controller
 * - transmit data received on DI pin 16 SCLK cycles earlier
 *   until CS transitions to logic 1
 *
 * Serial data in (DI): Takes data when CS is logic 0
 * - receive 16 bits:
 *   [keyword] [mode]   [OUTn]
 *   [b15-b12] [b11-b8] [b7-b0]
 * - transmit 16 bits; what was received on DI is output back on DO
 *   after DO finishes transmitting fault register (16 bits).
 *   LSB is first bit of each byte (and MSB is last bit of each byte)
 *
 * Chip select (CS):
 */

/* spi_t The MC's SPI controller
 * || @dev: The device for communicating with controller
 */
static struct spi_t {
} l9733_spi_driver;

/* l9733_output_fault the fault register consists of an array of 2 bits
 * used to indicate the faults of each outputs.
 */
enum l9733_output_fault {
        L9733_FAULT_NO_FAULT = 0,
        L9733_FAULT_OPEN_LOAD,
        L9733_FAULT_SHORT_CIRCUIT,
        L9733_FAULT_OVERCURRENT
};

/* l9733_command_mode for choosing the action to perform via SPI.
 * - L9733_COMMAND_OUTPUT: turn output on or off
 * - L9733_COMMAND_DIAGNOSTICS: turn latch mode on or off
 * - L9733_COMMAND_PROTECTION: turn overcurrent protection on or off
 */
enum l9733_command_mode {
        L9733_COMMAND_OUTPUT,
        L9733_COMMAND_DIAGNOSTICS,
        L9733_COMMAND_PROTECTION
};

/* for implementation: send this down SPI wire for selected l9733_command_mode */
#define L9733_COMMAND_MODE_OUTPUT (0b1100)
#define L9733_COMMAND_MODE_DIAGNOSTICS (0b0011)
#define L9733_COMMAND_MODE_PROTECTION (0b1010)

/* Errors */
#define ERR_L9733_FAIL_SPI_READ (-10)

/* Total number of outputs available in L9733 */
#define L9733_NUMBER_OF_OUTPUTS (8)

/* States for each writing mode */
#define L9733_COMMAND_SET_OUTPUT_OFF     (0)
#define L9733_COMMAND_SET_OUTPUT_ON      (1)
#define L9733_COMMAND_SET_NO_LATCH_MODE  (0)
#define L9733_COMMAND_SET_LATCH_MODE     (1)
#define L9733_COMMAND_SET_PROTECTION_OFF (0)
#define L9733_COMMAND_SET_PROTECTION_ON  (1)

/* Set a new state for an output N
 * || shifts the state for output N to the correct flag position in the mask
 */
#define L9733_OUTPUT_STATE_n(n, s) (s << n)

/* Decode a fault state for a particular output from the fault register.
 * || extracts the fault for output N
 */
#define L9733_FAULT_STATUS_n(n, faults) \
        ((enum l9733_output_fault)((faults >> (2*n)) & 0x3))

/* Set to whichever GPIO Pins are connected from L9733 to this MC */
#define GPIO_PIN_1 (assert("undefined"))
#define GPIO_PIN_2 (assert("undefined"))
#define GPIO_PIN_3 (assert("undefined"))
#define GPIO_PIN_4 (assert("undefined"))
#define L9733_OUTPUT_6_PIN (GPIO_PIN_1)
#define L9733_OUTPUT_7_PIN (GPIO_PIN_2)
#define L9733_OUTPUT_8_PIN (GPIO_PIN_3)
#define L9733_OUTPUT_RES_PIN (GPIO_PIN_4)

/* l9733_input_command
 * @keyword: The input command must always start with the key-word
 *     NOTE: If it's programmed to always be 0b1010, then this probably
 *     doesn't need to be configurable...
 * @mode: the command action to perform for each output
 *     \see l9733_command_mode for more info.
 * @output_mode_mask: set new states for each outputs (output 8 is MSB:bit 7)
 *     For these writing mode, the following state (in each bit flag position) means:
 *      * output: command each output OFF (0)/ON (1)
 *      * diagnostics: set No Latch Mode (0)/Latch Mode (1) for each output
 *      * protection: switch OFF (0) or set linear overcurrent protection (1)
 *     \see L9733_OUTPUT_STATE_n can use this to set output_mode_mask, for example
 *     (L9733_OUTPUT_STATE_n(0, L9733_COMMAND_SET_PROTECTION_ON)
 *     | L9733_OUTPUT_STATE_n(1, L9733_COMMAND_SET_PROTECTION_OFF)
 *     ...)
 */
struct l9733_input_command {
        uint8_t keyword;
        enum l9733_command_mode mode;
        uint8_t output_mode_mask;
};

/* l9733_initialize_spi perform any initialisation required for the SPI controller.
 */
int l9733_initialize_spi(struct spi_t *dev);

/* l9733_write_spi use the SPI channel to send commands to the l9733.
 * @dev the MC's SPI controller used to communicate with l9733.
 * @cmd The command to send over the SPI wire. \see l9733_input_command
 * @return error status
 */
int l9733_write_spi(struct spi_t *dev, struct l9733_input_command cmd);

/* l9733_write_pin use discrete inputs on the l9733 to update an output
 * @output_n select one of these outputs: 
 *     * Output 6 (L9733_OUTPUT_6_PIN)
 *     * Output 7 (L9733_OUTPUT_7_PIN)
 *     * Output 8 (L9733_OUTPUT_8_PIN)
 *  @commant_set command an output OFF (L9733_COMMAND_SET_OUTPUT_OFF) or
 *     ON (L9733_COMMAND_SET_OUTPUT_ON)
 *  @return error status
 */
int l9733_write_pin(uint8_t output_n, uint8_t command_set);

/* l9733_reset use the reset input pin on the l9733 to reset all internal
 *     registers and switch off all output stages.
 * @return error status
 */
int l9733_reset();

/* l9733_read_fault_register read the status of each outputs,
 *     \see l9733_output_fault for the states returned by l9733
 * @return ERR_L9733_FAIL_SPI_READ if error occured,
 *     otherwise a 16 bit number is returned representing the fault register,
 *     \see L9733_FAULT_STATUS_n for decoding fault for a particular output
 */
int16_t l9733_read_fault_register(struct spi_t *dev);

/* alternative definition if memory constraints aren't a problem:
int l9733_read_fault_register(struct spi_t *dev,
                              enum l9733_output_fault states[L9733_NUMBER_OF_OUTPUTS]);
*/
