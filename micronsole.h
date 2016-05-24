/*
	 Micronsole - v1.00 - public domain
	 no warrenty implied; use at your own risk.
	 authored from 2016 by Thomas Versteeg

DEFINES:
MC_PRIVATE - make all the functions static, so they can only be used on the file where MC_IMPLEMENTATION is defined
MC_OUTPUT_TEXTURE_[RGB, RGBA, BGR, BGRA] - render the output as a texture with the defined pixel format
MC_DYNAMIC_ARRAYS - dynamically grow the array size instead of using static sizes
MC_MAX_COMMANDS (n>0) - maximum number of commands that can be registered, only useable when MC_DYNAMIC_ARRAYS is not defined
MC_MAX_COMMAND_LENGTH (n>0) - maximum string length of the command, only useable when MC_DYNAMIC_ARRAYS is not defined
MC_MAX_INPUT_LENGTH (n>0) - maximum length of the input string, only useable when MC_DYNAMIC_ARRAYS is not defined
MC_ASSERT - define the assert function, leave empty for no assertions

TODO:
UTF8 support
Option to replace stdlib

LICENSE:
This software is dual-licensed to the public domain and under the following
license: you are granted a perpetual, irrevocable license to copy, modify,
publish and distribute this file as you see fit.
*/

#ifndef MC_H
#define MC_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef MC_PRIVATE
#define MC_API static
#else
#define MC_API extern
#endif

#if defined MC_OUTPUT_TEXTURE_RGB || defined MC_OUTPUT_TEXTURE_RGBA || defined MC_OUTPUT_TEXTURE_BGR || defined MC_OUTPUT_TEXTURE_BGRA
#define MC_OUTPUT_TEXTURE
#endif

#ifndef MC_MAX_COMMANDS
#define MC_MAX_COMMANDS 128
#endif

#ifndef MC_MAX_INPUT_LENGTH
#define MC_MAX_INPUT_LENGTH 256
#endif

#ifndef MC_MAX_COMMAND_LENGTH
#define MC_MAX_COMMAND_LENGTH 64
#endif

#ifndef MC_ASSERT
#define MC_ASSERT(x) assert(x)
#endif

enum mc_keys {MC_KEY_LEFT, MC_KEY_RIGHT, MC_KEY_UP, MC_KEY_DOWN, MC_KEY_BACKSPACE};

#ifdef MC_OUTPUT_TEXTURE
struct mc_pixel {
#ifdef MC_OUTPUT_TEXTURE_RGB
	unsigned char r, g, b;
#elif defined MC_OUTPUT_TEXTURE_RGBA
	unsigned char r, g, b, a;
#elif defined MC_OUTPUT_TEXTURE_BGR
	unsigned char b, g, r;
#elif defined MC_OUTPUT_TEXTURE_BGRA
	unsigned char b, g, r, a;
#endif
};
#endif // MC_OUTPUT_TEXTURE

typedef struct mc_console _mc_console_t;
typedef void (*mc_cmd_ptr) (_mc_console_t *term, int argc, char **argv);

struct mc_console {
	char *outstr;
	unsigned outlen, outmaxlen;
	unsigned outwidth, outheight;
	bool outupdate;

#ifdef MC_DYNAMIC_ARRAYS
	char *instr;
#else
	char instr[MC_MAX_INPUT_LENGTH];
#endif
	unsigned inpos;

	bool insert;

#ifdef MC_DYNAMIC_ARRAYS
	mc_cmd_ptr *cmdfuncs;
	char **cmds;
#else
	mc_cmd_ptr cmdfuncs[MC_MAX_COMMANDS];
	char cmds[MC_MAX_COMMANDS][MC_MAX_COMMAND_LENGTH];
#endif
	unsigned ncmds;

#ifdef MC_OUTPUT_TEXTURE
	unsigned width, height;
	struct mc_pixel *pixels;
#endif
};

MC_API int mc_create(struct mc_console *con);
MC_API int mc_free(struct mc_console *con);

