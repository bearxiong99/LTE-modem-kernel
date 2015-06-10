/*
 * Copyright Motorola, Inc. 2005-2006
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at 
 * your option) any later version.  
 *
 * This library is distributed in the hope that it will by useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTIBILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
 */
/*
 *
 * Description: WiMAX QoS Agent header file DNM (Do Not Modify part). If any modification 
 * is need, an agreement has to be reached with concerned LOWER layer party, e.g.
 * AMP MAC team, Motorola ICD, IEEE, WiMAX Forum ...
 *
 *
 */

#ifndef _MOT_AMP_WIMAX_QOS_AGENT_DNM_INC_
#define _MOT_AMP_WIMAX_QOS_AGENT_DNM_INC_


// SFM/BWM SPFT2
#define AMP_IPC_WIMAX_MAC_MSG_SFM_BASE (eAMP_IPC_WIMAX_MSG_QOS_BASE)
typedef enum
{
    eAMP_IPC_WIMAX_MAC_MSG_SF_INVALID = -1,
    eAMP_IPC_WIMAX_MAC_MSG_SF_ADD_REQUEST = eAMP_IPC_WIMAX_MSG_QOS_ALLOC_REQ - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_SF_CHANGE_REQUEST = eAMP_IPC_WIMAX_MSG_QOS_UPDATE_REQ - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_SF_DELETE_REQUEST = eAMP_IPC_WIMAX_MSG_QOS_DELETE_REQ - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_SF_ADD_RESP = eAMP_IPC_WIMAX_MSG_QOS_ALLOC_RSP - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_SF_CHANGE_RESP = eAMP_IPC_WIMAX_MSG_QOS_UPDATE_RSP - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_SF_DELETE_RESP = eAMP_IPC_WIMAX_MSG_QOS_DELETE_RSP - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_SF_ADD_NTF = eAMP_IPC_WIMAX_MSG_QOS_ALLOC_NTF - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_SF_CHANGE_NTF = eAMP_IPC_WIMAX_MSG_QOS_UPDATE_NTF - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_SF_DELETE_NTF = eAMP_IPC_WIMAX_MSG_QOS_DELETE_NTF - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_CS_SPEC_NTF = eAMP_IPC_WIMAX_MSG_CS_NTF - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_BWM_SF_INFO_REQ = eAMP_IPC_WIMAX_MSG_BWM_SF_INFO_REQ - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_BWM_SF_INFO_RSP = eAMP_IPC_WIMAX_MSG_BWM_SF_INFO_RSP - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_BWM_SF_SILENCE_PERIOD_REQ = eAMP_IPC_WIMAX_MSG_BWM_SF_SILENCE_PERIOD_REQ - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_BWM_SF_SILENCE_PERIOD_RSP = eAMP_IPC_WIMAX_MSG_BWM_SF_SILENCE_PERIOD_RSP - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_PSC_REQ = eAMP_IPC_WIMAX_MSG_PSC_REQ - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_PSC_RSP = eAMP_IPC_WIMAX_MSG_PSC_RSP - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_WIMAX_MAC_MSG_PSC_NTF = eAMP_IPC_WIMAX_MSG_PSC_NTF - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
#ifdef CONFIG_LTE
    eAMP_IPC_LTE_MAC_MSG_NAS_TFT = eAMP_IPC_LTE_MSG_NAS_TFT - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
    eAMP_IPC_LTE_MAC_MSG_NAS_TFT_MODIFY = eAMP_IPC_LTE_MSG_NAS_TFT_MODIFY - AMP_IPC_WIMAX_MAC_MSG_SFM_BASE,
#endif
    eAMP_IPC_WIMAX_MAC_MSG_SF_MAX_VAL
} AMP_WIMAX_MAC_SFM_MSG_E;

