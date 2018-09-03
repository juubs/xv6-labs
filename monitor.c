// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include "stdio.h"
#include "memlayout.h"
#include "x86.h"
#include "types.h"
#include "string.h"
#include "kdebug.h"

#include "monitor.h"

#define CMDBUF_SIZE 80 // enough for one VGA text line

struct Command {
  const char *name;
  const char *desc;
  // return -1 to force monitor to exit
  int (*func)(int argc, char **argv, struct Trapframe * tf);
};

static struct Command commands[] = {
  { "help",      "Display this list of commands",        mon_help       },
  { "info-kern", "Display information about the kernel", mon_infokern   },
  { "backtrace", "Display backtrace",			 mon_backtrace	},
};
#define NCOMMANDS (sizeof(commands)/sizeof(commands[0]))

/***** Implementations of basic kernel monitor commands *****/

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
  int i;

  for (i = 0; i < NCOMMANDS; i++)
    cprintf("%s - %s\n", commands[i].name, commands[i].desc);
  return 0;
}

int
mon_infokern(int argc, char **argv, struct Trapframe *tf)
{
  extern char _start[], entry[], etext[], edata[], end[];

  cprintf("Special kernel symbols:\n");
  cprintf("  _start                  %08x (phys)\n", _start);
  cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
  cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
  cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
  cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
  cprintf("Kernel executable memory footprint: %dKB\n",
          ROUNDUP(end - entry, 1024) / 1024);
  return 0;
}

void trace_helper(int ebp, int eip);

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
 //get current ebp
 uint *ebp = (void*)read_ebp(); 
 cprintf("Stack backtrace:\n");
 cprintf(" ebp %8x eip %8x args %8x %8x %8x %8x %8x\n", ebp[0], ebp[1], ebp[2], ebp[3], ebp[4], ebp[5], ebp[6]);
 struct Eipdebuginfo info;
 debuginfo_eip(ebp[1], &info);
 cprintf("   %s:%d:\n   %.*s+%x\n", info.eip_file, info.eip_line, info.eip_fn_namelen, info.eip_fn_name, ebp[1] - info.eip_fn_addr);
 //recurse until at beginning of OS stack 
 trace_helper(ebp[0], ebp[1]);
 return 0;
}

void
trace_helper(int ebp, int eip) {
 //cast ebp into pointer for older ebp
 int *ebp_p = (void*)ebp;
 if (ebp_p[0] < KERNBASE) return;
 cprintf(" ebp %8x eip %8x args %8x %8x %8x %8x %8x\n", ebp_p[0], ebp_p[1], ebp_p[2], ebp_p[3], ebp_p[4], ebp_p[5], ebp_p[6]);
 struct Eipdebuginfo info;
 debuginfo_eip(eip, &info);
 cprintf("   %s:%d:\n   %.*s+%x\n", info.eip_file, info.eip_line, info.eip_fn_namelen, info.eip_fn_name, eip - info.eip_fn_addr);
 //recurse on ebp value found on old stack
 trace_helper(ebp_p[0], ebp_p[1]); 
 return;
}

/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
  int argc;
  char *argv[MAXARGS];
  int i;

  // Parse the command buffer into whitespace-separated arguments
  argc = 0;
  argv[argc] = 0;
  while (1) {
    // gobble whitespace
    while (*buf && strchr(WHITESPACE, *buf))
      *buf++ = 0;
    if (*buf == 0)
      break;

    // save and scan past next arg
    if (argc == MAXARGS-1) {
      cprintf("Too many arguments (max %d)\n", MAXARGS);
      return 0;
    }
    argv[argc++] = buf;
    while (*buf && !strchr(WHITESPACE, *buf))
      buf++;
  }
  argv[argc] = 0;

  // Lookup and invoke the command
  if (argc == 0)
    return 0;
  for (i = 0; i < NCOMMANDS; i++)
    if (strcmp(argv[0], commands[i].name) == 0)
      return commands[i].func(argc, argv, tf);
  cprintf("Unknown command '%s'\n", argv[0]);
  return 0;
}

void
monitor(struct Trapframe *tf)
{
  char *buf;

  cprintf("Welcome to the xv6 kernel monitor!\n");
  cprintf("Type 'help' for a list of commands.\n");

  while (1) {
    buf = readline("K> ");
    if (buf != NULL)
      if (runcmd(buf, tf) < 0)
        break;
  }
}
