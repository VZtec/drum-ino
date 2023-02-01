



void EEPROMWritelong(int address, long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}


long EEPROMReadlong(long address) {
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);
  
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}




void saveActualSequenceTo(byte preset)
{
    for (byte i = 0; i < VOICE_NUM; i++)
    {
      EEPROMWritelong(preset*PRESET_SIZE + i*8, voiceSequence[i]);
      EEPROMWritelong(preset*PRESET_SIZE + i*8 + 4 , voiceSequenceVol[i]);

      Serial.print(preset*PRESET_SIZE + i*8);
      Serial.print("    ");
      printBits(voiceSequence[i]);
      Serial.print(preset*PRESET_SIZE + i*8 + 4);
      Serial.print("    ");
      printBits(voiceSequenceVol[i]);
      Serial.println();
    }


    EEPROMWritelong(preset*PRESET_SIZE + 48, tempo);
    EEPROM.update(preset*PRESET_SIZE + 50, sequenceLength);

    Serial.print(preset*PRESET_SIZE + 48);
    Serial.print("    ");
    Serial.println(tempo);
    Serial.print(preset*PRESET_SIZE + 50);
    Serial.print("    ");
    Serial.println(sequenceLength);    
    Serial.println("-------------------");

}

void loadSequenceToActual(byte preset)
{
    for (int i = 0; i < VOICE_NUM; i++)
    {
      voiceSequence[i]= EEPROMReadlong(preset*PRESET_SIZE + i*8);
      voiceSequenceVol[i]= EEPROMReadlong(preset*PRESET_SIZE + i*8 + 4);
      Serial.print(preset*PRESET_SIZE + i*4);
      Serial.print("    ");
      printBits(voiceSequence[i]);
      Serial.print(preset*PRESET_SIZE + i*4+VOICE_NUM*2);
      Serial.print("    ");
      printBits(voiceSequenceVol[i]);
      Serial.println();
    }

    tempo = EEPROMReadlong(preset*PRESET_SIZE + 48);
    
    sequenceLength = EEPROM.read(preset*PRESET_SIZE + 50);
    Serial.println("-------------------");

}


void handleNoteOff(byte channel, byte pitch, byte velocity)
{
    // Do something when the note is released.
    // Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.
}


void handleControlChange(byte channel, byte control, byte value)
{
 kTriggerDelay.set(15000/map(value,0,127,40,200));
}





void samplesSampleRate(uint16_t sampleRate)
{
    aKick.setFreq((float) sampleRate / (float) KICK_NUM_CELLS); // play at the speed it was recorded at
    aSnare.setFreq((float) sampleRate / (float) SNARE_NUM_CELLS); // play at the speed it was recorded at
    aClap.setFreq((float) sampleRate / (float) CLAP_NUM_CELLS); // play at the speed it was recorded at
    aHihatClosed.setFreq((float) sampleRate / (float) HIHAT_CLOSED_NUM_CELLS); // play at the speed it was recorded at
    aCowBell.setFreq((float) sampleRate / (float) COW_BELL_NUM_CELLS); // play at the speed it was recorded at
    aTom.setFreq((float) sampleRate / (float) TOM_NUM_CELLS); // play at the speed it was recorded at
}



void displaySequence()
{
  for (int i = 0; i < VOICE_NUM; ++i)
  {
    printBits(voiceSequence[i]);
  }
  

}


void printBits(long int n) 
{
  byte numBits = 32;  // 2^numBits must be big enough to include the number n
  char b;
  char c = ' ';   // delimiter character
  for (byte i = 0; i < numBits; i++) 
  {
    // shift 1 and mask to identify each bit value
    b = (n & (uint32_t(1) << (numBits - 1 - i))) > 0 ? '1' : '0'; // slightly faster to print chars than ints (saves conversion)
    Serial.print(b);
    if (i < (numBits - 1) && ((numBits-i - 1) % 4 == 0 )) Serial.print(c); // print a separator at every 4 bits
  }
  Serial.println();
}


void playSamples(byte voiceSelect)
{
  switch(voiceSelect)
  {
    case KICK: aKick.start(); break;
    case SNARE: aSnare.start(); break;
    case CLAP: aClap.start(); break;
    case HIHAT_CLOSED: aHihatClosed.start(); break;
    case COW_BELL: aCowBell.start(); break;
    case TOM: aTom.start(); break;
  }
}