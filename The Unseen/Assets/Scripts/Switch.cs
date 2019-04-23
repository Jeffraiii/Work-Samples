using System.Collections;

using System.Collections.Generic;

using UnityEngine;

public class Switch : MonoBehaviour {

    public KeyCode cheatCheck;
    public GameObject[] lights;
    private bool on = true;

    private void Update()
    {
        if (Input.GetKeyDown(cheatCheck))
        {
            foreach (GameObject l in lights)
            {

                if (l.activeSelf)
                {
                    l.SetActive(false);
                }
                else if (!l.activeSelf)
                {
                    l.SetActive(true);
                }
            }
        }
    }

    private void OnTriggerEnter(Collider collision)
    {
        if (on){
            AkSoundEngine.PostEvent("Trigger_Yellow_Switch_Off", gameObject);
        }
        else{
            AkSoundEngine.PostEvent("Trigger_Yellow_Switch_On", gameObject);
        }
        on = !on;
        foreach (GameObject l in lights){

            if (l.activeSelf)
            {
               l.SetActive(false);
            }
            else if (!l.activeSelf)
            {
                l.SetActive(true);
            }
        }
    }

}

