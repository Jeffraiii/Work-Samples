using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ButtonSound : MonoBehaviour {

    public void PlaySelect(){
        AkSoundEngine.PostEvent("Trigger_Menu_Select", gameObject);
    }
    public void PlayMove()
    {
        AkSoundEngine.PostEvent("Trigger_Menu_Move", gameObject);
    }
    public void StopSounds(){
        AkSoundEngine.StopAll();
    }
}
