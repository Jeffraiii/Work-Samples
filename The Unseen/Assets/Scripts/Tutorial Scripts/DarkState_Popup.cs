using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;


public class DarkState_Popup : MonoBehaviour
{
    public Text PopupText;

    void Start()
    {
        PopupText.text = "You are in Dark State" + "\r\n" 
                        +"Avoid the white lights for now!" + "\r\n"
                        +"The green lights are the vision zones of guards" + "\r\n"
                        + "Further instruction on the top right corner";
    }

}

