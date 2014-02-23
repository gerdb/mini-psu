/*
 *  Project:      Mini_PSU
 *  File:         typedef.h
 *  Author:       Gerd Bartelt - www.sebulli.com
 *
 *  Description:  types and enums
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
#ifndef TYPEDEF_H_
#define TYPEDEF_H_

typedef enum {
	OFF,
	IDLE,
	GYRO,
	SUN,
	SUNSIMULATION,
} e_sysstate;

typedef enum {
	OK,
	LOWBAT = 2,
	ACTOR
	} e_errstate;



#endif /* TYPEDEF_H_ */
