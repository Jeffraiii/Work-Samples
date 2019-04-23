using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class DarkState_Tutorial : MonoBehaviour {

    public GameObject player;
    public GameObject PopupText;
    public Text theText;

    public void Start()
    {
        theText.text = "Subject 90007, use WASD/LEFT JOYSTICK to move around";
    }
    void Update () {

        if (player.transform.position.z >= -11.00f && player.transform.position.z <= -6.25f)
        {
            PopupText.SetActive(false);
            theText.text = "While you are in Dark State, staying in the dark makes you invisible" + "\r\n"
                            +"But white lights expose you to guards' visions";
        }

        if (player.transform.position.z >= -6.25f)
        {
            theText.text = "While being invisible, do not be afraid of the guards";
        }
	}
}
