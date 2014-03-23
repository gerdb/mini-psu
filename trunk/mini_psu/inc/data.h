/*
 *  Project:      Mini_PSU
 *  File:         controller.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  header file for data.c
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
#ifndef DATA_H_
#define DATA_H_
#include "stm32f4xx.h"

extern int voltage_setpSM;
extern int voltage_setp;
extern int current_setp;

extern int setupController;
extern int outputOn;

extern int pid_KP;
extern int pid_KI;
extern int pid_KD;

extern int voltage_VOUT;
extern int voltage_VSM;
extern int voltage_VIN;
extern int current_IOUT;

extern int cursor;
#define CURSOR_KP 0
#define CURSOR_KI 1
#define CURSOR_KD 2

#endif /* DATA_H_ */
