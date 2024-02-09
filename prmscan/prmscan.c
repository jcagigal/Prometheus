/* Source code of prmscan - program scanning system for PCI cards plugged   */
/* into Prometheus board. It demonstrates use of prometheus.library.        */

#define __NOLIBBASE__  /* we do not want to peeking library bases */

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/prometheus.h>

#if defined(__VBCC__)
#include <string.h>
#else
#include <faststring.h>
#endif

#if defined(__VBCC__)
extern struct Library *SysBase, *DOSBase;
#else
struct Library *SysBase, *DOSBase;
#endif
struct Library *PrometheusBase;

#define STR_VENDORID_MAX 128

const char VString[] = "$VER: prmscan 1.8 (14.09.2023) by Grzegorz Kraszewski, Tobias Seiler, Dennis van der Boon, Jaime Cagigal\n";


UWORD HexStrToWord(UBYTE *buffer)
 {
  WORD i;
  UWORD result = 0, ch;

  for (i = 0; i < 4; i++)
   {
    result <<= 4;
    ch = buffer[i];
    if (ch >= '0' && ch <= '9') ch -= 48;
    else if (ch >= 'A' && ch <= 'F') ch -= 55;
    else if (ch >= 'a' && ch <= 'f') ch -= 87;
    else return 0xFFFF;
    result += ch;
   }
  return result;
 }


void ShiftBufferLeft(UBYTE *buffer, WORD shift)
 {
  UBYTE *ptr = buffer;

  while (ptr[shift]) {ptr[0] = ptr[shift]; ptr++;}
 }


void StripLeadingSpaces(UBYTE *buffer)
 {
  WORD idx = 0;

  while (buffer[idx] == 0x09 || buffer[idx] == 0x20) idx++;
  if (idx) ShiftBufferLeft(buffer, idx);
 }


void RemoveEOL(UBYTE *buffer)
 {
  UBYTE *ptr = buffer;
  while (*buffer++ != 0x0A);
  *--buffer = 0x00;
 }


LONG GetVendorString(BPTR vendors_file, UWORD vendor, UBYTE *buffer, LONG bufsize)
 {
  LONG error;

  if (vendors_file)
   {
    Seek(vendors_file, 0, OFFSET_BEGINNING);
    while (FGets(vendors_file, buffer, bufsize))
     {
      if ((*buffer & 0xFF) == ';') continue;              /* comment */
      if ((*buffer & 0xFF) == 0x09) continue;             /* device description */
      if ((*buffer & 0xFF) == 0x20) continue;
	  if ((buffer[0] == 'f' && buffer[1] == 'f' && buffer[2] == 'f' && buffer[3] == 'f') 
		  || (buffer[0] == 'F' && buffer[1] == 'F' && buffer[2] == 'F' && buffer[3] == 'F'))
		{
			sprintf (buffer, "unknown ($%04lx)", vendor);
			return 0;
		}
      if (HexStrToWord(buffer) == vendor)
       {
        ShiftBufferLeft(buffer, 5);
        StripLeadingSpaces(buffer);
        RemoveEOL(buffer);
        return 0;
       }
     }
    if (error = IoErr()) return error;
   }
  else sprintf (buffer, "unknown ($%04lx)", vendor);
  return 0;
 }


LONG GetDeviceString(BPTR vendors_file, UWORD device, UBYTE *buffer, LONG bufsize)
 {
  LONG error;

  if (device != 0xFFFF)
   {
    if (vendors_file)
     {
      while (FGets(vendors_file, buffer, bufsize))
       {
        if (*buffer == ';') continue;                       /* comment */
        if (*buffer != 0x09 && *buffer != 0x20) break;      /* vendor -> dev not found */
        StripLeadingSpaces(buffer);
        if (HexStrToWord(buffer) == device)
         {
          ShiftBufferLeft(buffer, 5);
          StripLeadingSpaces(buffer);
          RemoveEOL(buffer);
          return 0;
         }
       }
      if (error = IoErr()) return error;
     }
   }
  sprintf (buffer, "unknown ($%04lx)", device);
  return 0;
 }


