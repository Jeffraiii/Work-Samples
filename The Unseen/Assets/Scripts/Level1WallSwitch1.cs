using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Level1WallSwitch1 : MonoBehaviour
{
    public KeyCode cheatCheck;

    public GameObject[] wallsToBeControlled;

    private bool on = true;

    private void Update()
    {
        if(Input.GetKeyDown(cheatCheck))
        {
            foreach (GameObject Wall in wallsToBeControlled)
            {
                if (Wall.activeSelf)
                {
                    Wall.SetActive(false);
                }
                else if (!Wall.activeSelf)
                {
                    Wall.SetActive(true);
                }
            }
        }
    }
    private void OnTriggerEnter(Collider other)
    {
        if (on)
        {
            AkSoundEngine.PostEvent("Trigger_Blue_Switch_Off", gameObject);
        }
        else
        {
            AkSoundEngine.PostEvent("Trigger_Blue_Switch_On", gameObject);
        }
        on = !on;
        foreach (GameObject Wall in wallsToBeControlled)
        {
            if(Wall.activeSelf)
            {
                Wall.SetActive(false);
            }
            else if (!Wall.activeSelf)
            {
                Wall.SetActive(true);
            }
        }
    }
}
