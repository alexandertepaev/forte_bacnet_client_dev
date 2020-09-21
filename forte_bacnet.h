/*******************************************************************************
 * Copyright (c) 2020 Alexander Tepaev github.com/alexandertepaev
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Alexander Tepaev
 *******************************************************************************/
#ifndef BACNET_H
#define BACNET_H

#define IP_ADDR_LEN 4
#define PORT_LEN 2

#define BVLC_HEADER_LEN 4
#define BVLC_TYPE_BYTE 0
#define BVLC_FUNCTION_BYTE 1
#define BVLC_LEN_BYTE 2

#define NPDU_OFFSET 4

#define APDU_TYPE_MASK 0xF0
#define UNCONFIRMED_REQ_APP_TAGS_OFFSET 2
#define SIMPLE_ACK_INVOKE_ID_OFFSET 1
#define UNCONFIRMED_REQ_SERVICE_CHOICE_OFFSET 1
#define SIMPLE_ACK_SERVICE_CHOICE_OFFSET 2
// #define COMPLEX_ACK_NPDU_SEGMENTED_MASK (0x00 || 0b00001000(1<<3))
#define COMPLEX_ACK_NPDU_SEGMENTED_MASK (0x00 || 0b00001000)
#define COMPLEX_ACK_SERVICE_CHOICE_OFFSET 2
#define COMPLEX_ACK_APP_TAGS_OFFSET 3
#define SIMPLE_COMPLEX_ACK_INVOKE_ID_OFFSET 1
#define COV_INFINITE_LIFETIME 0

#define BACNET_WRITEPROP_LOWEST_PRIO 16

/* core files */
#include "version.h"
//#include "config.h"
#include "address.h"
#include "apdu.h"
#include "bacapp.h"
// #include "bacdcode.h"
#include "bacint.h"
// #include "bacreal.h"
// #include "bacstr.h"
#include "bacdef.h"
#include "bacenum.h"
// #include "bacerror.h"
// #include "bactext.h"
// #include "datalink.h"
// #include "indtext.h"
#include "npdu.h"
// #include "reject.h"
// #include "tsm.h"

#include "rp.h"
#include "wp.h"
#include "bip.h"
#include "bvlc.h"
#include "whois.h"
#include "iam.h"
#include "cov.h"

#endif