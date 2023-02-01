void handleNoteOn(uint16_t key)
{
  //primero las teclas que queremos usar la repeticion por hardware
  if(key==UP_KEY && tempo>10)
  {
    tempo=tempo - 5;
    Serial.println(CONTROL_RATE*60.0/tempo);
  }
  else if(key==DOWN_KEY && tempo<60000)
  {
    tempo=tempo + 5;
    Serial.println(CONTROL_RATE*60.0/tempo);
  }

  // luego las teclas que queremos filtrar las repeticiones
  else if(key != prevKey)
  {
    prevKey=key;

  // dentro de este for seteamos lo que es STEP dependent
  // eso es escritura de secuencia, length y jump

    for (int i = 0; i < SEQ_MAX_LENGTH; ++i)
    {
      if(key==sequenceIndexKeys[i])
      {
        if(voiceSelect==LENGTH)
          sequenceLength = i + stepsLayer*16;
        else if(voiceSelect==JUMP)
        {
          sequenceIndex = i + stepsLayer*16;
          recSequenceIndex = i + stepsLayer*16;
        }
        else
        {
          voiceSequence[voiceSelect] ^= (uint32_t(1) << (i + stepsLayer*16));
          if(lastSelectedVol)
            voiceSequenceVol[voiceSelect] |= (uint32_t(1) << (i + stepsLayer*16));
          else
            voiceSequenceVol[voiceSelect] &= ~(uint32_t(1) << (i + stepsLayer*16));
        }
      }
    }

    // dentro de este for seteamos lo que es SCALE dependent
    // eso es ajuste de freq segun nota

    for (int i = 0; i < SCALE_MAX_LENGTH; ++i)
    {
      if(key==scaleOnIndexKeys[i])
      {
        oscilToggle=1;
        oscilTimerCount = float(1.0/mtof(scale[i] + 60))*AUDIO_RATE;
        aSin.setFreq(mtof(scale[i] + 52));
        pressedKeys[pressedKeysIndex] = i;
        pressedKeysIndex++;
        //stutterToggle=1;
        noteSelected=i;  
        //stutterTempo = float(1.0/mtof(scale[i] + 40))*AUDIO_RATE;

      }
      if(key==scaleOffIndexKeys[i])
      {
        if(pressedKeysIndex != 0)
        {
          pressedKeysIndex--;
          //oscilTimerCount = float(1.0/mtof(scale[i] + 40))*AUDIO_RATE;
          //stutterTempo = float(1.0/mtof(scale[pressedKeys[pressedKeysIndex]] + 40))*AUDIO_RATE;
        }
        if(pressedKeysIndex==0)
        {
          //stutterToggle=0;
          oscilToggle=0;
          prevKey=0;
        }

        Serial.println(pressedKeysIndex);
      }
    }


    /*
    else if(key==STUTTER_16_ON_KEY || key==STUTTER_8_ON_KEY || key==STUTTER_5_ON_KEY || key==STUTTER_6_ON_KEY) 
      {
        stutterToggle=1;
        //stutterTimerIndex=0;
        if(key==STUTTER_16_ON_KEY)
          stutterTempo=tempo/16;
        else if(key==STUTTER_8_ON_KEY)
          stutterTempo=tempo/64;
        else if(key==STUTTER_6_ON_KEY)
          stutterTempo=tempo/128;
        else if(key==STUTTER_5_ON_KEY)
          stutterTempo=tempo/150;
      }
      else if(key==STUTTER_16_OFF_KEY || key==STUTTER_8_OFF_KEY || key==STUTTER_5_OFF_KEY || key==STUTTER_6_OFF_KEY) 
        stutterToggle=0;
        */

      // dentro de este for seteamos lo que es VOICE dependent
      // eso es rec y play

    for (int i = 0; i < VOICE_NUM; ++i)
    {
      if(key==voiceIndex[i])
      {
        voiceSelect=i;
        if(voiceMode==MODE_PLAY || voiceMode==MODE_REC_8TH || voiceMode==MODE_REC_16TH)
        {
          voiceVolume[voiceSelect]=lastSelectedVol*2+1; // * 2 + 1 escalo 0-1 a 1-3
          playSamples(voiceSelect);          
        }
        if(voiceMode==MODE_REC_8TH && playStop)
        {
          voiceSequence[voiceSelect] |= uint32_t(1) << byte(((2*((recSequenceIndex+2)/4)) % (sequenceLength+1)));
          voiceSequenceVol[voiceSelect] |= (uint32_t(lastSelectedVol) << byte(((2*((recSequenceIndex+2)/4)) % (sequenceLength+1))));
          //reloadSequenceToggle=1;
          Serial.println(((2*((recSequenceIndex+2)/4)) % (sequenceLength+1)));
        }
        else if(voiceMode==MODE_REC_16TH && playStop)
        {
          voiceSequence[voiceSelect] |= uint32_t(1) << byte(((((recSequenceIndex+1)/2)) % (sequenceLength+1)));
          voiceSequenceVol[voiceSelect] |= (uint32_t(lastSelectedVol) << byte(((((recSequenceIndex+1)/2)) % (sequenceLength+1))));
          //reloadSequenceToggle=1;
          //printBits(voiceSequence[i]);
          Serial.println((((((recSequenceIndex)/2)) % (sequenceLength+1))));
        }
      }
    }



    if(key==LENGTH_KEY)   //  voice 
      voiceSelect=LENGTH;
    else if(key==STEPS_1_16__KEY)
      stepsLayer=0;
    else if(key==STEPS_17_32__KEY)
      stepsLayer=1;
    else if(key==JUMP_STEP_KEY)
      voiceSelect=JUMP;
    else if(key==PP_KEY)
      lastSelectedVol=0;
    else if(key==FF_KEY)
      lastSelectedVol=1;
    else if(key==VOICE_CLEAR_KEY)   // clear voice
    {
      voiceSequence[voiceSelect]=0;
      voiceSequenceVol[voiceSelect]=0;
    }
    else if(key==CLEAR_ALL_KEY)
    {
      for (int i = 0; i < VOICE_NUM; i++)
      {
        voiceSequence[i]=0;    
        voiceSequenceVol[voiceSelect]=0;
      }      
    }
    else if(key==RESET_KEY)  // reset
    {
      recSequenceIndex=0;
      sequenceIndex=0;
      sequenceNext=1;
      recSeqTimerIndex=0;
    }
    else if(key==FX_SRATE_DOWN_ON)
    {
      fxSrateDown=1;
      fxSrateDownToggle=1;
    }
    else if(key==FX_SRATE_DOWN_OFF)
    {
      fxSrateDown=0;
      fxSrateDownToggle=1;
    }
    else if(key==FX_SRATE_UP_ON)
    {
      fxSrateUp=1;
      fxSrateUpToggle=1;
    }
    else if(key==FX_SRATE_UP_OFF)
    {
      fxSrateUp=0;
      fxSrateUpToggle=1;
    }
    /////////// DISTORTION ////////////////////

    else if(key==FX_DISTORTION_ON)
      fxDistortion=1;
    else if(key==FX_DISTORTION_OFF)
      fxDistortion=0;  
    else if(key==FX_BIT_CRUSH_ON)
      fxBitCrush=1;
    else if(key==FX_BIT_CRUSH_OFF)
      fxBitCrush=0;    


    /////////// STUTTER ////////////////////
    else if(key==FX_STUTTER_A_ON || key==FX_STUTTER_B_ON || key==FX_STUTTER_C_ON || key==FX_STUTTER_D_ON)
    {
      playSamples(voiceSelect);
      stutterToggle=1;

      switch(key)
      {
        case FX_STUTTER_A_ON: stutterTempo=tempo / FX_STUTTER_A_RATIO    ; break;
        case FX_STUTTER_B_ON: stutterTempo=tempo / FX_STUTTER_B_RATIO; resetStutter=1;break;
        case FX_STUTTER_C_ON: stutterTempo=tempo / FX_STUTTER_C_RATIO;  break;
        case FX_STUTTER_D_ON: stutterTempo=tempo / FX_STUTTER_D_RATIO; resetStutter=1; break;
      }

    }
    else if(key==FX_STUTTER_A_OFF || key==FX_STUTTER_B_OFF || key==FX_STUTTER_C_OFF || key==FX_STUTTER_D_OFF)
      stutterToggle=0;


#define FX_GLISS_MIN_MIN_COUNT     5
#define FX_GLISS_MIN_MAX_COUNT     10
#define FX_GLISS_MAX_MIN_COUNT     50
#define FX_GLISS_MAX_MAX_COUNT     80
    /////////// LASER ////////////////////

    else if(key==FX_GLISS_DOWN_ON)
    {
      fxGlissTimerCount = tempo/32;
      oscilToggle=1;
      fxGlissToggle=1;
      fxGlissDirection=FX_GLISS_DIRECTION_DOWN;
      fxGlissTimerIndex=0;
      oscilTimerCount=random(FX_GLISS_MIN_MAX_COUNT-FX_GLISS_MIN_MIN_COUNT) + FX_GLISS_MIN_MIN_COUNT;
    }
    else if(key==FX_GLISS_DOWN_OFF)
    {
      oscilToggle=0;
      fxGlissToggle=0;
    }
    else if(key==FX_GLISS_UP_ON)
    {
      fxGlissTimerCount = tempo/32;
      oscilToggle=1;
      fxGlissToggle=1;
      fxGlissDirection=FX_GLISS_DIRECTION_UP;
      fxGlissTimerIndex=0;
      oscilTimerCount=random(FX_GLISS_MAX_MAX_COUNT-FX_GLISS_MAX_MIN_COUNT) + FX_GLISS_MAX_MIN_COUNT;
    }
    else if(key==FX_GLISS_UP_OFF)
    {
      oscilToggle=0;
      fxGlissToggle=0;
    }
      



    /////////// START STOP ////////////////////
    else if(key==START_STOP) //play stop
    {
      if(playStop==1)
      {
        playStop=0;
        sequenceIndex=0;
        recSequenceIndex=200; // se resetea en max asi el primer paso es 0
        recSeqTimerIndex=0;
        stutterTimerIndex=0;
        sequenceTimerIndex=0;
      }
      else if(playStop==0)
      {
        sequenceNext=1;
        playStop=1;
        sequenceIndex=0;
        recSequenceIndex=200;
        recSeqTimerIndex=0;
        stutterTimerIndex=0;
        sequenceTimerIndex=0;
      }
    }
    


    else if(key==SAVE_PRESET_KEY_1)
      saveActualSequenceTo(0);
    else if(key==SAVE_PRESET_KEY_2)
      saveActualSequenceTo(2);
    else if(key==SAVE_PRESET_KEY_3)
      saveActualSequenceTo(3);
    else if(key==SAVE_PRESET_KEY_4)
      saveActualSequenceTo(4);

    else if(key==SAVE_PRESET_KEY_5)
      saveActualSequenceTo(5);
    else if(key==SAVE_PRESET_KEY_6)
      saveActualSequenceTo(6);
    else if(key==SAVE_PRESET_KEY_7)
      saveActualSequenceTo(7);
    else if(key==SAVE_PRESET_KEY_8)
      saveActualSequenceTo(8);

    else if(key==SAVE_PRESET_KEY_9)
      saveActualSequenceTo(9);
    else if(key==SAVE_PRESET_KEY_10)
      saveActualSequenceTo(10);
    else if(key==SAVE_PRESET_KEY_11)
      saveActualSequenceTo(11);
    else if(key==SAVE_PRESET_KEY_12)
      saveActualSequenceTo(12);

    else if(key==LOAD_PRESET_KEY_1)
      loadSequenceToActual(0);
    else if(key==LOAD_PRESET_KEY_2)
      loadSequenceToActual(2);
    else if(key==LOAD_PRESET_KEY_3)
      loadSequenceToActual(3);
    else if(key==LOAD_PRESET_KEY_4)
      loadSequenceToActual(4);

    else if(key==LOAD_PRESET_KEY_5)
      loadSequenceToActual(5);
    else if(key==LOAD_PRESET_KEY_6)
      loadSequenceToActual(6);
    else if(key==LOAD_PRESET_KEY_7)
      loadSequenceToActual(7);
    else if(key==LOAD_PRESET_KEY_8)
      loadSequenceToActual(8);

    else if(key==LOAD_PRESET_KEY_9)
      loadSequenceToActual(9);
    else if(key==LOAD_PRESET_KEY_10)
      loadSequenceToActual(10);
    else if(key==LOAD_PRESET_KEY_11)
      loadSequenceToActual(11);
    else if(key==LOAD_PRESET_KEY_12)
      loadSequenceToActual(12);



    else if(key==TAP_TEMPO_KEY)
    {
      toggleTap=1;
      //Serial.println(sequencePeriodMs);
    }
    else if(key==RANDOM_VOICE_KEY)
    {
      uint32_t randomValue1 = random(0xFFFF);
      uint32_t randomValue2 = random(0xFFFF);
      uint32_t randomValue3 = random(0xFFFF);
      uint32_t randomValue4 = random(0xFFFF);
      voiceSequence[voiceSelect]=(uint32_t(randomValue1 & randomValue2) << 16 )+ uint32_t(randomValue3 & randomValue4);
      printBits(voiceSequence[voiceSelect]);
      voiceSequenceVol[voiceSelect]=uint32_t(lastSelectedVol *  0xFFFFFFFF);
      printBits(voiceSequenceVol[voiceSelect]);

    }
    else if(key==MUTE_KEY)
      voiceMute[voiceSelect] = !voiceMute[voiceSelect];
    else if(key==MODE_PLAY_KEY)
      voiceMode=MODE_PLAY;
    else if(key==MODE_REC_8TH_KEY)
      voiceMode=MODE_REC_8TH;
    else if(key==MODE_REC_16TH_KEY)
      voiceMode=MODE_REC_16TH;
    else if(key==MODE_EDIT_KEY)
      voiceMode=MODE_EDIT;

  }
}