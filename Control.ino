void updateControl()
{  
  
  if (keyboard.available()) 
  {
    uint16_t key = keyboard.read();
    handleNoteOn(key);

    //Serial.println(key);
    //Serial.println();
  }



  if(++sequenceTimerIndex >= tempo/4)
  {
    sequenceNext=1;
    sequenceTimerIndex=0;

    if(resetStutter)
    {
      resetStutter=0;
      stutterTimerIndex=0;
    }
    
    //sequenceIndex++;
    if(++sequenceIndex > sequenceLength)
    {
      sequenceIndex=0;
      //if(playStop)
        //Serial.println("----------------");
    }




  }
  
  if(++recSeqTimerIndex >= tempo/8)
  {
    recSeqTimerIndex=0;

    if(recSequenceIndex++ > sequenceLength * 2)
      recSequenceIndex=0;    


    if(playStop)
    {
      //Serial.print(sequenceIndex);
      //Serial.print("    ");
      //Serial.print(recSequenceIndex);
      //Serial.print("    ");
      //Serial.print(((recSequenceIndex+1)/2)) % (sequenceLength+1);
      //Serial.print("    ");
      //Serial.println(2*((recSequenceIndex+2)/4)) % (sequenceLength+1);

    }

  }


  if(tapTempoTimerIndex < 60000)
    tapTempoTimerIndex++;
  else
    toggleTap2=1;
  
  if(toggleTap)
  {  
    toggleTap2 = !toggleTap2;
    if(toggleTap2)
    {
      tempo=tapTempoTimerIndex;
      Serial.println(CONTROL_RATE*60.0/tapTempoTimerIndex);
    }
    tapTempoTimerIndex=0;
    toggleTap=0;
  }


  if(stutterTimerIndex++ >= stutterTempo)
  {
    stutterTimerIndex=0;
    if(stutterToggle)
    {
      voiceVolume[voiceSelect]=lastSelectedVol*2+1;
      playSamples(voiceSelect);
    }
  }



  if(sequenceNext)
  {
    sequenceNext=0;
    if(playStop)
    {
      for (int i = 0; i < VOICE_NUM; i++)
      {
        if(voiceSequence[i] & (uint32_t(1) << (sequenceIndex)))
        {
          voiceVolume[i]=bool((voiceSequenceVol[i] & (uint32_t(1) << (sequenceIndex))))*2+1;    // 
          if(voiceMute[i]==0)
          {
            switch(i)
            {
              case KICK: aKick.start(); break;
              case SNARE: aSnare.start(); break;
              case CLAP: aClap.start(); break;
              case HIHAT_CLOSED: aHihatClosed.start(); break;
              case COW_BELL: aCowBell.start(); break;
              case TOM: aTom.start(); break;
            }
            
          }
        }
      }
    }


  }

  if(fxSrateDown && fxSrateDownToggle)
  {
    fxSrateDownToggle=0;

    #define FX_SRATE_DOWN_RATE  8192
    samplesSampleRate(FX_SRATE_DOWN_RATE);

  }
  else if(fxSrateDown==0 && fxSrateDownToggle)
  {
    fxSrateDownToggle = 0;
    samplesSampleRate(AUDIO_RATE);
  }

  if(fxSrateUp && fxSrateUpToggle)
  {
    fxSrateUpToggle=0;
    #define FX_SRATE_UP_RATE  32767
    samplesSampleRate(FX_SRATE_UP_RATE);
  }
  else if(fxSrateUp==0 && fxSrateUpToggle)
  {
    fxSrateUpToggle = 0;
    samplesSampleRate(AUDIO_RATE);
  }

  if(fxGlissToggle)
  {
    if(fxGlissTimerIndex++ >=fxGlissTimerCount)
    {
      fxGlissTimerIndex=0;
      if(fxGlissDirection==FX_GLISS_DIRECTION_UP)
        oscilTimerCount--;
      else if(fxGlissDirection==FX_GLISS_DIRECTION_DOWN)
        oscilTimerCount++;      
    }
  }



//     // subtract the last reading:
//  total = total - readings[readIndex];
//  // read from the sensor:
//  readings[readIndex] = mozziAnalogRead(A0)>>3;
//  // add the reading to the total:
//  total = total + readings[readIndex];
//  // advance to the next position in the array:
//  readIndex = readIndex + 1;
//
//  // if we're at the end of the array...
//  if (readIndex >= numReadings) {
//    // ...wrap around to the beginning:
//    readIndex = 0;
//  }
//
//  // calculate the average:
//  average = total / numReadings;
//
//
//  if(averagePrev != average)
//  {
//    averagePrev=average;
//
//    potProcesado= map(average,0,255,8192,32767);
//
//    samplesSampleRate(potProcesado);
//    Serial.println(average);
//
//  }
  

  //envelope.update();


  //MIDI.read();
  //Serial.println(tempo);
  
}