MC_API int mc_map(struct mc_console *con, const char *cmd, mc_cmd_ptr func);

MC_API int mc_input_key(struct mc_console *con, enum mc_keys key);
MC_API int mc_input_char(struct mc_console *con, char key);

#ifdef MC_OUTPUT_TEXTURE
MC_API int mc_set_texture_size(struct mc_console *con, unsigned width, unsigned height);

MC_API int mc_blit_glyph_default(struct mc_console *con, unsigned x, unsigned y, char glyph);
#endif

#endif // MC_H

#ifdef MC_IMPLEMENTATION

MC_API int mc_create(struct mc_console *con)
{
	MC_ASSERT(con);
	memset(con, 0, sizeof(struct mc_console));

#ifdef MC_DYNAMIC_ARRAYS
	con->instr = (char*)calloc(1, sizeof(char));
	con->outstr = (char*)calloc(1, sizeof(char));
#else
	memset(con->instr, '\0', MC_MAX_INPUT_LENGTH * sizeof(char));
	memset(con->cmdfuncs, 0, MC_MAX_COMMANDS * sizeof(mc_cmd_ptr));
	for(int i = 0; i <  MC_MAX_COMMANDS; i++){
		memset(con->cmds[i], '\0', MC_MAX_COMMAND_LENGTH * sizeof(char));
	}
#endif

	return 0;
}

MC_API int mc_free(struct mc_console *con)
{
	MC_ASSERT(con);

	free(con->outstr);
#ifdef MC_DYNAMIC_ARRAYS
	free(con->instr);
	free(con->cmdfuncs);
	free(con->cmds);
#endif
#ifdef MC_OUTPUT_TEXTURE
	free(con->pixels);
#endif

	return 0;
}

MC_API int mc_map(struct mc_console *con, const char *cmd, mc_cmd_ptr func)
{
	MC_ASSERT(con);
#ifdef MC_DYNAMIC_ARRAYS
	con->cmdfuncs = (mc_cmd_ptr*)realloc(con->cmdfuncs, con->ncmds * sizeof(mc_cmd_ptr));
	con->cmds = (char**)realloc(con->cmds, con->ncmds * sizeof(char*));
#else
	MC_ASSERT(con->ncmds < MC_MAX_COMMANDS);
	if(con->ncmds == MC_MAX_COMMANDS){
		return -1;
	}
#endif

	con->cmdfuncs[con->ncmds] = func;
#ifdef MC_DYNAMIC_ARRAYS
	con->cmds[con->ncmds] = (char*)malloc(strlen(cmd));
#endif
	strcpy(con->cmds[con->ncmds], cmd);

	con->ncmds++;

	return 0;
}

MC_API int mc_input_key(struct mc_console *con, enum mc_keys key)
{
	MC_ASSERT(con);
	switch(key){
		case MC_KEY_LEFT:
			if(con->inpos > 0){
				con->inpos--;
			}
			break;
		case MC_KEY_RIGHT:
			if(con->instr[con->inpos] != '\0'){
				con->inpos++;
			}
			break;
		case MC_KEY_UP:
			break;
		case MC_KEY_DOWN:
			break;
		case MC_KEY_BACKSPACE:
			if(con->inpos > 0){
				con->inpos--;
				memmove(con->instr + con->inpos, con->instr + con->inpos + 1, strlen(con->instr) - con->inpos);
			}
			break;
	}

	return 0;
}

MC_API int mc_input_char(struct mc_console *con, char key)
{
	MC_ASSERT(con);
	if(key == '\0'){
		return -1;
	}

	if(key >= ' ' && key <= '~'){

	}

	switch(key){
		case '\n':

			break;
		case '\t':

			break;
	}

	return 0;
}

