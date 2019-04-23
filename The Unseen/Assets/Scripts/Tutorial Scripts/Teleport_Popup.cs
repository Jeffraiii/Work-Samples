using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Teleport_Popup : MonoBehaviour {

    public Text PopupText;

    void Start()
    {
        PopupText.text = "You can switch between Dark and Light States by teleporting" + "\r\n"
                        + "Press SPACE/TRIANGLE to perform teleport" + "\r\n"
                        + "Use O /◻︎◻︎ SQUARE◻︎ to see things differently" + "\r\n"
                        + "Further instruction on the top right corner";
    }
}
