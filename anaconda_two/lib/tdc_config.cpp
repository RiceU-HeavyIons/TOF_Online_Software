/*
 * tdc_config.cpp
 *
 *  Created on: Oct 30, 2008
 *      Author: koheik
 */

#include "tdc_config.h"

tdc_config::tdc_config()
{
  // TODO Auto-generated constructor stub

}

tdc_config::~tdc_config()
{
  // TODO Auto-generated destructor stub
}

/*********************************************************************/
unsigned char tdc_config::bits_to_num(unsigned int bs)
{
  unsigned char c = 0;

  for(int i = 0; i < 8; i++)
    c |= ((bs >> 4*i) & 0x1) << i;

  return c;
}

/*********************************************************************/
unsigned char tdc_config::load_from_file(const char *path)
{
  FILE  *fp;

  unsigned short cline = 0;
  unsigned int ln = 0;
  unsigned int csum = 0;

  fp = fopen(path, "r");

  while(fscanf(fp, "%x", &ln) != EOF) {
    data[cline] = bits_to_num(ln);
    cline++;
    csum += bits_to_num(ln);
  }
  length = cline;
  check_sum = csum;
  block_length = (length / 7) + ((length % 7) ? 1 : 2);
  return check_sum;
}

/*********************************************************************/
void tdc_config::set_block_msg(TPCANMsg *msg, int line) {

  msg->DATA[0] = 0x20;
  int len = 8;

  if (line == block_length - 1 && length % 7)
    len = 1 + (length % 7);

  msg->LEN = len;

  for (int j = 1; j < len; ++j)
    msg->DATA[j] = data[7*line + (j-1)];
}

