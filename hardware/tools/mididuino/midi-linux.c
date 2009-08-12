/*
 * 
 * Copyright (C) 1999-2008 Manuel Odendahl, Steve Ratcliffe
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 */

#include <stdio.h>
#include <alsa/asoundlib.h>

#include "midi.h"

snd_rawmidi_t *handle_in;
snd_rawmidi_t *handle_out;

/* listDevices out of pmidi by Steve Ratcliffe */

static int is_input(snd_ctl_t *ctl, int card, int device, int sub)
{
  snd_rawmidi_info_t *info;
  int err;
  
  snd_rawmidi_info_alloca(&info);
  snd_rawmidi_info_set_device(info, device);
  snd_rawmidi_info_set_subdevice(info, sub);
  snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
  
  if ((err = snd_ctl_rawmidi_info(ctl, info)) < 0 && err != -ENXIO)
    return err;
  else if (err == 0)
    return 1;
  
  return 0;
}

static int is_output(snd_ctl_t *ctl, int card, int device, int sub)
{
  snd_rawmidi_info_t *info;
  int err;
  
  snd_rawmidi_info_alloca(&info);
  snd_rawmidi_info_set_device(info, device);
  snd_rawmidi_info_set_subdevice(info, sub);
  snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
  
  if ((err = snd_ctl_rawmidi_info(ctl, info)) < 0 && err != -ENXIO)
    return err;
  else if (err == 0)
    return 1;
  
  return 0;
}

void list_input_device(snd_ctl_t *ctl, int card, int device) {
  snd_rawmidi_info_t *info;
  const char *name;
  const char *sub_name;
  int subs, subs_in, subs_out;
  int sub, in, out;
  int err;
  
  snd_rawmidi_info_alloca(&info);
  snd_rawmidi_info_set_device(info, device);
  
  snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
  snd_ctl_rawmidi_info(ctl, info);
  subs = snd_rawmidi_info_get_subdevices_count(info);
  
  sub = 0;
  in = out = 0;
  if ((err = is_input(ctl, card, device, sub)) < 0) {
    fprintf(stderr, "Cannot get raw information: %s\n", snd_strerror(err));
    exit(1);
  }
  
  name = snd_rawmidi_info_get_name(info);
  sub_name = snd_rawmidi_info_get_subdevice_name(info);
  if (sub_name[0] == '\0') {
    if (subs == 1) {
      printf("hw:%d,%d)    %s\n", 
	     card, device, name);
    } else
      printf("hw:%d,%d)    %s (%d subdevices)\n",
	     card, device, name, subs);
  } else {
    sub = 0;
    for (;;) {
      printf("hw:%d,%d,%d)  %s\n",
	     card, device, sub, sub_name);
      if (++sub >= subs)
	break;
      snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
      if ((err = snd_ctl_rawmidi_info(ctl, info)) < 0) {
	error("cannot get rawmidi information %d:%d:%d: %s",
	      card, device, sub, snd_strerror(err));
	exit(1);
      }
      sub_name = snd_rawmidi_info_get_subdevice_name(info);
    }
  }
}
      
void list_card_input_devices(int card) {
  snd_ctl_t *ctl;
  char name[32];
  int device;
  int err;
  
  sprintf(name, "hw:%d", card);
  if ((err = snd_ctl_open(&ctl, name, 0)) < 0) {
    error("cannot open control for card %d: %s", card, snd_strerror(err));
    return;
  }
  device = -1;
  for (;;) {
    if ((err = snd_ctl_rawmidi_next_device(ctl, &device)) < 0) {
      error("cannot determine device number: %s", snd_strerror(err));
      break;
    }
    if (device < 0)
      break;
    list_input_device(ctl, card, device);
  }
  snd_ctl_close(ctl);
}  
  

void listInputMidiDevices(void) {
  int card, err;
  
  card = -1;
  if ((err = snd_card_next(&card)) < 0) {
    error("cannot determine card number: %s", snd_strerror(err));
    return;
  }
  if (card < 0) {
    error("no sound card found");
    return;
  }
  do {
    list_card_input_devices(card);
    if ((err = snd_card_next(&card)) < 0) {
      error("cannot determine card number: %s", snd_strerror(err));
      break;
		}
  } while (card >= 0);
}

