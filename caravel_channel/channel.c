/*
 * SPDX-FileCopyrightText: 2020 Efabless Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * SPDX-License-Identifier: Apache-2.0
 */

// This include is relative to $CARAVEL_PATH (see Makefile)
#include <defs.h>
#include <stub.c>

// change to your project's ID - ask Matt
#define PROJECT_ID 0

// Write a small register map based on wrapper.v pin mapping
#define DATA_VALUE 0x10
#define LO_NCO_PHASE 0x0c
#define CA_NCO_PHASE 0x08
#define PROMPT_Q 0x07
#define DATA_ADDRESS 0x04
#define CA_GEN_ENABLE 0x03
#define CA_NCO_ENABLE 0x02
#define LO_NCO_ENABLE 0x01
#define RESET_ADDR 0x00

#define DATA_MASK 0xFFFF0000
#define LO_PHASE_MASK 0x0F00
#define CA_PHASE_MASK 0X0F00
#define ADDRESS_MASK 0x0070

void main()
{
	/*
	IO Control Registers
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 3-bits | 1-bit | 1-bit | 1-bit  | 1-bit  | 1-bit | 1-bit   | 1-bit   | 1-bit | 1-bit | 1-bit   |

	Output: 0000_0110_0000_1110  (0x1808) = GPIO_MODE_USER_STD_OUTPUT
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 110    | 0     | 0     | 0      | 0      | 0     | 0       | 1       | 0     | 0     | 0       |


	Input: 0000_0001_0000_1111 (0x0402) = GPIO_MODE_USER_STD_INPUT_NOPULL
	| DM     | VTRIP | SLOW  | AN_POL | AN_SEL | AN_EN | MOD_SEL | INP_DIS | HOLDH | OEB_N | MGMT_EN |
	| 001    | 0     | 0     | 0      | 0      | 0     | 0       | 0       | 0     | 1     | 0       |

	*/

	/* Set up the housekeeping SPI to be connected internally so	*/
	/* that external pin changes don't affect it.			*/

	// GPS RF sample input
	reg_mprj_io_23 = GPIO_MODE_USER_STD_INPUT_NOPULL;

	// Prompt and Local Oscillator (LO) In-Phase Outputs
	reg_mprj_io_21 = GPIO_MODE_USER_STD_OUTPUT;
	reg_mprj_io_19 = GPIO_MODE_USER_STD_OUTPUT;

	/* Apply configuration */
	reg_mprj_xfer = 1;
	while (reg_mprj_xfer == 1);

	// activate the project by setting the 0th bit of 1st bank of LA
	reg_la0_iena = 0;				   // input enable off
	reg_la0_oenb = 0xFFFFFFFF;		   // enable all of bank0 logic analyser outputs (ignore the name, 1 is on, 0 off)
	reg_la0_data |= (1 << PROJECT_ID); // enable the project

	// reset design with 0bit of 2nd bank of LA
	reg_la1_oenb = 1; // enable
	reg_la1_iena = 0;
	reg_la1_data = 1;
	reg_la1_data = 0;

	reg_la1_oenb = 0x7F; // enable outputs for NCO and CA_GEN enables and register address

	// Set reset to high and enable both NCOs
	reg_la1_data |= (1 << RESET_ADDR);
	reg_la1_data |= (1 << CA_NCO_ENABLE);
	reg_la1_data |= (1 << LO_NCO_ENABLE);

	// Clear the reset bit
	reg_la1_data &= ~(1 << RESET_ADDR);

	// Enable the C/A code generator
	reg_la1_data |= (1 << CA_GEN_ENABLE);

	// do nothing
}
