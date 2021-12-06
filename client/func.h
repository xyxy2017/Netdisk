#ifndef __FUNC__
#define __FUNC__
#include "head.h"
#include "md5.h"
int get_filename(char *cmd,int cmd_len,char *filename);
int get_file(int sfd,char *filepath);
int tran_file(int sfd,char *filename);
#endif