int main(void)
 {
  UBYTE vendor_name[STR_VENDORID_MAX];
  UBYTE device_name[STR_VENDORID_MAX];

  Printf ("\nPrmScan 1.8 (14.09.2023) by Grzegorz Kraszewski, Dennis van der Boon & Jaime Cagigal\n");
  if (PrometheusBase = OpenLibrary ("prometheus.library", 2))
   {
    APTR board = NULL;
    ULONG vendor, device, revision, dclass, dsubclass, blkaddr, blksize, bus, slot, function;
    ULONG dheadertype, dinterface;
    ULONG dsubsysvendor,dsubsysid;
    ULONG romaddr, romsize;
	UBYTE pc_LatencyTimer;
	UBYTE pc_IntLine;
	UBYTE pc_IntPin;
    WORD blk;
    BPTR vendors_file;
    struct Node *owner;
    STRPTR owner_name = "NONE";
	
	Printf ("\nprometheus.library %ld.%ld\n\n", PrometheusBase->lib_Version, PrometheusBase->lib_Revision);
	Printf ("PCI cards listing:\n-------------------------------------------------\n");

    vendors_file = Open("DEVS:PCI/vendors.txt", MODE_OLDFILE);

    while (board = Prm_FindBoardTags(board, TAG_END))
     {
      Prm_GetBoardAttrsTags(board,
        PRM_Vendor, (ULONG)&vendor,
        PRM_Device, (ULONG)&device,
        PRM_Revision, (ULONG)&revision,
        PRM_Class, (ULONG)&dclass,
        PRM_SubClass, (ULONG)&dsubclass,
        PRM_BusNumber, (ULONG)&bus,
        PRM_SlotNumber, (ULONG)&slot,
        PRM_FunctionNumber, (ULONG)&function,
        PRM_BoardOwner, (ULONG)&owner,
        PRM_HeaderType, (ULONG)&dheadertype,
        PRM_SubsysVendor, (ULONG)&dsubsysvendor,
        PRM_SubsysID, (ULONG)&dsubsysid,
        PRM_Interface, (ULONG)&dinterface,
       TAG_END);
      GetVendorString(vendors_file, vendor, vendor_name, STR_VENDORID_MAX);
      GetDeviceString(vendors_file, device, device_name, STR_VENDORID_MAX);
      Printf("Board on bus %ld in slot %ld, function %ld\n", bus, slot, function);
      Printf("Vendor: ($%04lx) \033[1m%s\033[0m\nDevice: ($%04lx) \033[1m%s\033[0m\nRevision: %ld.\n",
       vendor, (LONG)vendor_name, device, (LONG)device_name, revision);
      Printf("Device class %02lx, subclass %02lx, progIF %02lx, intLine %03lx, intPin %03lx, LatencyTimer %03lx.\n", dclass, dsubclass, dinterface, pc_IntLine, pc_IntPin, pc_LatencyTimer);
      for (blk = 0; blk < 6; blk++)
       {
        Prm_GetBoardAttrsTags (board,
         PRM_MemoryAddr0 + blk, (ULONG)&blkaddr,
         PRM_MemorySize0 + blk, (ULONG)&blksize,
         TAG_END);
        if (blkaddr && blksize)
         {
          ULONG normsize = blksize;
          STRPTR unit = "B";

          if (normsize >= 1024)
            {
              normsize >>= 10;
              unit = "kB";
            }

          if (normsize >= 1024)
            {
              normsize >>= 10;
              unit = "MB";
            }

          Printf ("Address range: %08lx - %08lx (%ld %s).\n", blkaddr,
           blkaddr + blksize - 1, normsize, (LONG)unit);
         }
       }
      Prm_GetBoardAttrsTags(board,
       PRM_ROM_Address, (ULONG)&romaddr,
       PRM_ROM_Size, (ULONG)&romsize,
       TAG_END);
      if (romaddr && romsize)
       {
        Printf ("%ld kB of ROM at %08lx - %08lx.\n", romsize >> 10, romaddr,
         romaddr + romsize - 1);
       }
      if (owner)
        {
          if (owner->ln_Name) owner_name = owner->ln_Name;
          else owner_name = "unknown";
        }
      else owner_name = "NONE";
      Printf ("Board driver: \033[32m%s\033[0m.\n", (ULONG)owner_name);
      Printf ("-------------------------------------------------\n");
     }

    if (vendors_file) Close(vendors_file);

    Printf ("\n");
    CloseLibrary (PrometheusBase);
   }
  else Printf ("Can't open prometheus.library v2.\n\n");
  return 0;
 }

