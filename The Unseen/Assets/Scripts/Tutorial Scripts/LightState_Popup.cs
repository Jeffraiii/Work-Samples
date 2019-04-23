using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class LightState_Popup : MonoBehaviour {

    public Text PopupText;

    void Start()
    {
        PopupText.text = "As opposite, you are in Light State" + "\r\n"
                        + "Stay in the white lights for now!" + "\r\n"
                        + "The green lights are still the vision zones of guards" + "\r\n"
                        + "Further instruction on the top right corner";
    }
}
