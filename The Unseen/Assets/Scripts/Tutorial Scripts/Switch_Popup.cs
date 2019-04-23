using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Switch_Popup : MonoBehaviour {

    public Text PopupText;

    void Start()
    {
        PopupText.text = "There are mechanism that you use to change environment" + "\r\n"
                        +"There are levers and buttons"
                        + "Further instruction on the top right corner";
    }
}