// service flow TLV, 802.16e 2005 cor-1
typedef enum
{
    eAMP_WIMAX_SF_TLV_TYPE_UNKNOWN = -1,
    eAMP_WIMAX_SF_TLV_TYPE_SFID = 1,
    eAMP_WIMAX_SF_TLV_TYPE_CID = 2,
    eAMP_WIMAX_SF_TLV_TYPE_SERVICE_CLASS_NAME = 3,
    eAMP_WIMAX_SF_TLV_TYPE_MBS_SERVICE = 4,
    eAMP_WIMAX_SF_TLV_TYPE_QOS_PARAMS_SET_TYPE = 5,
    eAMP_WIMAX_SF_TLV_TYPE_TRAFFIC_PRIORITY = 6,    // 0 to 7�Higher numbers indicate higher priority, Default 0
    eAMP_WIMAX_SF_TLV_TYPE_MAX_SUSTAINED_TRAFFIC_RATE = 7,
    eAMP_WIMAX_SF_TLV_TYPE_MAX_TRAFFIC_BURST = 8,
    eAMP_WIMAX_SF_TLV_TYPE_MIN_RESERVED_TRAFFIC_RATE = 9,
    eAMP_WIMAX_SF_TLV_TYPE_RESERVED10 = 10,
    eAMP_WIMAX_SF_TLV_TYPE_SF_SCHEDULING_TYPE = 11,
    eAMP_WIMAX_SF_TLV_TYPE_REQ_TRANSMISSION_POLICY = 12,
    eAMP_WIMAX_SF_TLV_TYPE_TOLERATED_JITTER = 13,
    eAMP_WIMAX_SF_TLV_TYPE_MAX_LATENCY = 14,
    eAMP_WIMAX_SF_TLV_TYPE_SDU_IND = 15,        //Fixed-length versus Variable-length SDU Indicator
    eAMP_WIMAX_SF_TLV_TYPE_SDU_SIZE = 16,
    eAMP_WIMAX_SF_TLV_TYPE_TARGET_SAID = 17,
    eAMP_WIMAX_SF_TLV_TYPE_ARQ_ENABLE = 18,
    eAMP_WIMAX_SF_TLV_TYPE_ARQ_WINDOW_SIZE = 19,
    eAMP_WIMAX_SF_TLV_TYPE_ARQ_RETRY_TIMEOUT_TX = 20,       // Transmitter Delay
    eAMP_WIMAX_SF_TLV_TYPE_ARQ_RETRY_TIMEOUT_RX = 21,       // Receiver Delay
    eAMP_WIMAX_SF_TLV_TYPE_ARQ_BLOCK_LIFETIME = 22,
    eAMP_WIMAX_SF_TLV_TYPE_ARQ_SYNC_LOSS = 23,
    eAMP_WIMAX_SF_TLV_TYPE_ARQ_DELIVER_IN_ORDER = 24,
    eAMP_WIMAX_SF_TLV_TYPE_ARQ_PURGE_TIMEOUT = 25,
    eAMP_WIMAX_SF_TLV_TYPE_ARQ_BLOCK_SIZE = 26,
    eAMP_WIMAX_SF_TLV_TYPE_RECEIVER_ARQ_ACK_PROCESSING_TIME = 27,
    eAMP_WIMAX_SF_TLV_TYPE_CS_SPECIFICATION = 28,
    eAMP_WIMAX_SF_TLV_TYPE_TYPE_OF_DATA_DELIVERY_SERVICES = 29,
    eAMP_WIMAX_SF_TLV_TYPE_SDU_INTER_ARRIVAL_INTERVAL = 30,
    eAMP_WIMAX_SF_TLV_TYPE_TIME_BASE = 31,
    eAMP_WIMAX_SF_TLV_TYPE_PAGING_PREFERENCE = 32,
    eAMP_WIMAX_SF_TLV_TYPE_MBS_ZONE_IDENTIFIER_ASSIGNMENT = 33,
#ifdef IEEE80216e_cor2  
    eAMP_WIMAX_SF_TLV_TYPE_RESERVED34 = 34,
#else
    eAMP_WIMAX_SF_TLV_TYPE_TRAFFIC_INDICATION_PREFERENCE = 34,
#endif
    eAMP_WIMAX_SF_TLV_TYPE_GLOBAL_SERVICE_CLASS_NAME = 35,
    eAMP_WIMAX_SF_TLV_TYPE_RESERVED36 = 36,
    eAMP_WIMAX_SF_TLV_TYPE_SN_FEEDBACK_ENABLED = 37,
    eAMP_WIMAX_SF_TLV_TYPE_FSN_SIZE = 38,
    eAMP_WIMAX_SF_TLV_TYPE_CID_ALLOCATION_FOR_ACTIVE_BSS = 39,
    eAMP_WIMAX_SF_TLV_TYPE_UNSOLICITED_GRANT_INTERVAL = 40,
    eAMP_WIMAX_SF_TLV_TYPE_UNSOLICITED_POLLING_INTERVAL = 41,
    eAMP_WIMAX_SF_TLV_TYPE_PDU_SN_EXT_HARQ = 42,        // PDU SN extended subheader for HARQ reordering
    eAMP_WIMAX_SF_TLV_TYPE_MBS_CONTENTS_ID = 43,
    eAMP_WIMAX_SF_TLV_TYPE_HARQ_SERVICE_FLOWS = 44,
    eAMP_WIMAX_SF_TLV_TYPE_AUTHORIZATION_TOKEN = 45,
    eAMP_WIMAX_SF_TLV_TYPE_HARQ_CHANNEL_MAPPING = 46,
    eAMP_WIMAX_SF_TLV_TYPE_ROHC_PARAM_PAYLOAD = 47,
    eAMP_WIMAX_SF_TLV_TYPE_MAX_VAL
} AMP_WIMAX_SF_TLV_TYPE_E;


