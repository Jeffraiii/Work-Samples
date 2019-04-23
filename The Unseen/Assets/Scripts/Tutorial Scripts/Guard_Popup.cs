using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Guard_Popup : MonoBehaviour {

    public Text PopupText;

    void Start()
    {
        PopupText.text = "From now on, most guard drones would be patrolling" + "\r\n"
                        + "Use the correct states to remain invisible" + "\r\n"
                        + "Use O /◻︎◻︎ SQUARE◻︎ to see further" + "\r\n"
                        + "Further instruction on the top right corner";
    }
}
