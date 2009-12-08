#!/usr/bin/perl

use XML::Simple;
use Data::Dumper;

$counter = 0;

$license = <<EOF;

Live API for Python (c)1999-2007 Ableton AG

Ableton grants to you the license to use Live's Python API and to
distribute applications based on the API under the following
conditions.
This license does not affect the terms of Ableton Live's End User
License Agreement or other licenses in any way.

1. Your application is released under the GPL license
   (http://www.gnu.org/licenses/gpl.html).

2. The end user is unmistakably informed about our conditions
   of use by presenting him the following disclaimer wherever
   applicable (download pages, installer dialogs, text files):

   DISCLAIMER: This work is based on an unsupported interface
   of Ableton Live 6.  Use at your own risk.  It is likely that
   things will not work with future versions of Live.  Please
   do not use it for applications which you need to rely on.

   Manipulating your Live Sets via this interface or software
   based on it might damage your documents in a way that they
   not longer can be opened in the current or future versions
   of Live.

   Please do not contact Ableton about questions and problems
   with this software.
EOF


my $ref = XMLin($ARGV[0], ForceArray => 1);

printf("<html><head><link href='../liveapi.css' rel='stylesheet' type='text/css'></head><title>LiveAPI.org: Ableton Live API Documentation - %s.%s</title><body>\n", $ref->{title}->[0]->{module}, $ref->{title}->[0]->{class});

printf("<h1>%s.%s</h1>\n", $ref->{title}->[0]->{module}, $ref->{title}->[0]->{class});
printf("<pre class='ableton_license'>$license</pre>\n");

printf("<TABLE width='100%'>");


printf("<TR><TH class='docheader'>Classes</TH><TD><TABLE WIDTH='100%'>\n");

foreach $class (@{$ref->{classes}->[0]->{class}}) {
	if ($counter % 2) {
		$bg = 'odd';
	} else {
		$bg = 'even';
	}
	$counter++;

	printf("\t<TR class='%s'><TD class='class_name'>%s</TD><TD class='class_doc'>%s</TD></TR>\n", $bg, $class->{name}->[0], $class->{doc}->[0]);
}

printf("</TABLE></TD><TR><TR><TD COLSPAN=2><HR></HR></TD><TR>");

printf("<TR><TH class='docheader'>Methods</TH><TD><TABLE WIDTH='100%'>\n");

foreach $method (@{$ref->{methods}->[0]->{method}}) {
	if ($counter % 2) {
		$bg = 'odd';
	} else {
		$bg = 'even';
	}
	$counter++;

	printf("\t<TR class='%s'><TD class='method_name'>%s</TD><TD class='method_doc'>%s</TD></TR>\n", $bg, $method->{name}->[0], $method->{doc}->[0]);
}

printf("</TABLE></TD><TR><TR><TD COLSPAN=2><HR></HR></TD><TR>");

printf("<TR><TH class='docheader'>Attributes</TH><TD><TABLE WIDTH='100%'>\n");

foreach $attribute (@{$ref->{attributes}->[0]->{attribute}}) {
	if ($counter % 2) {
		$bg = 'odd';
	} else {
		$bg = 'even';
	}
	$counter++;

	$type = $attribute->{type}->[0];

	# Replace object references with HTML links..

	$type =~ s/type//g;
	$type =~ s/Clip.Clip/<A HREF="Clip.Clip.html">Clip.Clip<\/A>/g;
	$type =~ s/ClipSlot.ClipSlot/<A HREF="ClipSlot.ClipSlot.html">ClipSlot.ClipSlot<\/A>/g;
	$type =~ s/Device.Device/<A HREF="Device.Device.html">Device.Device<\/A>/g;
	$type =~ s/DeviceParameter.DeviceParameter/<A HREF="DeviceParameter.DeviceParameter.html">DeviceParameter.DeviceParameter<\/A>/g;
	$type =~ s/MixerDevice.MixerDevice/<A HREF="MixerDevice.MixerDevice.html">MixerDevice.MixerDevice<\/A>/g;
	$type =~ s/Scene.Scene/<A HREF="Scene.Scene.html">Scene.Scene<\/A>/g;
	$type =~ s/Song.BeatTime/<A HREF="Song.BeatTime.html">Song.BeatTime<\/A>/g;
	$type =~ s/Song.CuePoint/<A HREF="Song.CuePoint.html">Song.CuePoint<\/A>/g;
	$type =~ s/Song.Quantization/<A HREF="Song.Quantization.html">Song.Quantization<\/A>/g;
	$type =~ s/Song.Song/<A HREF="Song.Song.html">Song.Song<\/A>/g;
	$type =~ s/Song.View/<A HREF="Song.View.html">Song.View<\/A>/g;
	$type =~ s/Track.Track/<A HREF="Track.Track.html">Track.Track<\/A>/g;
	$type =~ s/Track.View/<A HREF="Track.View.html">Track.View<\/A>/g;


	$value = $attribute->{value}->[0];


	# Strip object memory placement..
	$value =~ s/ object at .x........//g;

	# Replace object references with HTML links..
	$value =~ s/Clip.Clip/<A HREF="Clip.Clip.html">Clip.Clip<\/A>/g;
	$value =~ s/ClipSlot.ClipSlot/<A HREF="ClipSlot.ClipSlot.html">ClipSlot.ClipSlot<\/A>/g;
	$value =~ s/Device.Device/<A HREF="Device.Device.html">Device.Device<\/A>/g;
	$value =~ s/DeviceParameter.DeviceParameter/<A HREF="DeviceParameter.DeviceParameter.html">DeviceParameter.DeviceParameter<\/A>/g;
	$value =~ s/MixerDevice.MixerDevice/<A HREF="MixerDevice.MixerDevice.html">MixerDevice.MixerDevice<\/A>/g;
	$value =~ s/Scene.Scene/<A HREF="Scene.Scene.html">Scene.Scene<\/A>/g;
	$value =~ s/Song.BeatTime/<A HREF="Song.BeatTime.html">Song.BeatTime<\/A>/g;
	$value =~ s/Song.CuePoint/<A HREF="Song.CuePoint.html">Song.CuePoint<\/A>/g;
	$value =~ s/Song.Quantization/<A HREF="Song.Quantization.html">Song.Quantization<\/A>/g;
	$value =~ s/Song.Song/<A HREF="Song.Song.html">Song.Song<\/A>/g;
	$value =~ s/Song.View/<A HREF="Song.View.html">Song.View<\/A>/g;
	$value =~ s/Track.Track/<A HREF="Track.Track.html">Track.Track<\/A>/g;
	$value =~ s/Track.View/<A HREF="Track.View.html">Track.View<\/A>/g;

	printf("\t<TR class='%s'><TD class='attribute_name'>%s</TD><TD class='attribute_type'>%s</TD><TD class='attribute_value'>%s</TD></TR>\n", $bg, $attribute->{name}->[0], $type, $value);
}
printf("</TABLE></TD><TR><TR><TD COLSPAN=2><HR></HR></TD><TR>");

printf("<TR><TH class='docheader'>Listeners</TH><TD><TABLE WIDTH='100%'>\n");

foreach $listener (@{$ref->{listeners}->[0]->{listener}}) {
	if ($counter % 2) {
		$bg = 'odd';
	} else {
		$bg = 'even';
	}
	$counter++;

	printf("\t<TR class='%s'><TD class='attribute_name'>%s</TD><TD class='attribute_type'>%s</TD><TD class='attribute_value'>%s</TD></TR>\n", $bg, $listener->{name}->[0], "", "");
}
printf("</TABLE></TD><TR><TR><TD COLSPAN=2><HR></HR></TD><TR>");


printf("<TR><TH class='docheader'>Builtins</TH><TD><TABLE WIDTH='100%'>\n");
foreach $builtin (@{$ref->{builtin}->[0]->{name}}) {

	if ($counter % 2) {
		$bg = 'odd';
	} else {
		$bg = 'even';
	}
	$counter++;

	printf("\t<TR class='%s'><TD colspan=2>%s</TD></TR>\n", $bg, $builtin);
}
printf("</TABLE></TD><TR><TR><TD COLSPAN=2><HR></HR></TD><TR>");


printf("</TABLE></BODY></HTML>\n");

