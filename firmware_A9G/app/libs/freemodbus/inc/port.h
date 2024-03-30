/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: port.h ,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#ifndef _PORT_H
#define _PORT_H

#include "mbconfig.h"
#include "api_os.h"
#include "api_inc_os.h"
#include "stdint.h"

#define PR_BEGIN_EXTERN_C           extern "C" {
#define PR_END_EXTERN_C             }

#define ENTER_CRITICAL_SECTION()    EnterCriticalSection()
#define EXIT_CRITICAL_SECTION()     ExitCriticalSection()

#ifndef true
#define true            1
#endif

#ifndef false
#define false           0
#endif

void EnterCriticalSection(void);
void ExitCriticalSection(void);

#endif