void list_output_device(snd_ctl_t *ctl, int card, int device) {
  snd_rawmidi_info_t *info;
  const char *name;
  const char *sub_name;
  int subs, subs_in, subs_out;
  int sub, in, out;
  int err;
  
  snd_rawmidi_info_alloca(&info);
  snd_rawmidi_info_set_device(info, device);
  
  snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
  snd_ctl_rawmidi_info(ctl, info);
  subs = snd_rawmidi_info_get_subdevices_count(info);
  
  sub = 0;
  in = out = 0;
  if ((err = is_output(ctl, card, device, sub)) < 0) {
    fprintf(stderr, "Cannot get raw information: %s\n", snd_strerror(err));
    exit(1);
  }
  
  name = snd_rawmidi_info_get_name(info);
  sub_name = snd_rawmidi_info_get_subdevice_name(info);
  if (sub_name[0] == '\0') {
    if (subs == 1) {
      printf("hw:%d,%d)    %s\n", 
	     card, device, name);
    } else
      printf("hw:%d,%d)    %s (%d subdevices)\n",
	     card, device, name, subs);
  } else {
    sub = 0;
    for (;;) {
      printf("hw:%d,%d,%d)  %s\n",
	     card, device, sub, sub_name);
      if (++sub >= subs)
	break;
      snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
      if ((err = snd_ctl_rawmidi_info(ctl, info)) < 0) {
	error("cannot get rawmidi information %d:%d:%d: %s",
	      card, device, sub, snd_strerror(err));
	exit(1);
      }
      sub_name = snd_rawmidi_info_get_subdevice_name(info);
    }
  }
}
      
void list_card_output_devices(int card) {
  snd_ctl_t *ctl;
  char name[32];
  int device;
  int err;
  
  sprintf(name, "hw:%d", card);
  if ((err = snd_ctl_open(&ctl, name, 0)) < 0) {
    error("cannot open control for card %d: %s", card, snd_strerror(err));
    return;
  }
  device = -1;
  for (;;) {
    if ((err = snd_ctl_rawmidi_next_device(ctl, &device)) < 0) {
      error("cannot determine device number: %s", snd_strerror(err));
      break;
    }
    if (device < 0)
      break;
    list_output_device(ctl, card, device);
  }
  snd_ctl_close(ctl);
}  
  

void listOutputMidiDevices(void) {
  int card, err;
  
  card = -1;
  if ((err = snd_card_next(&card)) < 0) {
    error("cannot determine card number: %s", snd_strerror(err));
    return;
  }
  if (card < 0) {
    error("no sound card found");
    return;
  }
  do {
    list_card_output_devices(card);
    if ((err = snd_card_next(&card)) < 0) {
      error("cannot determine card number: %s", snd_strerror(err));
      break;
		}
  } while (card >= 0);
}

void midiSendLong(unsigned char *buf, unsigned long len) {
  int err = snd_rawmidi_write(handle_out, buf, len);
  if (err != len) {
    fprintf(stderr, "could not write %ld byte to output, return: %d\n", len, err);
    exit(1);
  }
}

void midiSendShort(unsigned char status,
		   unsigned char byte1, unsigned char byte2) {
  unsigned char buf[3];
  buf[0] = status;
  buf[1] = byte1;
  buf[2] = byte2;
  midiSendLong(buf, 3);
}


void midiInitialize(char *inputDevice, char *outputDevice) {
  int err = snd_rawmidi_open(&handle_in, NULL, inputDevice, SND_RAWMIDI_NONBLOCK);
  if (err) {
    fprintf(stderr, "could not open input device: %s\n",
	    snd_strerror(err));
    exit(1);
  }

  err = snd_rawmidi_open(NULL, &handle_out, outputDevice, SND_RAWMIDI_NONBLOCK);
  if (err) {
    fprintf(stderr, "could not open output device: %s\n",
	    snd_strerror(err));
    exit(1);
  }
}

unsigned char inbuf[256];
void midiMainLoop(void) {
  int timeout_us_default = 10*10*1000*1000; // 10 seconds in micro seconds
  int timeout_us = timeout_us_default; // 10 seconds in micro seconds
  int timer_resolution_us = 50; // 50 micro seconds

  while (!exitMainLoop) {
    int len;
    len = snd_rawmidi_read(handle_in, inbuf, 256);
    if (len > 0) {
      // reset timeout 
      timeout_us = timeout_us_default;
      //hexdump(inbuf, len);
      int i = 0;
      for (i = 0; i < len; i++)
	midiReceive(inbuf[i]);
    }

    if (len == -EAGAIN) {
      //      printf("usleep\n");
      usleep(timer_resolution_us);
      timeout_us -= timer_resolution_us;
      if (timeout_us <0) {
        fprintf(stderr, "timeout\n");
        exit(1);
      }
    } else if (len < 0) {
      fprintf(stderr, "read error: %s\n", snd_strerror(len));
      exit(1);
    }
  }

  snd_rawmidi_close(handle_in);
  snd_rawmidi_close(handle_out);
}

void midiClose(void) {
  exit(0);
}
