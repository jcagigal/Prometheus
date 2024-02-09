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
    ULONG Memory0Size;
    UBYTE *ROM;
	ULONG ROMSize;
  };

static ULONG count = 0;
#define PCI_VENDOR     0x1013
#define PCI_DEVICE_ID  0x00B8
#define CHIP_NAME   "picasso96/CirrusGD5446.chip"

/*#define ROMBase       ChipData[14]
#define DeviceID      ChipData[15]*/

BOOL InitCirrusGD5446(struct CardBase *cb, struct BoardInfo *bi)
  {
    struct Library* PrometheusBase = cb->cb_PrometheusBase;
    struct Library* SysBase = cb->cb_SysBase;

    APTR board = NULL;
    ULONG current = 0;

    #ifdef DBG
      KPrintf("prometheus.card: InitCirrusGD5446()\n");
    #endif

    while ((board = (APTR)Prm_FindBoardTags(board, PRM_Vendor, PCI_VENDOR, TAG_END)) != NULL)
      {
        struct CardInfo ci;
        BOOL found = FALSE;

        #ifdef DBG
          KPrintf("  Cirrus GD 5446 found on PCI [$%08lx]\n", (ULONG)board);
        #endif

        Prm_GetBoardAttrsTags(board,
          PRM_Device,         (ULONG)&ci.Device,
          PRM_MemoryAddr0,    (ULONG)&ci.Memory0,
          PRM_MemorySize0,    (ULONG)&ci.Memory0Size,
          PRM_ROM_Address,    (ULONG)&ci.ROM,
		  PRM_ROM_Size,       (ULONG)&ci.ROMSize,
          TAG_END);

        switch(ci.Device)
          {
			case PCI_DEVICE_ID: // Cirrus GD5446
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

//                bi->DeviceID      = ci.Device;
                bi->MemoryBase    = ci.Memory0;
/*                bi->MemoryIOBase  = ci.Memory1;
                bi->RegisterBase  = ci.Memory2;*/
                bi->MemorySize    = ci.Memory0Size;
//                bi->ROMBase       = (ULONG)ci.ROM;
//				bi->ROMSize       = (ULONG)ci.ROMSize;

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
