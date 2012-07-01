// See other tabs...

// The firmware contains two pages, which can be switched with button 1. 
// On the first page the transpose mechanism and the chord mechanism can be enabled or disabled, as well as the current key can be selected. 
// On the second page different settings for the chord mechanism can be selected. 
// One midi cable has to be connected with the 1th midi in port and another midi cable has to be connected to the midi out port. 
// When chord mode is activated just play around with some single notes and see what chords are played. 
// I will make some better version of the firmware the next days showing the current played chord.
//
// The midi channel cc1 selects the type of chord played. 
//
// There are some minor bugs that only one note is allowed to be played by the user at any time and that you cannot play notes overlapping, the next firmware will allow you to play notes overlapping, but the first limitation will still remains. 
//
// I think i will make some different "use"-mode where cc1 select the root octave and the pitch of the notes played select the key and the chord type: c1: major c, c2: minor c, c3: major c7 etc.
