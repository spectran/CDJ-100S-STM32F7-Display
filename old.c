bytes = ((float)wavfile.FileSize-(float)BufferCtl.fptr)/(float)wavfile.ByteRate/(float)wavfile.NbrChannels*150*2;
track_lenght = ((float)f_tell(&MyFile))/((float)f_size(&MyFile))*((float)rekordbox.spectrum_size);
file_pos_wide = (track_lenght + 0.5);				  				  
fr -= (uint32_t)(wasbytes - bytes);
wasbytes = bytes;
if(fr < 0) {
	fr = 149;
	sec--;
}
if(sec < 0) {
	sec = 59;
	min--;
}
if(min < 0) {
	min = 59;
}


fr += 2;
if(fr > 74) {
	fr = 0;
	sec++;
}
if(sec > 59) {
	sec = 0;
	min++;
}
if(min > 59) {
	min = 0;
}

frame_lenght = (float)mp3FrameInfo.outputSamps/(float)mp3FrameInfo.samprate/(float)mp3FrameInfo.nChans*150;
track_lenght = ((float)f_tell(&MyFile))/((float)f_size(&MyFile))*((float)rekordbox.spectrum_size);
file_pos_wide = (track_lenght + 0.5);
fr -= frame_lenght;
if(fr < 0) {
	fr = 149;
	sec--;
}
if(sec < 0) {
	sec = 59;
	min--;
}
if(min < 0) {
	min = 59;
}			  