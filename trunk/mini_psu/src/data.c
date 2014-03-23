/*
 *  Project:      Mini_PSU
 *  File:         controller.c
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  global data
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "data.h"

// Setpoints
int voltage_setpSM = 0;
int voltage_setp = 120;
int current_setp = 200;

// Mode
int setupController = 0;

// Output on/off
int outputOn = 0;

// Controller settings
int pid_KP = 2; //10
int pid_KI = 3; //5
int pid_KD = 30; //20

// Voltage and currents
int voltage_VOUT = 0;
int voltage_VSM = 0;
int voltage_VIN = 0;
int current_IOUT = 0;

//Cursor
int cursor = 0;