// SFM SPFT2
typedef enum
{
    eAMP_WIMAX_MAC_SF_INFO_TYPE_UNKNOWN = -1,   
    eAMP_WIMAX_MAC_SF_VENDOR_SPECIFIC_QOS_PARAMS = 253,     // Type 143 of Service Flow Encodings
    eAMP_WIMAX_MAC_SF_CS_PARAMS_SET = 254,      // one or more sets of the standard CS Parameter Encodings for Classification and PHS rules.
    eAMP_WIMAX_MAC_SF_SF_PARAMS_SET = 255,      // Subset of standard Service Flow Encodings
    eAMP_WIMAX_MAC_SF_INFO_TYPE_MAX_VAL
} AMP_WIMAX_MAC_SF_INFO_TYPE_E;

// 802.16e 2005 cor-1
// Packet CS encodings for configuration and MAC messaging
typedef enum
{
    eAMP_WIMAX_MAC_CS_ENCODING_UNKNOWN = -1,   
    eAMP_WIMAX_MAC_CS_ENCODING_CLA_DSC = 1,     // ONE BYTE, subtype 1 (Classifier DSC action)
    eAMP_WIMAX_MAC_CS_ENCODING_CLA_ERROR_PARAMETER_SET = 2,
    eAMP_WIMAX_MAC_CS_ENCODING_CLA_RULE = 3,        // subtype 3 compound (Classification rule).
    eAMP_WIMAX_MAC_CS_ENCODING_PHS_DSC = 4,     // subtype 4 (PHS DSC action).
    eAMP_WIMAX_MAC_CS_ENCODING_PHS_ERROR_PARAMS = 5,
    eAMP_WIMAX_MAC_CS_ENCODING_PHS_RULE = 6,        // subtype 6 compound (PHS rule).
    eAMP_WIMAX_MAC_CS_ENCODING_MAX_VAL
} AMP_WIMAX_MAC_CS_ENCODING_E;


// 802.16e 2005 cor-1
typedef enum
{
    eAMP_WIMAX_MAC_PHS_RULE_TYPE_UNKNOWN = -1,   
    eAMP_WIMAX_MAC_PHS_RULE_TYPE_PHSI = 1,      
    eAMP_WIMAX_MAC_PHS_RULE_TYPE_PHSF = 2,      
    eAMP_WIMAX_MAC_PHS_RULE_TYPE_PHSM = 3, 
    eAMP_WIMAX_MAC_PHS_RULE_TYPE_PHSS = 4,
    eAMP_WIMAX_MAC_PHS_RULE_TYPE_PHSV = 5,
    eAMP_WIMAX_MAC_PHS_RULE_TYPE_VENDOR_SPECIFIC = 143
} AMP_WIMAX_MAC_PHS_RULE_TYPE_E;
#define eAMP_WIMAX_MAC_PHS_RULE_TYPE_MAX_VAL (4)


// 802.16e 2005 cor-1
// Length in the "definite form" defined in ITU-T Recommendation X.690
// TLV for length is less than 127
typedef struct
{
    UINT8 u8Type;
    UINT8 u8Length;         // MSB is 0
    UINT8 u8Value[1];
} AMP_WIMAX_TLV_SHORT_S;

// 802.16e 2005 cor-1
// TLV for length is more than 127
typedef struct
{
    UINT8 u8Type;
    UINT8 u8LengthLength;   // MSB is 1, additional length
    UINT8 u8Length[1];
    UINT8 u8Value[1];
} AMP_WIMAX_TLV_LONG_S;


