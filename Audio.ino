AudioOutput_t updateAudio()
{

  if(oscilToggle)
  {
    if(oscilTimerIndex++ >= oscilTimerCount)
    {
      oscilTimerIndex=0;
      oscilOut = !oscilOut;
    }
  }





    asig1= aKick.next()*voiceVolume[KICK];
    asig2= aSnare.next()*voiceVolume[SNARE];
    asig3= aClap.next()*voiceVolume[CLAP];
    asig4= aHihatClosed.next()*voiceVolume[HIHAT_CLOSED];
    asig5= aCowBell.next()*voiceVolume[COW_BELL];   
    if(fxGlissToggle==0) 
      asig6= aTom.next()*voiceVolume[TOM];  
    if(oscilToggle)  
      asig7=(oscilOut<<(6+lastSelectedVol));// * (recSequenceIndex % 2);
//    if(oscilToggle)
//      asig7=aSin.next()*(lastSelectedVol*2+1);
//    else
//      asig7=0;

  

  if(initSamples==0)  // timeout para que no suenen los samples al inicializar
    masterOut=asig1+asig2+asig3+asig4+asig5+asig6+asig7;//+asig9;
  else
    initSamples--;

  // clip to keep sample loud but still in range
  //return MonoOutput::fromAlmostNBit(9, asig1+asig2+asig3).clip();
  if(fxSrateDown || fxSrateUp)
    masterOut=masterOut * 4 >> 1;

  if(fxDistortion)
    masterOut= int(masterOut<<8)>>7;
  
  if(fxBitCrush)
    masterOut=byte(masterOut>>1);


  
  return MonoOutput::fromAlmostNBit(9, masterOut).clip();

}
