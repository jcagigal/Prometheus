/* W.I.P. */
#include <proto/exec.h>
#include <proto/prometheus.h>
#include <libraries/prometheus.h>
#include "boardinfo.h"
#include <proto/picasso96_chip.h>

#include "card.h"

struct CardInfo
  {
    ULONG Device;
    UBYTE *Memory0;
    UBYTE *Memory1;
    UBYTE *Memory2;
    ULONG Memory0Size;
    ULONG Memory1Size;
    ULONG Memory2Size;
    UBYTE *ROM;
	ULONG ROMSize;
  };

static ULONG count = 0;
#define PCI_VENDOR  0x1002
#define CHIP_NAME   "picasso96/ATIRadeon.chip"

BOOL InitATIRadeon(struct CardBase *cb, struct BoardInfo *bi)
  {
    struct Library* PrometheusBase = cb->cb_PrometheusBase;
    struct Library* SysBase = cb->cb_SysBase;

    APTR board = NULL;
    ULONG current = 0;

    #ifdef DBG
      KPrintf("prometheus.card: InitATIRadeon()\n");
    #endif

    while ((board = (APTR)Prm_FindBoardTags(board, PRM_Vendor, PCI_VENDOR, TAG_END)) != NULL)
      {
        struct CardInfo ci;
        BOOL found = FALSE;

        #ifdef DBG
          KPrintf("  ATI Radeon board found on PCI [$%08lx]\n", (ULONG)board);
        #endif

        Prm_GetBoardAttrsTags(board,
          PRM_Device,         (ULONG)&ci.Device,
          PRM_MemoryAddr0,    (ULONG)&ci.Memory0,
          PRM_MemoryAddr1,    (ULONG)&ci.Memory1,
          PRM_MemoryAddr2,    (ULONG)&ci.Memory2,
          PRM_MemorySize0,    (ULONG)&ci.Memory0Size,
          PRM_MemorySize1,    (ULONG)&ci.Memory1Size,
          PRM_MemorySize2,    (ULONG)&ci.Memory2Size,
          PRM_ROM_Address,    (ULONG)&ci.ROM,
		  PRM_ROM_Size,       (ULONG)&ci.ROMSize,
          TAG_END);

/*       ULONG baseaddr;

       for (i = 0; i < 6; i++)
        {
         Prm_GetBoardAttrsTags (any_board, PRM_MemoryAddr0 + i,
          (ULONG)&baseaddr, TAG_END);
        }*/

        switch(ci.Device)
          {
			case 0x5144:  // RADEON
			case 0x5145:  // RADEON
			case 0x5146:  // RADEON
			case 0x5147:  // RADEON
			case 0x4C57:  // RADEON
			case 0x4C58:  // RADEON
			case 0x4C59:  // RV100
			case 0x4C5A:  // RV100
			case 0x5159:  // RV100
			case 0x515A:  // RV100
			case 0x4136:  // RS100
			case 0x4336:  // RS100
			case 0x4137:  // RS200
			case 0x4337:  // RS200
			case 0x4237:  // RS200
			case 0x4437:  // RS250
			case 0x5148:  // R200
			case 0x514C:  // R200
			case 0x514D:  // R200
			case 0x4242:  // R200
			case 0x4243:  // R200
			case 0x5157:  // RV200 (Radeon7500)
			case 0x5158:  // RV200
			case 0x4966:  // RV250
			case 0x4967:  // RV250
			case 0x4C64:  // RV250
			case 0x4C66:  // RV250
			case 0x4C67:  // RV250
			case 0x5834:  // RS300
			case 0x5835:  // RS300
			case 0x7834:  // RS350
			case 0x7835:  // RS350
			case 0x5960:  // RV280
			case 0x5961:  // RV280
			case 0x5962:  // RV280
			case 0x5964:  // RV280
			case 0x5C61:  // RV280 (Radeon9250)
			case 0x5C63:  // RV280
			case 0x4144:  // R300
			case 0x4145:  // R300
			case 0x4146:  // R300
			case 0x4147:  // R300
			case 0x4E44:  // R300
			case 0x4E45:  // R300
			case 0x4E46:  // R300
			case 0x4E47:  // R300
			case 0x4150:  // RV350
			case 0x4151:  // RV350
			case 0x4152:  // RV360
			case 0x4153:  // RV350
			case 0x4154:  // RV350
			case 0x4156:  // RV350
			case 0x4E50:  // RV350
			case 0x4E51:  // RV350
			case 0x4E52:  // RV350
			case 0x4E53:  // RV350
			case 0x4E54:  // RV350
			case 0x4E56:  // RV350
			case 0x4148:  // R350
			case 0x4149:  // R350
			case 0x414A:  // R350
			case 0x414B:  // R350
			case 0x4E48:  // R350
			case 0x4E49:  // R350
			case 0x4E4A:  // R350
			case 0x4E4B:  // R360
			case 0x3E50:  // RV380
			case 0x3E54:  // RV380
			case 0x3150:  // RV380
			case 0x3154:  // RV380
			case 0x5B60:  // RV370
			case 0x5B62:  // RV370
			case 0x5B63:  // RV370
			case 0x5B64:  // RV370
			case 0x5B65:  // RV370
			case 0x5460:  // RV370
			case 0x5464:  // RV370
			case 0x4A48:  // R420
			case 0x4A49:  // R420
			case 0x4A4A:  // R420
			case 0x4A4B:  // R420
			case 0x4A4C:  // R420
			case 0x4A4D:  // R420
			case 0x4A4E:  // R420
			case 0x4A50:  // R420
			case 0x5548:  // R423
			case 0x5549:  // R423
			case 0x554A:  // R423
			case 0x554B:  // R423
			case 0x5551:  // R423
			case 0x5552:  // R423
			case 0x5554:  // R423
			case 0x5D57:  // R423
			case 0x791F:  // R420
			case 0x94CC:  // RV610LE (Radeon HD2400)
              found = TRUE;
              break;

            default:
              found = FALSE;
          }

        if(found)
          {
            struct ChipBase *ChipBase;

            #ifdef DBG
              KPrintf("  card attrs read (device %ld)\n", (ULONG)ci.Device);
            #endif

            // check for multiple hits and skip the ones already used
            if(current++ < count) continue;

            // we have found one - so mark it as used and
            // don't care about possible errors from here on
            count++;

            if ((ChipBase = (struct ChipBase*)OpenLibrary(CHIP_NAME, 0)) != NULL)
              {
                #ifdef DBG
                  KPrintf("  chip driver opened [$%08lx]\n", (ULONG)ChipBase);
                #endif

                bi->ChipBase = ChipBase;

//                bi->DeviceID     = ci.Device;
                bi->MemoryBase   = ci.Memory0; 		// Framebuffer
                bi->MemoryIOBase = ci.Memory1; 
                bi->RegisterBase = ci.Memory2;			// MMIO
                bi->MemorySize   = ci.Memory0Size; 	// FB Size
//                bi->BIOSBase     = (ULONG)ci.ROM;
//				bi->BIOSSize     = (ULONG)ci.ROMSize;
//				bi->IOSize       = (ULONG)ci.Memory1Size;
//				bi->RegisterSize = (ULONG)ci.Memory2Size;

                // register interrupt server
                RegisterIntServer(cb, board, &bi->HardInterrupt);

                InitChip(bi);

                // enable vertical blanking interrupt
                bi->Flags |= BIF_VBLANKINTERRUPT;

                Prm_SetBoardAttrsTags(board,
                  PRM_BoardOwner, (ULONG)ChipBase,
                TAG_END);

                return TRUE;
              }
          }
      } // while
    return FALSE;
  }
