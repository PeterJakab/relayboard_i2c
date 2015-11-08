#include "relayctl.h"
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

char binary[9];

void print_help(void)
{
     fprintf(stderr, "Usage: relayctl [options] <command> [value]\n"
       "Control the relay board\n\n"
       " -b, --bus <I2C bus>\n"
       "    Use the given I2C bus to access the relay board, default=%s\n"
       " -a, --address <I2C address>\n"
       "    Use the given I2C address to access the relay board, default=0x%02x\n"
       " -o, --output <base>\n"
       "    Set output format. Base x=hexadecimal (16), b=binary (2), d=decimal (10), default=x\n"
       " -v, --verbose\n"
       "    Verbose mode\n"
       " -V, --version\n"
       "    Show program version\n"
       " -h, --help\n"
       "    Show usage and help\n\n"
       " <command>: getport | setport <n> | setbit <n> | clearbit <n>\n\n",
       MCP_DEVICE_BUS, MCP_DEVICE_ADDRESS
    );
}

long int convert(char *value)
{
  long int result;
  char *endptr;
  errno = 0;
  if (strncasecmp(value, "0b", 2) == 0)
    result = strtol(&value[2], &endptr, 2);
  else
   result = strtol(value, &endptr, 0);
}

char * itob(unsigned int value)
{
   int i;
   value &= 0xff;

   for (i=0; i < 8; i++)
   {
     if (value & 0x80) binary[i] = '1';
     else binary[i] = '0';

     value <<= 1;
   }

   binary[i] = '\0';
   return binary;
}

void print_value(int value, int base)
{
    switch(base)
  {
    case 16: printf("0x%02hhx\n", value);
             break;
    case 10: printf("%u\n", value);
             break;
    case 2:  printf("0b%s\n", itob(value));
             break;
  } // switch
}

int mcp_open(char *bus, uint8_t address)
{
   int fh;
   fh = open(bus, O_RDWR);
   if (fh < 0)
   {
      perror("open");
      exit(1);
   }

   if (ioctl(fh, I2C_SLAVE, address) < 0)
   {
     perror("ioctl(I2C_SLAVE)");
     exit(1);
   }

   return fh;
}

uint8_t mcp_read(int fh, uint8_t reg)
{
   uint8_t data;
   data = reg;
   if (write(fh, &data, 1) != 1)
   {
     perror("write before read");
     exit(1);
   }

   if (read(fh, &data, 1) != 1)
   {
     perror("read");
     exit(1);
   }

   return data;
}

void mcp_write(int fh, uint8_t reg, uint8_t value)
{
   uint8_t data[2];
   data[0] = reg;
   data[1] = value;

   if (write(fh, &data, 2) != 2)
   {
     perror("write");
     exit(1);
   }
}

int main(int argc, char *argv[])
{
  static const struct option long_options[] =
  {
    { "version", 0, 0, 'V' },
    { "verbose", 0, 0, 'v' },
    { "help", 0, 0, 'h' },
    { "bus", 1, 0, 'b' },
    { "address", 1, 0, 'a' },
    { "output", 1, 0, 'o' },
    { 0, 0, 0, 0 }
  };

  int c, err = 0;
  int verbose = 0, help = 0;
  char *device_bus = MCP_DEVICE_BUS;
  uint8_t device_address = MCP_DEVICE_ADDRESS;
  int base = 16;
  char *cmd;
  unsigned int value, bitmask;
  int fh;

  // process arguments
  while ((c = getopt_long(argc, argv, "Vvhb:a:o:", long_options, NULL)) != EOF)
  {
     switch(c)
     {
       case 'V':
        printf("relayctl " VERSION "\n");
        return 0;

      case 'v':
        verbose++;
        break;

      case 'h':
        help=1;
        break;

      case 'b':
        device_bus = optarg;
        break;

      case 'o':
           switch(optarg[0])
           {
             case 'x': base = 16; break;
             case 'b': base = 2; break;
             case 'd': base = 10; break;
             default: err++; break;
           } // switch
        break;

      case 'a':
        device_address = 0;

        if (*optarg) device_address = convert(optarg);
        break;

      case '?':
      default:
        err++;
        break;
        
     } // switch
  } // while

 if (err || help || argc <= optind)
  {
    print_help();
    return -1;
  } // if

 if (verbose)
 {
    fprintf(stderr, "Using I2C bus %s, I2C address 0x%02x\n", device_bus, device_address);
 }

 // parse command part
 cmd = argv[optind];
 if (strcmp(cmd, "setport") == 0)
 {
    if (optind+1 >= argc ) { print_help(); return -1; }
    value = convert(argv[optind+1]);
    fh = mcp_open(device_bus, device_address);
    mcp_write(fh, MCP_IODIR, 0);
    mcp_write(fh, MCP_OLAT, value);
    close(fh);
    return 0;
 }
  else if (strcmp(cmd, "getport") == 0)
  {
    fh = mcp_open(device_bus, device_address);
    value = mcp_read(fh, MCP_OLAT);
    print_value(value, base);
    close(fh);
    return 0;
  }
  else if (strcmp(cmd, "setbit") == 0)
  {
    if (optind+1 >= argc) { print_help(); return -1; }
    bitmask = convert(argv[optind+1]);
    fh = mcp_open(device_bus, device_address);
    value = mcp_read(fh, MCP_OLAT);
    value |= bitmask;
    mcp_write(fh, MCP_IODIR, 0);
    mcp_write(fh, MCP_OLAT, value);
    close(fh);
    return 0;
  }
  else if (strcmp(cmd, "clearbit") == 0)
  {
    if (optind+1 >= argc) { print_help(); return -1; }
    bitmask = convert(argv[optind+1]);
    fh = mcp_open(device_bus, device_address);
    value = mcp_read(fh, MCP_OLAT);
    value &= ~bitmask;
    mcp_write(fh, MCP_IODIR, 0);
    mcp_write(fh, MCP_OLAT, value);
    close(fh);
    return 0;
  }
  else
  {
      fprintf(stderr, "No such command\n\n");
      print_help();
      return -1;
  }

} // main

