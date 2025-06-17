/***************************************************************************
 Copyright (C) Realtek Semiconductor Corp.
 This module is a confidential and proprietary property of Realtek and
 a possession or use of this module requires written permission of Realtek.
 ***************************************************************************/

#ifndef _HCI_PHY_VENDOR_DEF_H_
#define _HCI_PHY_VENDOR_DEF_H_

#include "bt_fw_hci_external_defines.h"

/* PHY vendor operation */
#define PHY_VENDOR_GET                                       0
#define PHY_VENDOR_SET                                       1
/* PHY vendor sub-opcode */
#define PHY_GET_CUR_RF_MODE                                  0
#define PHY_SWITCH_RF_MODE                                   1
#define PHY_SET_TXGAIN_INDEX                                 2
#define PHY_DUMP_THERMAL_DATA                                5
#define PHY_DUMP_RAW_TXGAIN_INDEX                            6
#define PHY_SET_THERMAL_DEFAULT_IN_RAM                       12
#define PHY_TOGGLE_0P25_COMPENSATION                         13
#define PHY_SET_LBT_PARAMETER                                17


#endif /*_HCI_PHY_VENDOR_DEF_H_*/