// Font converted with ccfconv (ccFont) from Pixerif and converted to binary with xxd
static unsigned char _mc_default_font_bin[] = {
  0x01, 0x0c, 0x0f, 0x21, 0x80, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x5a,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0xe0,
  0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0,
  0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0,
  0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0,
  0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0, 0x00, 0x0e, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x03, 0x00, 0x00, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x01, 0x50, 0x00, 0x28, 0xe0, 0x01, 0x26, 0xc0, 0x01, 0x02, 0x40,
  0x00, 0x01, 0x50, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xc0,
  0x00, 0x02, 0xf0, 0x00, 0x0f, 0x40, 0x00, 0x1f, 0xc0, 0x00, 0x1f, 0xe0,
  0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80,
  0x07, 0x08, 0xf0, 0x00, 0x1c, 0xf0, 0x00, 0x1f, 0xf0, 0x01, 0x1c, 0x70,
  0x07, 0x07, 0xe0, 0x00, 0x77, 0x70, 0x00, 0xc3, 0x30, 0x07, 0x0c, 0xf0,
  0x00, 0x0c, 0xf0, 0x00, 0x1e, 0xf0, 0x01, 0x77, 0x70, 0x07, 0x07, 0x77,
  0x07, 0x77, 0xf0, 0x01, 0x07, 0x10, 0x00, 0x07, 0x80, 0x00, 0x00, 0x20,
  0x00, 0x00, 0x30, 0x00, 0x00, 0xc0, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x30,
  0x00, 0x02, 0x20, 0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x04, 0x10, 0x00, 0x04, 0x00, 0x00, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x01, 0x50, 0x00, 0x28, 0x50, 0x01, 0x29, 0x20, 0x01, 0x02, 0x20,
  0x00, 0x02, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x20,
  0x01, 0x03, 0x00, 0x01, 0x11, 0x40, 0x00, 0x01, 0x20, 0x00, 0x10, 0x10,
  0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x40,
  0x08, 0x08, 0x20, 0x01, 0x12, 0x20, 0x01, 0x12, 0x20, 0x01, 0x12, 0x20,
  0x02, 0x02, 0x80, 0x00, 0x22, 0x20, 0x00, 0x66, 0x60, 0x02, 0x12, 0x20,
  0x01, 0x12, 0x20, 0x01, 0x11, 0x50, 0x01, 0x22, 0x20, 0x02, 0x02, 0x22,
  0x02, 0x22, 0x10, 0x01, 0x01, 0x10, 0x00, 0x04, 0x40, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x20, 0x00, 0x00, 0x80, 0x00, 0x00, 0x20, 0x00, 0x00, 0x20,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x04, 0x10, 0x00, 0x04, 0x00, 0x00, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x01, 0x00, 0x00, 0x7e, 0x50, 0x00, 0x19, 0x20, 0x00, 0x00, 0x10,
  0x00, 0x04, 0xe0, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x10,
  0x02, 0x02, 0x00, 0x01, 0x10, 0x20, 0x00, 0x01, 0x10, 0x00, 0x08, 0x10,
  0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x00, 0x10, 0x20,
  0x17, 0x14, 0x20, 0x01, 0x01, 0x20, 0x02, 0x02, 0x20, 0x00, 0x01, 0x20,
  0x02, 0x02, 0x80, 0x00, 0x12, 0x20, 0x00, 0x66, 0x60, 0x02, 0x21, 0x20,
  0x01, 0x21, 0x20, 0x01, 0x01, 0x40, 0x00, 0x22, 0x20, 0x02, 0x22, 0x42,
  0x01, 0x14, 0x80, 0x00, 0x01, 0x20, 0x00, 0x04, 0x20, 0x02, 0x00, 0x00,
  0x00, 0x07, 0xe0, 0x00, 0x0e, 0xe0, 0x00, 0x06, 0x70, 0x00, 0x16, 0xe0,
  0x00, 0x03, 0x30, 0x00, 0x12, 0x10, 0x00, 0x6d, 0xd0, 0x00, 0x06, 0xd0,
  0x00, 0x16, 0xd0, 0x00, 0x0e, 0x70, 0x00, 0x1b, 0x70, 0x07, 0xc7, 0x31,
  0x03, 0x33, 0xf0, 0x00, 0x04, 0x10, 0x00, 0x04, 0x00, 0x00, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x01, 0x00, 0x00, 0x14, 0xe0, 0x00, 0x16, 0x60, 0x0e, 0x00, 0x10,
  0x00, 0x04, 0x50, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x10,
  0x02, 0x02, 0x80, 0x00, 0x0c, 0xa0, 0x00, 0x0f, 0xf0, 0x00, 0x08, 0xe0,
  0x00, 0x11, 0x10, 0x00, 0x01, 0x80, 0x01, 0x7e, 0x80, 0x01, 0x08, 0x90,
  0x14, 0x14, 0xe0, 0x01, 0x01, 0x20, 0x02, 0x0e, 0xe0, 0x00, 0x01, 0xe0,
  0x03, 0x02, 0x80, 0x00, 0x0e, 0x20, 0x00, 0x5a, 0xa0, 0x02, 0x21, 0xe0,
  0x00, 0x21, 0xe0, 0x00, 0x0e, 0x40, 0x00, 0x22, 0x20, 0x02, 0x22, 0x82,
  0x00, 0x08, 0x40, 0x00, 0x01, 0x20, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x08, 0x20, 0x01, 0x09, 0x90, 0x00, 0x09, 0x20, 0x00, 0x09, 0x20,
  0x01, 0x02, 0x20, 0x00, 0x0a, 0x10, 0x00, 0x92, 0x20, 0x01, 0x09, 0x20,
  0x01, 0x09, 0xa0, 0x00, 0x01, 0x20, 0x00, 0x12, 0x20, 0x02, 0x82, 0x20,
  0x01, 0x12, 0x90, 0x00, 0x04, 0x10, 0x00, 0x04, 0x60, 0x00, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x01, 0x00, 0x00, 0x14, 0x40, 0x01, 0x68, 0x90, 0x04, 0x00, 0x10,
  0x00, 0x04, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x02, 0x10,
  0x02, 0x02, 0x40, 0x00, 0x10, 0x90, 0x00, 0x10, 0x10, 0x01, 0x04, 0x10,
  0x01, 0x1e, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x06, 0x04, 0x50,
  0x14, 0x1c, 0x20, 0x02, 0x01, 0x20, 0x02, 0x02, 0x20, 0x00, 0x19, 0x20,
  0x02, 0x02, 0x80, 0x00, 0x0a, 0x20, 0x00, 0x5a, 0xa0, 0x02, 0x21, 0x20,
  0x00, 0x21, 0xa0, 0x00, 0x10, 0x40, 0x00, 0x22, 0x40, 0x01, 0x54, 0x81,
  0x00, 0x08, 0x20, 0x00, 0x01, 0x40, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0e, 0x20, 0x01, 0x01, 0x90, 0x00, 0x0f, 0x20, 0x00, 0x09, 0x20,
  0x01, 0x02, 0x20, 0x00, 0x06, 0x10, 0x00, 0x92, 0x20, 0x01, 0x09, 0x20,
  0x01, 0x09, 0x20, 0x00, 0x06, 0x20, 0x00, 0x12, 0x20, 0x02, 0x92, 0xc0,
  0x00, 0x12, 0x40, 0x00, 0x03, 0x10, 0x00, 0x18, 0x90, 0x04, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x01, 0x00, 0x00, 0x3f, 0x40, 0x01, 0x98, 0x10, 0x05, 0x00, 0x10,
  0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10,
  0x02, 0x02, 0x20, 0x00, 0x10, 0xf0, 0x01, 0x10, 0x10, 0x01, 0x04, 0x10,
  0x01, 0x10, 0x00, 0x00, 0x00, 0x80, 0x01, 0x7e, 0x80, 0x01, 0x02, 0x50,
  0x12, 0x22, 0x20, 0x02, 0x01, 0x20, 0x02, 0x02, 0x20, 0x00, 0x11, 0x20,
  0x02, 0x02, 0x80, 0x00, 0x12, 0x20, 0x00, 0x42, 0x20, 0x03, 0x21, 0x20,
  0x00, 0x29, 0x20, 0x01, 0x10, 0x40, 0x00, 0x22, 0x40, 0x01, 0x54, 0x41,
  0x01, 0x08, 0x10, 0x00, 0x01, 0x40, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x09, 0x20, 0x01, 0x01, 0x90, 0x00, 0x01, 0x20, 0x00, 0x09, 0x20,
  0x01, 0x02, 0x20, 0x00, 0x0a, 0x10, 0x00, 0x92, 0x20, 0x01, 0x09, 0x20,
  0x01, 0x09, 0x20, 0x00, 0x08, 0x20, 0x00, 0x12, 0x40, 0x01, 0x54, 0xc0,
  0x00, 0x0c, 0x20, 0x00, 0x04, 0x10, 0x00, 0x04, 0x00, 0x03, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x0a, 0x50, 0x01, 0x94, 0x10, 0x02, 0x00, 0x20,
  0x00, 0x02, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20,
  0x01, 0x02, 0x10, 0x01, 0x11, 0x80, 0x00, 0x11, 0x10, 0x01, 0x02, 0x10,
  0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x60, 0x00, 0x00, 0x90,
  0x0f, 0x22, 0x20, 0x02, 0x12, 0x20, 0x01, 0x12, 0x20, 0x00, 0x12, 0x20,
  0x02, 0x02, 0x90, 0x00, 0x22, 0x20, 0x01, 0x42, 0x20, 0x03, 0x12, 0x20,
  0x00, 0x12, 0x20, 0x01, 0x11, 0x40, 0x00, 0x22, 0x80, 0x00, 0x88, 0x20,
  0x02, 0x08, 0x10, 0x01, 0x01, 0x80, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x09, 0x20, 0x01, 0x09, 0x90, 0x00, 0x09, 0x20, 0x00, 0x09, 0x20,
  0x01, 0x02, 0x20, 0x00, 0x0a, 0x10, 0x00, 0x92, 0x20, 0x01, 0x09, 0x20,
  0x01, 0x09, 0x20, 0x00, 0x09, 0x20, 0x00, 0x12, 0x40, 0x01, 0x54, 0x20,
  0x01, 0x0c, 0x90, 0x00, 0x04, 0x10, 0x00, 0x04, 0x00, 0x00, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x00,
  0x01, 0x01, 0x00, 0x00, 0x0a, 0xf0, 0x00, 0x64, 0xe0, 0x0d, 0x00, 0x40,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10, 0x00, 0x01, 0xc0,
  0x00, 0x07, 0xf0, 0x01, 0x0f, 0x80, 0x00, 0x0f, 0xe0, 0x00, 0x02, 0xe0,
  0x00, 0x07, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x20,
  0x20, 0x77, 0xf0, 0x01, 0x1c, 0xf0, 0x00, 0x1f, 0x70, 0x00, 0x1c, 0x70,
  0x07, 0x07, 0x60, 0x00, 0x77, 0xf0, 0x01, 0xe7, 0x70, 0x02, 0x0c, 0x70,
  0x00, 0x6c, 0x70, 0x03, 0x0f, 0xe0, 0x00, 0x1c, 0x80, 0x00, 0x88, 0x70,
  0x07, 0x1c, 0xf0, 0x01, 0x07, 0x80, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1e, 0xd0, 0x00, 0x0e, 0x60, 0x01, 0x0e, 0x70, 0x00, 0x0e, 0x70,
  0x03, 0x07, 0x20, 0x00, 0x17, 0x60, 0x00, 0xb7, 0x71, 0x03, 0x06, 0xe0,
  0x00, 0x0e, 0x70, 0x00, 0x07, 0x40, 0x00, 0x2c, 0x80, 0x00, 0x28, 0x30,
  0x03, 0x08, 0xf0, 0x00, 0x04, 0x10, 0x00, 0x04, 0x00, 0x00, 0x1e, 0xe0,
  0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0,
  0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0,
  0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0,
  0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0, 0x01, 0x1e, 0xe0,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
  0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
  0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
  0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00,
  0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70,
  0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x06, 0x00, 0x00, 0x18, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00
};

