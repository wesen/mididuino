#include <jack/jack.h>
#include <jack/midiport.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>    
#include <string.h>
#include <time.h>       
#include <sys/time.h>
#include "midi.h"


#define JACK_CLIENT_NAME "midi-send"

jack_client_t *jack_client = NULL; // the jack client
jack_port_t *jack_midi_output_port = NULL; // jack midi output port
jack_port_t *jack_midi_input_port = NULL; // jack midi input port

void jack_begin() {
  // try to become a client of the JACK server 
  if ((jack_client = jack_client_new (JACK_CLIENT_NAME)) == 0) {
    fprintf (stderr, "jack server not running?\n");
    exit(1);
  }
}

void jack_end() {
  jack_client_close (jack_client); // close the client
  jack_client = NULL;
}

void jack_shutdown() {
  jack_midi_output_port = NULL;
  jack_midi_input_port = NULL;;
  if (jack_client != NULL) jack_end();
}

void listJackPorts(
  const char *port_name_pattern, 
  const char *type_name_pattern,
  unsigned long flags) {

  const char **ports;

  jack_begin();
  
  if ((ports = jack_get_ports (jack_client, port_name_pattern, type_name_pattern, flags)) == NULL) {
    fprintf(stderr, "Cannot find any physical capture ports\n");
    exit(1);
  }

  int i=0;
  while (ports[i]!=NULL) {
    const char* port_name = ports[i];//jack_port_name((jack_port_t *)ports[i]);
    printf("%s\n", port_name);
    i++;
  }
  
  free(ports); // free ports
  jack_end();
}

void listInputMidiDevices(void) {
  listJackPorts(NULL, "midi", JackPortIsPhysical|JackPortIsInput);
}
  
void listOutputMidiDevices(void) {
  listJackPorts(NULL, "midi", JackPortIsPhysical|JackPortIsOutput);
}  

/* **************************************************************************** */

void printhex(const unsigned char* prefix, unsigned char *buf, unsigned long len) {
  printf("%s", prefix);
  unsigned long i=0;
  for (;i<len;i++)
    printf("%X ",buf[i]);
  printf("\n");
}

void midiSendLong(unsigned char *buf, unsigned long len) {
  /*int err = snd_rawmidi_write(handle_out, buf, len);
  if (err != len) {
    fprintf(stderr, "could not write %ld byte to output, return: %d\n", len, err);
    exit(1);
  }*/
  if (jack_midi_output_port == NULL) {
    fprintf(stderr, "midiSendLong failed: output port closed\n");
    exit(1);
  }

  void* output_buf = jack_port_get_buffer (jack_midi_output_port, 1);   
  jack_midi_clear_buffer(output_buf);  

  printhex("send:", buf, len);

  int err = jack_midi_event_write(output_buf, 0, buf, len);
  if (err != 0) { 
    if (err == ENOBUFS) { // if there's not enough space in buffer for event
    }
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
  jack_begin(); // create a new jack client

  // register the midi-send ports
  jack_midi_input_port = jack_port_register (jack_client, "input", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
  if (jack_midi_input_port == NULL) {
    fprintf (stderr, "cannot create input port\n");
    exit(1);
  }

  jack_midi_output_port = jack_port_register (jack_client, "output", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
  if (jack_midi_output_port == NULL) {
      fprintf (stderr, "cannot create output port\n");
      exit(1);
  }

  // activate
  if (jack_activate (jack_client)) {
    fprintf (stderr, "cannot activate client");
    exit(1);
  }

  // connect ports to specified devices

  printf("connecting %s to %s\n", jack_port_name(jack_midi_output_port), inputDevice);
  printf("connecting %s to %s\n", outputDevice, jack_port_name(jack_midi_input_port));

  int err;
  if ((err=jack_connect (jack_client, outputDevice, jack_port_name(jack_midi_input_port)))) {
    fprintf (stderr, "cannot connect input port: %s\n", strerror(err));
    exit(1);
  }
  if ((err=jack_connect (jack_client, jack_port_name(jack_midi_output_port), inputDevice))) {
    fprintf (stderr, "cannot connect output port: %s\n", strerror(err));
    exit(1);
  }

}

unsigned long long time_us() {
  struct timeval time;
  gettimeofday(&time, NULL);		
  return time.tv_sec*1000*1000+time.tv_usec;
}

void midiMainLoop(void) {
      fprintf (stderr, "midiMainLoop\n");

  jack_midi_event_t input_event; 

  int timeout = 10*1000*1000+time_us(); // 10 seconds in microseconds
//int exitMainLoop = 0;
  while (!exitMainLoop) {
    jack_nframes_t nframes = jack_cycle_wait(jack_client);
    /* Get input and output buffer pointers. */
    void* input_buf = jack_port_get_buffer (jack_midi_input_port, nframes);
    //void* output_buf = jack_port_get_buffer (jack_midi_output_port, nframes);

    jack_nframes_t input_event_count = jack_midi_get_event_count(input_buf);
    if (input_event_count>0) {
      unsigned int event_index = 0;
      for (;event_index<input_event_count;event_index++) {
	if (0==jack_midi_event_get(&input_event, input_buf, event_index)) {
	  if (input_event.size > 0) {

	printhex("receive:", input_event.buffer,input_event.size);

	    int i = 0;
	    for (; i < input_event.size; i++) {
	      midiReceive(input_event.buffer[i]);
	    }
	  }
        }
      }
    } 

    if (time_us() > timeout) {
        fprintf(stderr, "timeout\n");
	exit(1);
    }
  }

  jack_end();
      fprintf (stderr, "midiMainLoop:end\n");
}