// SFM SPFT2
// message payload from QA to MAC SFM
#define AMP_MAC_SF_ADD_REQ_FIXED_LENGTH (4)
typedef struct 
{
    UINT32 u32Direction;
    UINT8 u8HifTlv[1];
} AMP_WIMAX_MAC_MSG_SF_ADD_REQUEST_S;

// SFM SPFT2
#define AMP_MAC_SF_CHANGE_REQ_FIXED_LENGTH (4)
typedef struct 
{
    UINT32 u32Sfid;
    UINT8 u8HifTlv[1];
} AMP_WIMAX_MAC_MSG_SF_CHANGE_REQUEST_S;

// SFM SPFT2
typedef struct 
{
    UINT32 u32Sfid;
} AMP_WIMAX_MAC_MSG_SF_DELETE_REQUEST_S;

// SFM SPFT2
// Message payload from SFM to QA
#define AMP_MAC_SF_ADD_RESP_FIXED_LENGTH (8)
typedef struct 
{
    UINT32 u32Sfid;
    UINT8 u8Status;
    UINT8 u8RemoteCc;           // BS Confirmation code if status is 1
    UINT8 u8Pad[2];
    UINT8 u8HifTlv[1];
} AMP_WIMAX_MAC_MSG_SF_ADD_RESP_S;

// SFM SPFT2
#define AMP_MAC_SF_CHANGE_RESP_FIXED_LENGTH (8)
typedef struct 
{
    UINT32 u32Sfid;
    UINT8 u8Status;
    UINT8 u8RemoteCc;           // BS Confirmation code if status is 1
    UINT8 u8Pad[2];
    UINT8 u8HifTlv[1];
} AMP_WIMAX_MAC_MSG_SF_CHANGE_RESP_S;

// SFM SPFT2
typedef struct 
{
    UINT32 u32Sfid;
    UINT8 u8Status;
    UINT8 u8RemoteCc;           // BS Confirmation code if status is 1
    UINT8 u8Pad[2];
} AMP_WIMAX_MAC_MSG_SF_DELETE_RESP_S;

// SFM SPFT2
#define AMP_MAC_SF_ADD_NTF_FIXED_LENGTH (8)
typedef struct 
{
    UINT32 u32Sfid;
    UINT32 u32Direction;
    UINT8 u8HifTlv[1];
} AMP_WIMAX_MAC_MSG_SF_ADD_NOTIF_S;

// SFM SPFT2
#define AMP_MAC_SF_CHANGE_NTF_FIXED_LENGTH (4)
typedef struct 
{
    UINT32 u32Sfid;
    UINT8 u8HifTlv[1];
} AMP_WIMAX_MAC_MSG_SF_CHANGE_NOTIF_S;

// SFM SPFT2
typedef struct 
{
    UINT32 u32Sfid;
} AMP_WIMAX_MAC_MSG_SF_DELETE_NOTIF_S;

// SFM SPFT2
typedef struct 
{
    UINT8 u8CsList[1];
} AMP_WIMAX_MAC_MSG_CS_SPEC_NOTIF_S;


// BWM SPFT2
typedef AMP_WIMAX_MSG_BWM_SF_INFO_REQ_S AMP_WIMAX_MAC_MSG_BWM_SF_INFO_REQ_S;
typedef AMP_WIMAX_MSG_BWM_SF_SILENCE_PERIOD_REQ_S AMP_WIMAX_MAC_MSG_BWM_SF_SILENCE_PERIOD_REQ_S;
typedef AMP_WIMAX_MSG_BWM_SF_INFO_RSP_S AMP_WIMAX_MAC_MSG_BWM_SF_INFO_RSP_S;
typedef AMP_WIMAX_MSG_BWM_SF_SILENCE_PERIOD_RSP_S AMP_WIMAX_MAC_MSG_BWM_SF_SILENCE_PERIOD_RSP_S;

// PSC SPFT3
typedef AMP_WIMAX_MSG_PSC_REQ_S AMP_WIMAX_MAC_MSG_PSC_REQ_S;
typedef AMP_WIMAX_MSG_PSC_NTF_S AMP_WIMAX_MAC_MSG_PSC_NTF_S;
typedef AMP_WIMAX_MSG_PSC_RSP_S AMP_WIMAX_MAC_MSG_PSC_RSP_S;

#endif //_MOT_AMP_WIMAX_QOS_AGENT_DNM_INC_