static bool _mc_default_font_is_allocated = false;
static int _mc_default_font_glyph_width, _mc_default_font_glyph_height, _mc_default_font_glyph_start, _mc_default_font_glyph_num;
static unsigned _mc_default_font_width;
static unsigned char *_mc_default_font_data;

static int _mc_font_allocate()
{
	if(_mc_default_font_is_allocated){
		return 0;
	}

	if(_mc_default_font_bin[0] != 1){
		return -1;
	}

	_mc_default_font_glyph_width = _mc_default_font_bin[1];
	_mc_default_font_glyph_height = _mc_default_font_bin[2];
	_mc_default_font_glyph_start = _mc_default_font_bin[3];
	_mc_default_font_glyph_num = _mc_default_font_bin[4];

	printf("w: %d, h: %d\n", _mc_default_font_glyph_width, _mc_default_font_glyph_height);

#define _MC_UNPACK8TO32(b, c, i) \
	b = (c[i] << 24) | (c[i + 1] << 16) | (c[i + 2] << 8) | c[i + 3];
	unsigned totallen;
	_MC_UNPACK8TO32(_mc_default_font_width, _mc_default_font_bin, 5);
	_MC_UNPACK8TO32(totallen, _mc_default_font_bin, 9);
#undef _MC_UNPACK8TO32

	_mc_default_font_data = (unsigned char*)malloc(totallen);

	unsigned len = totallen >> 3;
	unsigned char reminder = totallen % 8;

	unsigned i;
	for(i = 0; i < len; i++){
		unsigned char j;
		for(j = 0; j < 8; j++){
			_mc_default_font_data[i * 8 + j] = (_mc_default_font_bin[i + 13] >> j) & 1;
		}
	}

	if(reminder > 0){
		unsigned char j;
		for(j = 0; j < reminder; j++){
			_mc_default_font_data[i * 8 + j] = (_mc_default_font_bin[i + 13] >> j) & 1;
		}
	}

	_mc_default_font_is_allocated = true;

	return 0;
}

