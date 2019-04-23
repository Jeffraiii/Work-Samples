using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class LightState_Tutorial : MonoBehaviour {

    public GameObject player;
    public GameObject PopupText;
    public Text theText;

    void Start () {
        theText.text = "Subject 90007, you are now in Light State";
    }
	
	void Update () {
        if (player.transform.position.z >= -10.75f && player.transform.position.z <= -6.25f)
        {
            PopupText.SetActive(false);
            theText.text = "While you are in Light State, staying in the lights makes you invisible" + "\r\n" 
                           + "Now darkness expose you to guards' visions";
        }

        if (player.transform.position.z >= -6.25f && player.transform.position.z <= -3.5f)
        {
            theText.text = "Your current state is always displayed on the left down corner of the screen";
        }

        if (player.transform.position.z >= -3.5f)
        {
            theText.text = "Remember, Light State for lights!!!";
        }
    }
}