MC_API int mc_blit_glyph_default(struct mc_console *con, unsigned x, unsigned y, char glyph)
{
	int c = glyph - _mc_default_font_glyph_start;
	if(c < 0 || c > _mc_default_font_glyph_num){
		return -2;
	}

	int start = c * _mc_default_font_glyph_width;

	int i;
	for(i = 0; i < _mc_default_font_glyph_height; i++){
		int j, ty = (y + i) * con->width;
		for(j = 0; j < _mc_default_font_glyph_width; j++){
			unsigned char bit = (_mc_default_font_data[start + j + i * _mc_default_font_width] & 1) * 255;

			con->pixels[x + j + ty] = (struct mc_pixel){bit, bit, bit
#if defined MC_OUTPUT_TEXTURE_RGBA || defined MC_OUTPUT_TEXTURE_BGRA
				,bit
#endif
			};
		}
	}

	return 0;
}

#ifdef MC_OUTPUT_TEXTURE
MC_API int mc_set_texture_size(struct mc_console *con, unsigned width, unsigned height)
{
	MC_ASSERT(con);

	if(con->pixels){
		con->pixels = (struct mc_pixel*)realloc(con->pixels, width * height * sizeof(struct mc_pixel));
	}else{
		con->pixels = (struct mc_pixel*)malloc(width * height * sizeof(struct mc_pixel));
		_mc_font_allocate();
	}

	con->width = width;
	con->height = height;

	return 0;
}
#endif // MC_OUTPUT_TEXTURE



#endif // MC_IMPLEMENTATION
